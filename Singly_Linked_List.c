#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <asm/current.h>
#include <linux/slab.h>//用來使用kmalloc
#include <linux/delay.h>
#include <linux/mutex.h>




struct my_data {
    int pid;
    struct  my_data *next;
};

struct my_data *first=NULL,*prev=NULL;


DECLARE_WAIT_QUEUE_HEAD(my_wait_queue);

static DEFINE_MUTEX(my_mutex); 

static int condition = 0;  


static int enter_wait_queue(void){
    int pid = (int)current->pid;
       printk("add to wait queue\n");
    struct my_data *entry=kmalloc(sizeof(*entry), GFP_KERNEL);
	entry->pid = pid;
printk("Added process with pid=%d to my_list\n", entry->pid);
    if(prev!=NULL){
		prev->next=entry;
		entry->next=NULL;
	}
	else{
		first=entry;
	}
    
    wait_event_interruptible(my_wait_queue, condition == pid);
    
	prev=entry;
	
    return 0;
}



static int clean_wait_queue(void){
    struct my_data *entry = first;
    while(entry) {
        struct my_data *temp = entry;  
        condition = entry->pid;
        wake_up_interruptible(&my_wait_queue); 
        msleep(100);
        entry = entry->next;  
        kfree(temp);  
    }
    first = NULL; 
    prev = NULL;
    return 0;
}

SYSCALL_DEFINE1(call_my_wait_queue, int, id){
    switch (id){
        case 1:
            mutex_lock(&my_mutex); //避免兩個process互相競爭造成race condition
            enter_wait_queue();
            mutex_unlock(&my_mutex);
            break;
        case 2:
            clean_wait_queue();
            break;
    }
    return 0;
}



