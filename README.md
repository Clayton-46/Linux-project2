# 完整筆記

https://hackmd.io/@clayton46/HJTilmur1l/edit

## 整體概述

#### 等待佇列 (Wait Queue) 是一種 同步機制 (Synchronization Mechanism)，主要用來讓Process 進入睡眠狀態，直到某個條件被滿足後再喚醒進程，本次 Project 在Kernel Space 實作一個自訂的等待佇列機制，並透過 系統呼叫 (System Call) 讓 使用者應用程式 (User Applications) 進行操作。了解到如何避免忙等待 (Busy-Waiting)，提升系統效能、如何在核心 (Kernel) 與使用者空間 (User Space) 之間建立介面、掌握 FIFO 原則在同步機制中的應用等等。

#### Environment
```
OS: Ubuntu 22.04

ARCH: X86_64

Source Version: 5.15.137
```

# trace code

![image](https://github.com/user-attachments/assets/4fd65ec1-657c-4d84-9794-468170c523ca)

disk_wait_queue 的 task_list 是鏈表的頭部節點。

task_list：實現雙向鏈表的核心數據結構。包含：

next：指向下一個節點。 prev：指向上一個節點。

flags：表示任務的狀態或屬性。

private：指向與該節點相關的進程描述符 (process descriptor)。

func：指向回調函數，用於喚醒任務時執行特定操作。

task_list：雙向鏈表指標，鏈接到等待佇列的其他節點。

每個節點的 task_list.next 和 task_list.prev 連接到其他節點，形成一個環狀的雙向鏈表。

#### ().DEFINE_MUTEX(my_mutex)

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/mutex.h

![image](https://github.com/user-attachments/assets/d1a9c6ae-6f65-47cb-9a89-2259c12b721b)

#### ().kmalloc以下圖可以知道括弧內前面欄位放分配的內存size，後面欄位放內存分配標誌(指定分配過程中的行為限制，決定是否允許阻塞、是否允許進行 I/O 操作等。)

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/slab.h

![image](https://github.com/user-attachments/assets/463cc001-91ba-43b5-a66d-6e5e630c320e)

#### ().GFP_KERNEL可以允許睡眠、I/O 和文件系統操作

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/gfp.h

![image](https://github.com/user-attachments/assets/0abaec1b-fc6f-40ff-afa9-43308bf9a4ca)

1.允許回收內存（__GFP_RECLAIM）。

2.允許執行 I/O 操作（__GFP_IO）。

3.允許文件系統相關操作（__GFP_FS）。

這些都是位元組標誌（bit flag）

#### ().DECLARE_WAIT_QUEUE_HEAD(name)

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/wait.h

![image](https://github.com/user-attachments/assets/ddff748f-914c-4f59-84aa-5d4966956e97)

![image](https://github.com/user-attachments/assets/36578b72-395b-49ec-9785-4e0539dabec6)

![image](https://github.com/user-attachments/assets/03aaa7bd-88be-4f75-80d4-8c9318858815)

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/list.h#L21

![image](https://github.com/user-attachments/assets/b2c219ee-2c31-44a7-a814-70e02f4fdd51)

#### ().LIST_HEAD()

![image](https://github.com/user-attachments/assets/53568e5b-cde5-4995-b4f3-cf48a99cbd6e)



#### ().list_add_tail(&wq_entry->entry, &wq_head->head);

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/wait.h

![image](https://github.com/user-attachments/assets/81e1643d-543b-486f-88d7-c16c0cc854c5)

entry->list 表示新節點。

my_list 是主鏈表。

將 entry->list 作為新節點加入到 my_list 的尾部。

運行後，my_list 的尾節點會更新為 entry->list，保證後續新增的節點繼續添加到尾部，維持 FIFO 順序。


https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/list.h#L90

![image](https://github.com/user-attachments/assets/de82473f-621e-48bc-a9cb-3839e9c283d7)


https://elixir.bootlin.com/linux/v5.15.137/source/tools/include/linux/list.h#L48

![image](https://github.com/user-attachments/assets/8e54eb2e-1d1e-496a-a9f1-a92a562ecde6)

## result 

![image](https://github.com/user-attachments/assets/9812267a-4500-445d-b322-5cde9220921c)

### FIFO實作重點整理

##### 1.Thread 的並行建立與執行

在 pthread_create 建立 thread 時，thread 會立即開始執行（並行執行）。

每個 thread 的第一個動作是輸出 "enter wait queue thread_id:XX" 並執行進入 wait queue 的操作。

由於是並行執行，輸出順序不一定按 thread_id 的建立順序（例如，thread_id 較大的 thread 可能會先輸出）。

##### 2.輸出與執行順序的差異

雖然 for 迴圈依序建立 thread，thread 執行的時間差極小，因此相鄰 thread 的執行順序可能交錯。

enter wait queue thread_id:XX 的輸出順序不代表實際進入 wait queue 的順序。

##### 3.System Call 的進入順序不確定性

輸出 "enter wait queue thread_id:XX" 的 thread 不一定會先執行 syscall(xxx, 1)。

並行執行情況下，thread 執行 system call 的順序可能被相鄰 thread 超越，但相鄰 thread 出現超越的機率較低。

##### 4.FIFO 排隊的實現

使用 Linked list 記錄每個 thread 實際執行 system call 的順序（以 PID 為基準）。

設置全域變數 PID_add_to_wait_queue，保證每個 thread 依照記錄順序進入 wait queue。

在喚醒過程中，依照 Linked list 的順序修改喚醒條件 condition，確保喚醒順序與進入順序一致（FIFO）。

##### 5.輸出一致性

為避免喚醒過程中 condition 被提前更改，每次修改 condition 後增加等待時間，確保 thread 被正確喚醒。

喚醒後，先被喚醒的 thread 會先輸出 "exit wait queue thread_id:XX"，使輸出順序與實際 FIFO 喚醒順序一致。

##### 6.未解決的不確定性

Thread 從輸出 "enter wait queue thread_id:XX" 到執行 syscall(xxx, 1) 並加入 list 的過程中，可能出現後輸出的 thread 先執行 system call 的情況（相鄰 thread 的機率較高，但整體機率低）。



