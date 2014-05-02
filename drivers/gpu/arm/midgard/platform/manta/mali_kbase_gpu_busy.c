/*
 * Copyright (C) 2014 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


#include <linux/notifier.h>
#include <linux/cpu.h>
#include <linux/../../kernel/sched/sched.h>
#include <malisw/mali_stdtypes.h>
#include <mali_kbase.h>

static int mali_compute_job_notifier(struct notifier_block *nb, unsigned long action, void *data) {
	int cpu;

	if (action == COMPUTE_JOB_SCHEDULED) {
		for_each_online_cpu(cpu)
			atomic_inc(&cpu_rq(cpu)->nr_iowait);
	} else if (action == COMPUTE_JOB_DONE) {
		for_each_online_cpu(cpu)
			atomic_dec(&cpu_rq(cpu)->nr_iowait);
	}

	return NOTIFY_OK;
}

static struct notifier_block mali_compute_job_notifier_block = {
	.notifier_call = mali_compute_job_notifier,
};

int kbase_platform_gpu_busy_init(void) {
	int ret;

	ret = compute_job_register_notifier(&mali_compute_job_notifier_block);

	if (ret)
		printk(KERN_ERR "%s: cannot register gpu busy notifier\n",
			__func__);

	return MALI_TRUE;
}
