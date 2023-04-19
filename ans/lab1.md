# 实验1 - 实验报告

## 思考题1

`mpidr_el1`寄存器用于获取当前cpu核心的编号。  
`mrs	x8, mpidr_el1`将当前cpu核心编号存储在寄存器x8中；`and	x8, x8,	#0xFF`为x8中的值做了mask，将最后1 byte之前的位全部置0；`cbz	x8, primary`判断x8中的值是否为0，如为0则该核为主要核心，跳转到primary函数进行初始化操作，如不为0则暂时将该核挂起，等到引入smp时再继续执行。

## 练习题2

填入`mrs x9, CurrentEL`，用gdb调试得到执行完该步后x9的值为0xc，处于el3。

## 练习题3

    adr x9, .Ltarget
	msr elr_el3, x9
	mov x9, SPSR_ELX_DAIF | SPSR_ELX_EL1H
	msr spsr_el3, x9

	isb
	eret

降级后需直接返回`_start`函数，所以将`elr_el3`设置为`.Ltarget`。需屏蔽所有中断并使用内核栈，所以将`spsr_el3`设置为`SPSR_ELX_DAIF | SPSR_ELX_EL1H`.

## 思考题4

`kernel.img`的反汇编中，`init_c`函数使用了栈。

    stp x29, x30, [sp, #-16]!
    mov x29, sp
    ...
    ldp	x29, x30, [sp], #16

C函数依赖栈运行，所以在运行C代码之前必须要设置启动栈。如不设置栈，则C函数无法在栈上存储临时变量，在调用函数时也无法在栈上存储信息，C函数不能正常执行。

## 思考题5
`.bss`中存储了未初始化或初始化为0的全局变量。如在后续过程中需用到初始化为0的全局变量，但该变量未在加载时初始化为0，程序就会出错。

## 练习题6
```c
while (1) {
    if (early_uart_lsr() & 0x20)
            break;
}
int pos = 0;
while (str[pos] != '\0') {
        early_put32(AUX_MU_IO_REG, str[pos]);   
        ++pos;
}
```

利用已经实现的`early_put32`，依次输出字符串中的字符，遇到`\0`停止输出。

运行qemu后输出：

    boot: init_c
    [BOOT] Install boot page table
    [BOOT] Enable el1 MMU
    [BOOT] Jump to kernel main

## 练习题7

`orr x8, x8, #SCTLR_EL1_M`设sctlr_el1的M位为1。
