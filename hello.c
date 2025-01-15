// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/errno.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training with custom parameters");
MODULE_LICENSE("Dual BSD/GPL");

static uint print_count = 1;
module_param(print_count, uint, 0444);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!'");

struct event {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(event_list);

static int __init hello_init(void)
{
	struct event *new_event;
	uint i;

	if (print_count == 0 || (print_count >= 5 && print_count <= 10))
		pr_warn("Warning: print_count is 0 or between 5 and 10. Proceeding...\n");
	else if (print_count > 10) {
		pr_err("Error: print_count > 10. Module not loaded.\n");
		return -EINVAL;
	}

	for (i = 0; i < print_count; i++) {
		new_event = kmalloc(sizeof(*new_event), GFP_KERNEL);
		if (!new_event)
			return -ENOMEM;

		new_event->time = ktime_get();
		list_add_tail(&new_event->list, &event_list);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct event *entry, *tmp;

	pr_info("Exiting module and printing event times:\n");

	list_for_each_entry_safe(entry, tmp, &event_list, list) {
		pr_info("Event time: %lld ns\n", ktime_to_ns(entry->time));
		list_del(&entry->list);
		kfree(entry);
	}
}

module_init(hello_init);
module_exit(hello_exit);

