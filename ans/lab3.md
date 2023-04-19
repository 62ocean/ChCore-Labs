# 实验3 - 实验报告

## 思考题1
内核进行完`uart_init()`、`mm_init()`、`arch_interrupt_init()`初始化后，调用`create_root_thread()`创建第一个进程并载入用户程序。  

在该函数中，首先调用`create_root_cap_group`创建第一个进程，再调用`__create_root_thread`在已创建的进程中创建第一个线程。  

在`__create_root_thread`函数中，为线程添加stack的映射，并调用`load_binary`向虚拟地址空间中载入用户程序代码。  

执行完`__create_root_thread`函数后，调用`switch_to_thread`函数将current_thread设为新创建的thread。  

执行完`create_root_cap_group`函数后，执行`eret_to_thread(switch_context())`语句切换虚拟地址空间和上下文，并最终通过`exception_exit`跳转到用户程序。

## 练习题2

`cap_group_init`：设置pid，初始化slot_table和thread_list。  
`sys_create_cap_group`：分别对cap_group和vmspace进行分配obj、初始化、添加到能力组中的操作。  
`create_root_cap_group`：同上，但使用的pid为ROOT_PID和ROOT_PCID。  

## 练习题3

```c
/* LAB 3 TODO BEGIN */
// 虚拟地址页对齐
vaddr_t vaddr_start = ROUND_DOWN(p_vaddr, PAGE_SIZE);
vaddr_t vaddr_end = ROUND_UP(p_vaddr + seg_sz, PAGE_SIZE);
seg_map_sz = vaddr_end - vaddr_start;
// 创建所需大小的pmo，PMO_DATA类型马上分配物理内存空间
ret = create_pmo(seg_map_sz, PMO_DATA, cap_group, &pmo);
// 将elf文件中的内容拷贝到物理内存空间中
u64 start_offset = p_vaddr - vaddr_start;
char *pmo_start = phys_to_virt(pmo->start) + start_offset;
char *seg_start = bin + elf->p_headers[i].p_offset;
u64 copy_size = elf->p_headers[i].p_filesz;
memcpy(pmo_start, seg_start, copy_size);

flags = PFLAGS2VMRFLAGS(elf->p_headers[i].p_flags);

// 添加vmregion并关联到上述pmo（即添加虚拟地址空间）
ret = vmspace_map_range(vmspace, p_vaddr, seg_sz, flags, pmo);
/* LAB 3 TODO END */
```

## 练习题4

```armasm
.align	11
EXPORT(el1_vector)
/* LAB 3 TODO BEGIN */
exception_entry sync_el1t
exception_entry irq_el1t
exception_entry fiq_el1t
exception_entry error_el1t

exception_entry sync_el1h
exception_entry irq_el1h
exception_entry fiq_el1h
exception_entry error_el1h

exception_entry sync_el0_64
exception_entry irq_el0_64
exception_entry fiq_el0_64
exception_entry error_el0_64

exception_entry sync_el0_32
exception_entry irq_el0_32
exception_entry fiq_el0_32
exception_entry error_el0_32
/* LAB 3 TODO END */
```
对于`sync_el1h`类型的异常，跳转`handle_entry_c`使用 C 代码处理异常。对于`irq_el1t、fiq_el1t、fiq_el1h、error_el1t、error_el1h、sync_el1t`则跳转`unexpected_handler`处理异常。

## 练习题5

`ret = handle_trans_fault(current_thread->vmspace, fault_addr)`

## 练习题6

```c
pa = get_page_from_pmo(pmo, index);
if (pa == 0) {
    /* Not committed before. Then, allocate the physical
        * page. */
    // 申请物理页并添加到pmo中
    pa = virt_to_phys((vaddr_t)get_pages(0));
    commit_page_to_pmo(pmo, index, pa);
    // 添加映射
    map_range_in_pgtbl(vmspace->pgtbl, fault_addr, pa, PAGE_SIZE, perm);
} else {
    // 添加映射
    map_range_in_pgtbl(vmspace->pgtbl, fault_addr, pa, PAGE_SIZE, perm);
}
```

## 练习题7

`exception_enter`：在栈中保存x1-x30通用寄存器和sp_el0、elr_el1、spsr_el1寄存器的值。  
`exception_exit`：从栈中恢复x1-x30通用寄存器和sp_el0、elr_el1、spsr_el1寄存器的值，并执行eret指令回到用户态。  

## 思考题8

系统调用对应的异常向量为`sync_el0_64`，程序进入异常处理函数，首先根据`esr_el1`判断是不是系统调用，如是系统调用则进入`el0_syscall`函数。在`el0_syscall`函数中，根据x8中存储的syscall number在syscall_table中找到相对应的系统函数入口。

## 练习题9
```c
void sys_putc(char ch)
{
    uart_send(ch);
}

u32 sys_getc(void)
{
    return uart_recv();
}

void sys_thread_exit(void)
{
    /* LAB 3 TODO BEGIN */
    struct thread *thread = current_thread;
    
    thread->thread_ctx->thread_exit_state = TE_EXITED;
    thread->thread_ctx->state = TS_EXIT;
    /* LAB 3 TODO END */
    printk("Lab 3 hang.\n");
    while (1) {
    }
    /* Reschedule */
    sched();
    eret_to_thread(switch_context());
}
```
`__chcore_sys_putc`、`__chcore_sys_getc`、`__chcore_sys_thread_exit`：  
调用`__chcore_syscallX`接口进行系统调用。
