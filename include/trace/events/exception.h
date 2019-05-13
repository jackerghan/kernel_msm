/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM exception

#if !defined(_TRACE_EXCEPTION_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_EXCEPTION_H

#include <linux/tracepoint.h>

struct task_struct;

TRACE_EVENT(user_fault,

	TP_PROTO(struct task_struct *tsk, unsigned long addr, unsigned int fsr),

	TP_ARGS(tsk, addr, fsr),

	TP_STRUCT__entry(
		__string(task_name, tsk->comm)
		__field(unsigned long, addr)
		__field(unsigned int, fsr)
	),

	TP_fast_assign(
	__assign_str(task_name, tsk->comm)
		__entry->addr	= addr;
		__entry->fsr	= fsr;
	),

	TP_printk("task_name:%s addr:%lu, fsr:%u", __get_str(task_name),\
		__entry->addr, __entry->fsr)
);

struct pt_regs;

TRACE_EVENT(undef_instr,

	TP_PROTO(struct pt_regs *regs, void *prog_cnt),

	TP_ARGS(regs, prog_cnt),

	TP_STRUCT__entry(
		__field(void *, prog_cnt)
		__field(struct pt_regs *, regs)
	),

	TP_fast_assign(
		__entry->regs		= regs;
		__entry->prog_cnt	= prog_cnt;
	),

	TP_printk("pc:%p", __entry->prog_cnt)
);

TRACE_EVENT(unhandled_abort,

	TP_PROTO(struct pt_regs *regs, unsigned long addr, unsigned int fsr),

	TP_ARGS(regs, addr, fsr),

	TP_STRUCT__entry(
		__field(struct pt_regs *, regs)
		__field(unsigned long, addr)
		__field(unsigned int, fsr)
	),

	TP_fast_assign(
		__entry->regs	= regs;
		__entry->addr	= addr;
		__entry->fsr	= fsr;
	),

	TP_printk("addr:%lu, fsr:%u", __entry->addr, __entry->fsr)
);

TRACE_EVENT(kernel_panic,

	TP_PROTO(long dummy),

	TP_ARGS(dummy),

	TP_STRUCT__entry(
		__field(long, dummy)
	),

	TP_fast_assign(
		__entry->dummy	= dummy;
	),

	TP_printk("dummy:%ld", __entry->dummy)
);

TRACE_EVENT(kernel_panic_late,

	TP_PROTO(long dummy),

	TP_ARGS(dummy),

	TP_STRUCT__entry(
		__field(long, dummy)
	),

	TP_fast_assign(
		__entry->dummy	= dummy;
	),

	TP_printk("dummy:%ld", __entry->dummy)
);

TRACE_EVENT(page_fault_filt,

       TP_PROTO(unsigned long addr, unsigned long faulting_pc, unsigned long vma_start, unsigned long vma_end, unsigned long vma_pgoff, unsigned int flags, dev_t dev, ino_t ino, char* path),

       TP_ARGS(addr, faulting_pc, vma_start, vma_end, vma_pgoff, flags, dev, ino, path),

       TP_STRUCT__entry(
				 			 __field( unsigned long, addr )
							 __field( unsigned long, faulting_pc )
							 __field( unsigned long, vma_start )
							 __field( unsigned long, vma_end )
							 __field( unsigned long, vma_pgoff )
				 			 __field( unsigned int, flags )
							 __field( dev_t, dev )
							 __field( ino_t, ino )
							 __string( path,	path	)
       ),

       TP_fast_assign(
				 			 __entry->addr = addr;
							 __entry->faulting_pc = faulting_pc;
							 __entry->vma_start = vma_start;
							 __entry->vma_end = vma_end;
							 __entry->vma_pgoff = vma_pgoff;
							 __entry->flags = flags;
							 __entry->dev = dev;
							 __entry->ino = ino;
							 __assign_str( path, path );
       ),

       TP_printk("%lx %lx %lx %lx %lx %x %d,%d %lu [%s]",
			 					 __entry->addr,
								 __entry->faulting_pc,
								 __entry->vma_start,
								 __entry->vma_end,
								 __entry->vma_pgoff,
								 __entry->flags,
								 MAJOR(__entry->dev), MINOR(__entry->dev),
								 (unsigned long)__entry->ino, __get_str(path))
);

#endif

#include <trace/define_trace.h>
