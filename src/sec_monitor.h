#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kernel.h> 

void sec_monitor(void)
{
	printk("this is sec monitor! \n");
}
EXPORT_SYMBOL(sec_monitor);
