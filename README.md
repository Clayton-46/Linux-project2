# 完整筆記

https://hackmd.io/@clayton46/HJTilmur1l/edit

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
