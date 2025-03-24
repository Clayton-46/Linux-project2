# 完整筆記

https://hackmd.io/@clayton46/HJTilmur1l/edit

## 整體概述

#### 等待佇列 (Wait Queue) 是一種 同步機制 (Synchronization Mechanism)，主要用來讓Process 進入睡眠狀態，直到某個條件被滿足後再喚醒進程，本次 Project 在Kernel Space 實作一個自訂的等待佇列機制，並透過 系統呼叫 (System Call) 讓 使用者應用程式 (User Applications) 進行操作。了解到如何避免忙等待 (Busy-Waiting)，提升系統效能、如何在核心 (Kernel) 與使用者空間 (User Space) 之間建立介面、掌握 FIFO 原則在同步機制中的應用等等。

# Wait Queue

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


https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/list.h#L90

![image](https://github.com/user-attachments/assets/de82473f-621e-48bc-a9cb-3839e9c283d7)


https://elixir.bootlin.com/linux/v5.15.137/source/tools/include/linux/list.h#L48

![image](https://github.com/user-attachments/assets/8e54eb2e-1d1e-496a-a9f1-a92a562ecde6)


