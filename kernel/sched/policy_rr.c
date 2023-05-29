/*
 * Copyright (c) 2022 Institute of Parallel And Distributed Systems (IPADS)
 * ChCore-Lab is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v1. You may obtain a copy of Mulan PSL v1 at:
 *     http://license.coscl.org.cn/MulanPSL
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
 * Mulan PSL v1 for more details.
 */

/* Scheduler related functions are implemented here */
#include <sched/sched.h>
#include <arch/machine/smp.h>
#include <common/kprint.h>
#include <machine.h>
#include <mm/kmalloc.h>
#include <common/list.h>
#include <common/util.h>
#include <object/thread.h>
#include <common/macro.h>
#include <common/errno.h>
#include <common/types.h>
#include <object/thread.h>
#include <irq/irq.h>
#include <sched/context.h>

/* in arch/sched/idle.S */
void idle_thread_routine(void);

/* Metadata for ready queue */
struct queue_meta {
        struct list_head queue_head;
        u32 queue_len;
        char pad[pad_to_cache_line(sizeof(u32) + sizeof(struct list_head))];
};

/*
 * rr_ready_queue
 * Per-CPU ready queue for ready tasks.
 */
struct queue_meta rr_ready_queue_meta[PLAT_CPU_NUM];

/*
 * RR policy also has idle threads.
 * When no active user threads in ready queue,
 * we will choose the idle thread to execute.
 * Idle thread will **NOT** be in the RQ.
 */
struct thread idle_threads[PLAT_CPU_NUM];

/*
 * Lab4
 * Sched_enqueue
 * Put `thread` at the end of ready queue of assigned `affinity`.
 * If affinity = NO_AFF, assign the core to the current cpu.
 * If the thread is IDLE thread, do nothing!
 * Do not forget to check if the affinity is valid!
 */
int rr_sched_enqueue(struct thread *thread)
{
        // /* LAB 4 TODO BEGIN */
        // if (thread == NULL || thread->thread_ctx == NULL || thread->thread_ctx->state == TS_READY) {
        //         return -EINVAL;
        // }
        // if (thread->thread_ctx->type == TYPE_IDLE) {
        //         return 0;
        // }
        // s32 cpu_id = thread->thread_ctx->affinity;
        // if (cpu_id == NO_AFF) {
        //         cpu_id = smp_get_cpu_id();
        // }
        // if (cpu_id >= PLAT_CPU_NUM) {
        //           return -EINVAL;
        // }

        // list_append(&thread->ready_queue_node, &rr_ready_queue_meta[cpu_id].queue_head);
        // thread->thread_ctx->cpuid = cpu_id;
        // thread->thread_ctx->state = TS_READY;
        // rr_ready_queue_meta[cpu_id].queue_len++;
        // return 0;
        // /* LAB 4 TODO END */

        if (thread == NULL || thread->thread_ctx == NULL || thread->thread_ctx->state == TS_READY)
	{
		return -EINVAL;
	}
	/* If the thread is IDEL thread, do nothing! */
	if (thread->thread_ctx->type == TYPE_IDLE)
	{
		return 0;
	}

	/* 
	 * If affinity = NO_AFF, assign the core to the current cpu.
	 * 将线程给他指定的cpu运行
	 */
	u32 cpu_id = smp_get_cpu_id();
	if (thread->thread_ctx->affinity != NO_AFF)
	{
		cpu_id = thread->thread_ctx->affinity;
		if (cpu_id >= PLAT_CPU_NUM)
		{
			return -EINVAL;
		}
	}

	list_append(&thread->ready_queue_node, &rr_ready_queue_meta[cpu_id].queue_head);
        rr_ready_queue_meta[cpu_id].queue_len++;
	thread->thread_ctx->state = TS_READY;
	thread->thread_ctx->cpuid = cpu_id; /* [ERROR]: tst_sched_param:120 threads[i]->thread_ctx->cpuid != cpuid*/
	return 0;
}

/*
 * Lab4
 * Sched_dequeue
 * remove `thread` from its current residual ready queue
 * Do not forget to add some basic parameter checking
 */
int rr_sched_dequeue(struct thread *thread)
{
        // /* LAB 4 TODO BEGIN */
        

        // if (thread == NULL || thread->thread_ctx == NULL || 
        // thread->thread_ctx->state != TS_READY || thread->thread_ctx->type == TYPE_IDLE) {
	// 	return -EINVAL;
	// }

	// // if (thread->thread_ctx->type == TYPE_IDLE) {
	// // 	return 0;
	// // }

        // list_del(&thread->ready_queue_node);
	// thread->thread_ctx->state = TS_INTER;
        // rr_ready_queue_meta[smp_get_cpu_id()].queue_len--;
        // return 0;
        // /* LAB 4 TODO END */

        if (thread == NULL || thread->thread_ctx == NULL || thread->thread_ctx->state != TS_READY)
	{
		return -EINVAL;
	}

	/* 如果是空闲线程 则不用出队 */
	if (thread->thread_ctx->type == TYPE_IDLE)
	{
		// thread->thread_ctx->state = TS_INTER;
		return 0;
	}

	list_del(&thread->ready_queue_node);
        rr_ready_queue_meta[smp_get_cpu_id()].queue_len++;
	thread->thread_ctx->state = TS_INTER;
	return 0;
}

/*
 * Lab4
 * Choose an appropriate thread and dequeue from ready queue
 *
 * Hints:
 * You can use `list_entry` to find the next ready thread in the ready queue
 */
struct thread *rr_sched_choose_thread(void)
{
        // struct thread *thread = NULL;
        // /* LAB 4 TODO BEGIN */
        // int cpu_id = smp_get_cpu_id();
        // if (list_empty(&rr_ready_queue_meta[cpu_id].queue_head)) {
        //          thread = &idle_threads[cpu_id];
        // } else {
        //         struct list_head *pos = &rr_ready_queue_meta[cpu_id].queue_head.next;
        //         thread = list_entry(pos, struct thread, ready_queue_node);
        //         if (rr_sched_dequeue(thread) < 0) {
        //                 return NULL;
        //         }

        // }
        
        // /* LAB 4 TODO END */
        // return thread;

        /* 首先检查CPU 核心的rr_ready_queue是否为空
	 * 如果是，rr_choose_thread返回CPU 核心自己的空闲线程 
	 */
	u32 cpu_id = smp_get_cpu_id();
	if (list_empty(&(rr_ready_queue_meta[cpu_id].queue_head)))
	{
		return &(idle_threads[cpu_id]);
	}

	/*
	 * 如果没有，它将选择rr_ready_queue的队首
	 * 并调用rr_sched_dequeue()使该队首出队，然后返回该队首
	 */
	struct thread *chosen_thread = list_entry(rr_ready_queue_meta[cpu_id].queue_head.next, struct thread, ready_queue_node);
	if (rr_sched_dequeue(chosen_thread) < 0)
	{
		return NULL;
	}
	return chosen_thread;
}


/*
 * Lab4
 * You should use this function in rr_sched
 */
static inline void rr_sched_refill_budget(struct thread *target, u32 budget)
{
        /* LAB 4 TODO BEGIN */
        target->thread_ctx->sc->budget = budget;
        /* LAB 4 TODO END */
}

/*
 * Lab4
 * Schedule a thread to execute.
 * This function will suspend current running thread, if any, and schedule
 * another thread from `rr_ready_queue[cpu_id]`.
 *
 * Hints:
 * Macro DEFAULT_BUDGET defines the value for resetting thread's budget.
 * After you get one thread from rr_sched_choose_thread, pass it to
 * switch_to_thread() to prepare for switch_context().
 * Then ChCore can call eret_to_thread() to return to user mode.
 * You should also check the state of the old thread. Old thread
 * could be exiting/waiting or running when calling this function.
 * You will also need to check the remaining budget of the old thread.
 */
int rr_sched(void)
{
        // if (current_thread && current_thread->thread_ctx && current_thread->thread_ctx->sc)
        //         kdebug("current thread: %lx, budget: %d\n", current_thread, current_thread->thread_ctx->sc->budget);

        // kdebug("----------------\n");
        // 对当前线程的处理
        if (current_thread && current_thread->thread_ctx && current_thread->thread_ctx->thread_exit_state == TE_EXITING) {
                //处理退出进程，不将其加入调度队列
                kdebug("exit thread: %lx budget: %d\n", current_thread, current_thread->thread_ctx->sc->budget);
                current_thread->thread_ctx->thread_exit_state = TE_EXITED;
                current_thread->thread_ctx->state = TS_EXIT;
        } else if (current_thread && current_thread->thread_ctx && current_thread->thread_ctx->state == TS_WAITING) {
                // 线程已加入等待队列，不将其加入调度队列
                kdebug("waiting thread: %lx thread exit state: %d budget: %d\n", current_thread, current_thread->thread_ctx->thread_exit_state, current_thread->thread_ctx->sc->budget);
        } else {
                if (current_thread && current_thread->thread_ctx && current_thread->thread_ctx->sc && current_thread->thread_ctx->sc->budget != 0) {
                        // budget != 0, 不调度直接返回
                        kdebug("return thread: %lx thread exit state: %d budget: %d\n", current_thread, current_thread->thread_ctx->thread_exit_state, current_thread->thread_ctx->sc->budget);
                        return 0;
                } else if (current_thread && current_thread->thread_ctx && current_thread->thread_ctx->sc && current_thread->thread_ctx->sc->budget == 0) {
                        // budget == 0, 调度
                        // kdebug("sched thread: %lx thread exit state: %d budget: %d\n", current_thread, current_thread->thread_ctx->thread_exit_state, current_thread->thread_ctx->sc->budget);
                        rr_sched_enqueue(current_thread);
                }
        }
        // kdebug("----------------\n");
        
        
        // else if (current_thread) {
                
        // }
        
	/*
	 * 首先检查当前是否正在运行某个线程
	 * 如果是，它将调用rr_sched_enqueue()将线程添加回rr_ready_queue
	 * rr_ready_queue 里面是不运行的线程哦
	 */
	// if (current_thread != NULL)
	// {
	// 	rr_sched_enqueue(current_thread);
	// }

	struct thread *target_thread;
	if ((target_thread = rr_sched_choose_thread()) == NULL)
	{
		return -EINVAL;
	}

	/* resetting thread's budget */
        // 要在此时设budget，因为IDLE线程不会入队
        if (target_thread->thread_ctx->type == TYPE_IDLE) {
                // 将idle线程的budget设为0，防止有其他线程可调度时idle占用cpu
                rr_sched_refill_budget(target_thread, 0);
        } else {
                rr_sched_refill_budget(target_thread, DEFAULT_BUDGET);
        }
	

        // kdebug("after sched\n");
        // rr_top();

	return switch_to_thread(target_thread);
}


int rr_sched_init(void)
{
        int i = 0;
        char idle_name[] = "KNL-IDLE-RR";
        int name_len = strlen(idle_name);

        struct cap_group *idle_cap_group;
        struct vmspace *idle_vmspace;

        /* Initialize global variables */
        for (i = 0; i < PLAT_CPU_NUM; i++) {
                current_threads[i] = NULL;
                init_list_head(&(rr_ready_queue_meta[i].queue_head));
                rr_ready_queue_meta[i].queue_len = 0;
        }

        /* Create a fake idle cap group to store the name */
        idle_cap_group = kzalloc(sizeof(*idle_cap_group));
        memset(idle_cap_group->cap_group_name, 0, MAX_GROUP_NAME_LEN);
        if (name_len > MAX_GROUP_NAME_LEN)
                name_len = MAX_GROUP_NAME_LEN;
        memcpy(idle_cap_group->cap_group_name, idle_name, name_len);
        init_list_head(&idle_cap_group->thread_list);

        extern struct vmspace *create_idle_vmspace(void);
        idle_vmspace = create_idle_vmspace();

        /* Initialize one idle thread for each core and insert into the RQ */
        for (i = 0; i < PLAT_CPU_NUM; i++) {
                /* Set the thread context of the idle threads */
                BUG_ON(!(idle_threads[i].thread_ctx =
                                 create_thread_ctx(TYPE_IDLE)));
                /* We will set the stack and func ptr in arch_idle_ctx_init */
                init_thread_ctx(&idle_threads[i], 0, 0, MIN_PRIO, TYPE_IDLE, i);
                /* Call arch-dependent function to fill the context of the idle
                 * threads */
                arch_idle_ctx_init(idle_threads[i].thread_ctx,
                                   idle_thread_routine);

                idle_threads[i].cap_group = idle_cap_group;
                idle_threads[i].vmspace = idle_vmspace;

                /* Add idle_threads to the threads list */
                list_add(&idle_threads[i].node, &idle_cap_group->thread_list, false);
        }
        kdebug("Scheduler initialized. Create %d idle threads.\n", i);

        return 0;
}

#define MAX_CAP_GROUP_BUF 256

void rr_top(void)
{
        u32 cpuid;
        struct thread *thread;
        /* A simple way to collect all cap groups */
        struct cap_group *cap_group_buf[MAX_CAP_GROUP_BUF] = {0};
        unsigned int cap_group_num = 0;
        int i = 0;

        // printk("cap_group_num: %d\n", cap_group_num);

        printk("\n*****CPU RQ Info*****\n");
        for (cpuid = 0; cpuid < PLAT_CPU_NUM; cpuid++) {
                printk("== CPU %d RQ LEN %lu==\n",
                       cpuid,
                       rr_ready_queue_meta[cpuid].queue_len);
                thread = current_threads[cpuid];
                if (thread != NULL) {
                        for (i = 0; i < cap_group_num; i++)
                                if (thread->cap_group == cap_group_buf[i])
                                        break;
                        if (i == cap_group_num
                            && cap_group_num < MAX_CAP_GROUP_BUF) {
                                cap_group_buf[cap_group_num] =
                                        thread->cap_group;
                                cap_group_num++;
                        }
                        printk("Current ");
                        print_thread(thread);
                }
                if (!list_empty(&(rr_ready_queue_meta[cpuid].queue_head))) {
                        for_each_in_list (
                                thread,
                                struct thread,
                                ready_queue_node,
                                &(rr_ready_queue_meta[cpuid].queue_head)) {
                                for (i = 0; i < cap_group_num; i++)
                                        if (thread->cap_group
                                            == cap_group_buf[i])
                                                break;
                                if (i == cap_group_num
                                    && cap_group_num < MAX_CAP_GROUP_BUF) {
                                        cap_group_buf[cap_group_num] =
                                                thread->cap_group;
                                        cap_group_num++;
                                }
                                print_thread(thread);
                        }
                }
                printk("\n");
        }
        // printk("cap_group_num: %d\n", cap_group_num);

        printk("\n*****CAP GROUP Info*****\n");
        for (i = 0; i < cap_group_num; i++) {
                printk("== CAP GROUP:%s ==\n",
                       cap_group_buf[i]->cap_group_name);
                for_each_in_list (thread,
                                  struct thread,
                                  node,
                                  &(cap_group_buf[i]->thread_list)) {
                        print_thread(thread);
                }
                printk("\n");
        }
}

struct sched_ops rr = {.sched_init = rr_sched_init,
                       .sched = rr_sched,
                       .sched_enqueue = rr_sched_enqueue,
                       .sched_dequeue = rr_sched_dequeue,
                       .sched_top = rr_top};
