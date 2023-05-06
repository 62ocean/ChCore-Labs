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

#include <semaphore/semaphore.h>
#include <sched/sched.h>
#include <object/thread.h>
#include <sched/context.h>
#include <irq/irq.h>

void init_sem(struct semaphore *sem)
{
        sem->sem_count = 0;
        sem->waiting_threads_count = 0;
        init_list_head(&sem->waiting_threads);
}

/*
 * Lab4
 * Return 0 if wait successfully, -EAGAIN otherwise
 * Hint: use sched() and eret_to_thread if current thread should be blocked.
 * You should also update the state of current thread to TS_WAITING
 * and set the return value of the current thread.
 * Besides, you should `obj_put` the sem before eret to the new thread.
 */
s32 wait_sem(struct semaphore *sem, bool is_block)
{
        kdebug("wait----------\n");
        kdebug("sem: %lx, sem_count: %d, sem_waiting_thread_count: %d\n", sem, sem->sem_count, sem->waiting_threads_count);
        s32 ret = 0;
        /* LAB 4 TODO BEGIN */
        if (sem->sem_count > 0) {
                sem->sem_count--;
                kdebug("sem: %lx, sem_count: %d, sem_waiting_thread_count: %d\n", sem, sem->sem_count, sem->waiting_threads_count);
                return 0;
        }
        if (!is_block) {
                return -EAGAIN;
        }

        current_thread->thread_ctx->state = TS_WAITING;
        sem->waiting_threads_count++;
        list_append(&current_thread->sem_queue_node, &sem->waiting_threads);
        kdebug("sem: %lx, sem_count: %d, sem_waiting_thread_count: %d\n", sem, sem->sem_count, sem->waiting_threads_count);

        // if(current_thread && current_thread->thread_ctx)
	// 	current_thread->thread_ctx->sc->budget = 0;

        obj_put(sem);      // why??  
        // kdebug("before sched:\n");
        // sched_top();
        sched();
        // kdebug("after sched:\n");
        // sched_top();
        eret_to_thread(switch_context());
        /* LAB 4 TODO END */
        return ret;
}


/*
 * Lab4
 * Return 0 if signal successfully
 * Hint: use `list_entry` to find the wakeup target in the waiting_threads list
 * Remember to delete the thread from the list using `list_del`.
 * Enqueue it to the ready queue rather than directly switch to it.
 */
s32 signal_sem(struct semaphore *sem)
{
        kdebug("signal----------\n");
        kdebug("sem: %lx, sem_count: %d, sem_waiting_thread_count: %d\n", sem, sem->sem_count, sem->waiting_threads_count);
        /* LAB 4 TODO BEGIN */
        // if (sem->waiting_threads_count > 0) {
        //         struct thread *target_thread = list_entry(sem->waiting_threads.next, struct thread, wait_list_node);
        //         list_del(&target_thread->wait_list_node);
        //         target_thread->wait_obj = NULL;
        //         target_thread->wait_retval = NULL;
        //         target_thread->state = TS_RUNNABLE;
        //         sem->waiting_threads_count--;
        //         add_to_ready_queue(target_thread);
        // }
        if (sem->waiting_threads_count == 0) {
                sem->sem_count++;
                kdebug("sem: %lx, sem_count: %d, sem_waiting_thread_count: %d\n", sem, sem->sem_count, sem->waiting_threads_count);
                return 0;
        }
        
        sem->waiting_threads_count--;
        struct thread *wakeup_thread = list_entry(sem->waiting_threads.next, struct thread, sem_queue_node);
        list_del(&wakeup_thread->sem_queue_node);
        sched_enqueue(wakeup_thread);
        kdebug("sem: %lx, sem_count: %d, sem_waiting_thread_count: %d\n", sem, sem->sem_count, sem->waiting_threads_count);
        // if (current_thread->thread_ctx->type == TYPE_IDLE) {
        //         sys_yield();
        // }
        /* LAB 4 TODO END */
        return 0;
}


s32 sys_create_sem(void)
{
        struct semaphore *sem = NULL;
        int sem_cap = 0;
        int ret = 0;

        sem = obj_alloc(TYPE_SEMAPHORE, sizeof(*sem));
        if (!sem) {
                ret = -ENOMEM;
                goto out_fail;
        }
        init_sem(sem);

        sem_cap = cap_alloc(current_cap_group, sem, 0);
        if (sem_cap < 0) {
                ret = sem_cap;
                goto out_free_obj;
        }

        return sem_cap;
out_free_obj:
        obj_free(sem);
out_fail:
        return ret;
}

s32 sys_wait_sem(u32 sem_cap, bool is_block)
{
        struct semaphore *sem = NULL;
        int ret;

        sem = obj_get(current_thread->cap_group, sem_cap, TYPE_SEMAPHORE);
        if (!sem) {
                ret = -ECAPBILITY;
                goto out;
        }
        ret = wait_sem(sem, is_block);
        obj_put(sem);
out:
        return ret;
}

s32 sys_signal_sem(u32 sem_cap)
{
        struct semaphore *sem = NULL;
        int ret;
        sem = obj_get(current_thread->cap_group, sem_cap, TYPE_SEMAPHORE);
        if (!sem) {
                ret = -ECAPBILITY;
                goto out;
        }
        ret = signal_sem(sem);
        obj_put(sem);
out:
        return ret;
}
