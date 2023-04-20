# 实验4 - 实验报告

## 思考题1

`_start`函数首先检查存储在`mpidr_el1`中的`CPU ID`，以确定它是否为主核心。如果不是，则函数等待主核心清除`BSS`段，然后从其他异常级别转到`EL1`。然后，它准备堆栈指针并跳转到`C`。

如果是主核心，则函数从其他异常级别转到`EL1`，准备堆栈指针并跳转到`C`。最后，函数调用`init_c`开始初始化流程。

为了暂停其他核心的执行，`ChCore`使用名为`secondary_boot_flag`的标志。函数等待标志设置为非零值，然后设置`CPU ID`并调用`secondary_init_c`来初始化辅助核心。

## 思考题2

是物理地址。  
在`init_c`函数中将`secondary_boot_flag`作为参数传递给`start_kernel`，`start_kernel`将其放在`x0`寄存器中传递给`main`函数，`main`函数调用`enable_smp_cores`将第一个参数即`secondary_boot_flag`传递给`enable_smp_cores`。

## 练习题3

`enable_smp_cores`：  
设置flag：`secondary_boot_flag[i] = 1`，等待某个cpu启动：`while (cpu_status[i] == cpu_hang) {}`  

`secondary_start`：   
设cpu状态为run：`cpu_status[cpuid] = cpu_run`

## 练习题4

见代码注释。

## 思考题5

不需要。  
因为在执行完`bl unlock_kernel`后用户态寄存器的值会从栈中恢复，所以只需保证放锁在恢复寄存器之前即可。

## 思考题6

`idle_threads`只会在队列为空时运行，如加入调度队列，可能会抢占其他线程，并引起不必要的开销。