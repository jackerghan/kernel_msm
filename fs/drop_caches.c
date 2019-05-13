/*
 * Implement the manual drop-all-pagecache function
 */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/writeback.h>
#include <linux/sysctl.h>
#include <linux/gfp.h>
#include "internal.h"

/* A global variable is a bit ugly, but it keeps the code simple */
int sysctl_drop_caches;

void drop_pagecache_sb(struct super_block *sb, void *unused)
{
	struct inode *inode, *toput_inode = NULL;

	spin_lock(&inode_sb_list_lock);
	list_for_each_entry(inode, &sb->s_inodes, i_sb_list) {
		spin_lock(&inode->i_lock);
		if ((inode->i_state & (I_FREEING|I_WILL_FREE|I_NEW)) ||
		    (inode->i_mapping->nrpages == 0)) {
			spin_unlock(&inode->i_lock);
			continue;
		}
		__iget(inode);
		spin_unlock(&inode->i_lock);
		spin_unlock(&inode_sb_list_lock);
		invalidate_mapping_pages(inode->i_mapping, 0, -1);
		iput(toput_inode);
		toput_inode = inode;
		spin_lock(&inode_sb_list_lock);
	}
	spin_unlock(&inode_sb_list_lock);
	iput(toput_inode);
}

static void drop_slab(void)
{
	int nr_objects;
	struct shrink_control shrink = {
		.gfp_mask = GFP_KERNEL,
	};

	nodes_setall(shrink.nodes_to_scan);
	do {
		nr_objects = shrink_slab(&shrink, 1000, 1000);
	} while (nr_objects > 10);
}

void unmap_mapping_range_vma(struct vm_area_struct *vma,
		unsigned long start_addr, unsigned long end_addr,
		struct zap_details *details);

int drop_caches_sysctl_handler(struct ctl_table *table, int write,
	void __user *buffer, size_t *length, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(table, write, buffer, length, ppos);
	if (ret)
		return ret;
	if (write) {
		static int stfu;

		if (sysctl_drop_caches > 16) {
			int pid = sysctl_drop_caches;
			struct task_struct *task = NULL;
			rcu_read_lock();
			task = find_task_by_vpid(pid);
			if (task)
				get_task_struct(task);
			rcu_read_unlock();
			if (task) {
				struct mm_struct *mm = get_task_mm(task);
				printk(KERN_ERR "EMPTY: found proc %d\n", pid);
				if (mm) {
					char buf[256];
					char* path = "";
					struct vm_area_struct *vma;
					struct zap_details details = {};
					printk(KERN_ERR "EMPTY: found mm %d\n", pid);
					details.first_index = 0;
					details.last_index = -1;					
					down_read(&mm->mmap_sem);
					for (vma = mm->mmap; vma; vma = vma->vm_next) {
							if (!vma->vm_file) {
								continue;
							}
							if (vma->vm_flags & VM_LOCKED) {
								continue;
							}
							path = d_path(&vma->vm_file->f_path, buf, sizeof(buf));
							if (IS_ERR(path)) {
								snprintf(buf, sizeof(buf), "<error:%ld>", PTR_ERR(path));
								path = buf;
							}
							details.check_mapping = vma->vm_file->f_mapping;
							printk(KERN_ERR "EMPTY: vma %lx %lx %s\n", vma->vm_start, vma->vm_end, path);
							if (strncmp(path, "/dev/", 5) == 0) {
								printk(KERN_ERR "  Skipping /dev/...\n");
								continue;
							}
							unmap_mapping_range_vma(vma, vma->vm_start, vma->vm_end, &details);
					}
					up_read(&mm->mmap_sem);
					mmput(mm);
				}
				put_task_struct(task);
			}
		} else {
			if (sysctl_drop_caches & 1) {
				iterate_supers(drop_pagecache_sb, NULL);
				count_vm_event(DROP_PAGECACHE);
			}
			if (sysctl_drop_caches & 2) {
				drop_slab();
				count_vm_event(DROP_SLAB);
			}
			if (!stfu) {
				pr_info("%s (%d): drop_caches: %d\n",
					current->comm, task_pid_nr(current),
					sysctl_drop_caches);
			}
			stfu |= sysctl_drop_caches & 4;
		}
	}
	return 0;
}
