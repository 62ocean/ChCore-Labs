# 实验2 - 实验报告

## 思考题1

优：当应用进程使用的虚拟地址远小于总的虚拟空间时，多级页表更节省空间（因为没有用到的页表不会申请）。  
劣：当应用进程使用的虚拟地址与总虚拟空间差不多时，多级页表更占用空间（因为有多余的L0,L1,L2页表）。多级页表的查询过程更复杂。  

4KB:  
1个L0，4个L1，$512 * 4$个L2，$512 * 4 * 4$个L3。总计1050629。  
2MB:  
1个L0，4个L1，$512 * 4$个L2，无L3。总计2053。

## 练习题2

内核高地址页表从`KERNEL_VADDR`开始，需要配置0-1G范围的页表。  `boot_ttbr1_l0`和`boot_ttbr1_l1`中只需添加一个映射，`boot_ttbr1_l2`中只需将`PHYSMEM_START+KERNEL_VADDR ~ PHYSMEM_END+KERNEL_VADDR`范围的虚拟地址映射为`PHYSMEM_START ~ PHYSMEM_END`范围的物理地址。

## 思考题3

刚开启MMU时，ChCore使用的是虚拟地址，但此时还没有跳转到高地址位置，所以需要配置页表使虚拟地址与物理地址一一对应，等到跳转到高地址区域后再使用高地址页表。  

验证：去掉为低地址配置的页表，程序不能正常跳转到main函数。

## 思考题4
`kernel/arch/aarch64/boot/raspi3/init/tools.s`226行的`el1_mmu_activate`函数中
```
adrp    x8, boot_ttbr0_l0
msr     ttbr0_el1, x8
adrp    x8, boot_ttbr1_l0
msr     ttbr1_el1, x8
isb
```
将`boot_ttbr0_l0`数组的位置和`boot_ttbr1_l0`数组的位置分别放入了`ttbr0_el1`和`ttbr1_el1`寄存器。  
isb是指令同步屏障，确保所有在ISB指令之后的指令都从指令高速缓存或内存中重新预取。这样可以保证isb指令之后的指令都是使用新的页表获取地址。

## 练习题5

`split_page`：将page不断分割为原来的一半，直到大小正好为2^order。  
`buddy_get_pages`：找到满足要求的最小的块，将其分割为所需大小并返回。  
`merge_page`：释放某一page时寻找其buddy page并尝试合并，直到不能合并为止。  
`buddy_free_pages`：释放page并调用`merge_page`合并。

## 练习题6

`get_next_ptp`需要写的部分：如该页未分配而需分配，则创建一个物理页并将页表项设为该物理页，同时设置一些属性。  
`query_in_pgtbl`：分别查询0、1、2、3级页表，在第3级页表取出所查询的物理地址并返回。  
`map_range_in_pgtbl`：添加页表映射。按照0、1、2、3级寻找并设置相应页表项，如不存在页表页则申请。  
`unmap_range_in_pgtbl`：删除页表映射。按照0、1、2、3级找到相应的页表项，并将其`is_valid`位置为0.  

## 练习题7

`map_range_in_pgtbl_huge`：从大页到小页申请，没有大页可申请的情况下申请下一级的小页，直到申请的空间满足要求。  
`unmap_range_in_pgtbl_huge`：从0、1、2、3级页表向下查询时，先判断是否为BLOCK，如是则直接释放大页，不是则继续向下查询，最终释放小页。  
`query_in_pgtbl`：在0、1、2级页表的查询后添加是否为BLOCK的判断，如是直接返回大页。

## 思考题8

在操作系统中支持写时拷贝需要配置页表描述符的AP字段。AP字段是访问权限字段，它指定了访问该页的权限。在写时拷贝中，AP字段应该设置为只读，以便多个进程可以共享同一块内存。  

当进程试图写入只读页时，会发生缺页异常。在这种情况下，操作系统会为该进程分配一个新的物理页，并将该页的内容复制到新的物理页中。然后，操作系统会更新页表，以便该页指向新的物理页。这样，该进程就可以修改其私有副本，而不会影响其他进程。

## 思考题9

如果需要更精细的映射，使用粗粒度映射表可能会导致映射的精度不够，无法满足需求。容易产生内部碎片。