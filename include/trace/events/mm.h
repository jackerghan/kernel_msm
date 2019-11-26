#undef TRACE_SYSTEM
#define TRACE_SYSTEM mm

#if !defined(_TRACE_MMAP_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_MMAP_H

#include <linux/sched.h>
#include <linux/tracepoint.h>
#include <linux/binfmts.h>

/*
 * Tracepoint for calling mmap:
 */
TRACE_EVENT(mmap,

	TP_PROTO(struct vm_area_struct *vma, dev_t dev, ino_t ino, char* name),

	TP_ARGS(vma, dev, ino, name),

	TP_STRUCT__entry(
		__field( unsigned long,	vm_start)
		__field( unsigned long,	vm_end)
		__field( unsigned long, vm_pgoff )
		__field( unsigned long, vm_flags )
		__field( dev_t, dev )
		__field( ino_t, ino )
		__string( name,	name	)
	),

	TP_fast_assign(
		__entry->vm_start	= vma->vm_start;
		__entry->vm_end	= vma->vm_end;
		__entry->vm_pgoff = ((unsigned long)vma->vm_pgoff) << PAGE_SHIFT;
		if (__entry->vm_pgoff == __entry->vm_start) {
			__entry->vm_pgoff = 0;
		}
		__entry->vm_flags	= vma->vm_flags;
		__entry->dev = dev;
		__entry->ino = ino;
		__assign_str( name, name );
	),

	TP_printk("%lx %lx %lx %lx %d,%d %lu [%s]",
						__entry->vm_start,
						__entry->vm_end,
						__entry->vm_pgoff,
						__entry->vm_flags,
						MAJOR(__entry->dev), MINOR(__entry->dev),
						(unsigned long)__entry->ino,
						__get_str(name))
);

/*
 * Tracepoint for calling set_anon_name:
 */
TRACE_EVENT(set_anon_name,

	TP_PROTO(long start, long end, char* name),

	TP_ARGS(start, end, name),

	TP_STRUCT__entry(
		__field( unsigned long,	start)
		__field( unsigned long,	end)
		__string( name,	name	)
	),

	TP_fast_assign(
		__entry->start	= start;
		__entry->end	= end;
		__assign_str( name, name );
	),

	TP_printk("%lx %lx [%s]",
						__entry->start,
						__entry->end,
						__get_str(name))
);

#endif /* _TRACE_MMAP_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
