#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
//编写内核模块所需要的一些头文件，比如模块注册和注销的宏等
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/fs_struct.h>
#include <linux/fdtable.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <linux/jiffies.h>
#include <linux/cdev.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <linux/path.h>
#include <linux/time.h>
#include <linux/stat.h>
#include <net/sock.h>
#include <net/inet_sock.h>
#include <asm/cpufeature.h>
#include <linux/kprobes.h>

#include <asm/debugreg.h>
#include <asm/siginfo.h>
#include <asm/traps.h>
#include <asm/trace/exceptions.h>

unsigned long sec_address_start = 0x0;
static  unsigned long space_addr ;

void get_mem(void)
{
	//int i;
	space_addr = get_zeroed_page(__GFP_HIGHMEM);
	//printk("space_addr : %lx\n",space_addr);
}


dotraplinkage void notrace
sdo_page_fault(struct pt_regs *regs, unsigned long error_code)
{
	unsigned long address = read_cr2(); /* Get the faulting address */
	//enum ctx_state prev_state;
	//printk("sdo_page_fault success ! : %lx \n",address);
	/*
	 * We must have this function tagged with __kprobes, notrace and call
	 * read_cr2() before calling anything else. To avoid calling any kind
	 * of tracing machinery before we've observed the CR2 value.
	 *
	 * exception_{enter,exit}() contain all sorts of tracepoints.
	 */
	//prev_state = exception_enter();
	/*if( (address >= space_addr) && (address <= space_addr + 4096) )
	{
		return ;
	}*/
	if(address >= TASK_SIZE_MAX)
		printk("Attention! Some program is accessing kernel space!\n");
	__do_page_fault(regs, error_code, address);
	//exception_exit(prev_state);
}

static inline void snative_write_cr0(unsigned long val)
{
	unsigned long old_cr0 = read_cr0();
	
	printk("cr0 \n");

	val |= ( 1UL<<16 );
	if ( !((val != old_cr0)))
		return;
	asm volatile("mov %0,%%cr0": : "r" (val), "m" (__force_order));
}


static inline void snative_write_cr3(unsigned long val)
{
	unsigned long old_cr3 = read_cr3();
	printk("cr3 \n");
	if ( !((val != old_cr3) || (val != __pa(space_addr))) )
		return;
	//printk("secaddr val : %lx\n",val);

	asm volatile("mov %0,%%cr3": : "r" (val), "m" (__force_order));
}

static inline void snative_write_cr4(unsigned long val)
{
	unsigned long old_cr4 = __read_cr4();
	printk("cr4\n");
	if ( !((val != old_cr4)) )
		return;
	asm volatile("mov %0,%%cr4": : "r" (val), "m" (__force_order));
}

#ifdef CONFIG_X86_64
static inline void snative_write_cr8(unsigned long val)
{
	unsigned long old_cr8 = read_cr8();

	if ( !((val != old_cr8)) )
		return;
	asm volatile("movq %0,%%cr8" :: "r" (val) : "memory");
}
#endif

static inline void snative_load_gdt(const struct desc_ptr *dtr)
{	
	printk("lgdt \n");
	asm volatile("lgdt %0"::"m" (*dtr));
}
static inline void snative_load_idt(const struct desc_ptr *dtr)
{
	printk("lidt \n");
	asm volatile("lidt %0"::"m" (*dtr));
}

static inline void sload_ldt(const struct desc_ptr *ldt)
{
	printk("lldt \n");
	asm volatile("lldt %0"::"m" (*ldt));
}

static inline void snative_set_pte(pte_t *ptep, pte_t pte)
{
	printk("set_pte \n");
	*ptep = pte;
}

static inline void snative_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	printk("set pmd \n");
	*pmdp = pmd;
}
