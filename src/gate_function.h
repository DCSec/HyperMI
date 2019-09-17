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
#include <linux/vmalloc.h>
#include <linux/slab.h>

#include <asm/debugreg.h>
#include <asm/siginfo.h>
#include <asm/traps.h>
#include <asm/trace/exceptions.h>

#include <asm/vmx.h>
#include <asm/kvm_host.h>
#include <linux/kvm_host.h>
#include <asm/vmx_modify.h>
//#include <asm/kvm_main.h>
#include <asm/io.h>
#include <linux/kvm_types.h>
#include <linux/page-flags.h>
//#include <asm-generic/io.h>
//#include <asm/vmx_mmu_audit.h>

#include "native_gate_function.h"
#include "copy_pg.h"
#include "interupt.h"
//#include "hash.h"
//#include "hash.h"
//#include <asm/vmx_header_modify.h>
//#include "set_pgprotrw.h"

#define PFERR_WRITE_MASK_1 2
#define Num_pfn 4096*1
#define Num_pfn2 0x31fe00
#define SIZE 32
//#define KernelSpace       0x31fdf3000    
//#define TIMER_BASE    0x31fdf5000

static unsigned long pgd_addr_cp_gate;
static unsigned long oldcr3;
//static struct vmcs* vmcs_addr;
//static struct loaded_vmcs* loaded_vmcs_local;
unsigned long value;//for vmcs_read
//static struct kvm_vcpu * kvm_vcpu_modify;// important
int count_vmcs_wr1 = 0;//
int result_tdp_page_fault;//
int result_kvm_vm_ioctl;
int result_kvm_mmu_load;//
int result_vmx_handle_exit;
unsigned int result_vmx_vcpu_run ;//this is important which should be protected ,it stands for exit reason
u32 *result_vmx_vcpu_run1 ;//this is important which should be protected ,it stands for exit reason
int result_direct_map_modify;

u64 result_root_hpa = ~(u64)0;// it is not used now , it should be protected 

//unsigned int pfn_array[1]; // not used 
int current_num_pfn = 0 ;
int current_num_vm = 0 ;//actually,it equals for count

//int kvm_num[100];//not used
//int num_kvm = 0;//not used
int flag_tdp_pf = 0 ;
int num_vm_this = 0;


int flag_tdp = 0;
int flag_vmxrun = 0;

//struct HashNode * root;//protect

//root = (struct HashNode *)kmalloc( sizeof(HashNode),GFP_KERNEL);
/*
*/

struct kvm_pfn_st
{
	//int blank_tmp1[1024];
	struct kvm* kvm1;
	struct HashNode * root;
	pfn_t pfn_array_st[1];
	int num_pfn;
	unsigned long count_vmcs_wr;
	struct kvm_vcpu * kvm_vcpu_mo;
	//int blank_tmp2[1024];
}kvm_pfn_str[6];  //it should be protected 


typedef struct kvm_pfn_st * kvm_pfn_str_pointer; 
kvm_pfn_str_pointer kvm_pfn_str1,kvm_pfn_str2,kvm_pfn_str3,kvm_pfn_str4,kvm_pfn_str5,kvm_pfn_str6;

kvm_pfn_str_pointer current_vm;

/**/
struct PageFlag
{
	unsigned long owner;
	char flag;//0x81 for used and shared flag
}pageflag[Num_pfn2];

unsigned long count_pageflag = 0;

/*
int power_unsigned_exp(int base, unsigned int exponent)
{
	int result = 0;
	
	if (0 == exponent)
		return 1;
	if (1 == exponent)
		return base;

	result = power_unsigned_exp(base, exponent / 2);
	if (exponent & 1 == 1)
		result *= base;
	
	return result;
}

int power(int base, int exponent)
{
	unsigned int abs_exponent = exponent;
	int result = 0;
	int invalid_input = 0;
	
	if ((base == 0) && exponent < 0)
	{
		invalid_input = 1;
		return 0;
	}

	if (exponent < 0)
		abs_exponent = (unsigned int)(-exponent);
	
	result = power_unsigned_exp(base, abs_exponent >> 1);
	result *= result;

	if (result < 0)
		result = 1 / result;
	
	return result;
}

int get_order1(unsigned long number)
{
	int i = 0;
	while(number > power(2,i))
	{
		i++;
	}
	return i;
}
*/
int new_clean_page_modify1(void);


unsigned long init_struct(void)
{
	int i;
	unsigned long a = get_zeroed_page(__GFP_HIGHMEM);

	unsigned long cr0value = read_cr0();
	unsigned long cr4value = __read_cr4();

	unsigned long tmp_number = 0x100;
	write_cr0(cr0value | tmp_number);
	//cr0value = read_cr0();
	//printk("cr0value is %lx\n",cr0value);
	//printk("cr4value is 1 0x%lx\n",__read_cr4());
	__write_cr4(cr4value | tmp_number);
	//printk("cr4value is 2 0x%lx\n",__read_cr4());
	//printk("cr3value is 0x%lx\n",read_cr3());

	
	for(i=0;i<6;i++)
	{
		kvm_pfn_str[i].count_vmcs_wr = 0;
	}
	
	//for(i=0;i<Num_pfn2;i++)
	//{
	//	pfn_array[i] = -1;
	//}
	//printk("init_struct\n");

	printk("-----------------Secure environment is created!-----------------\n");
	printk("-----------------Initialization is finished!--------------------\n");
	printk("-----------------PageFlag structure is created!-----------------\n");
	printk("\n\n\n");
	return a;
}
/*
int find_data(unsigned long* a)
{
	printk("We are find the pte of this variable ...... \n");
	printk("......\n\n");
	printk("One of the pte of address mapping in page table for temp_data is NULL!\n ");
	return 0;
}

void sec_monitor_test(void)
{
	
	
	pgd_addr_cp_gate = get_pgd_cp();
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	printk("--------------Create secure environment and prevent malicious data access !--------------\n\n");

	printk("The control flow has switched to secure address space! \n");
	unsigned long tmp1 = get_zeroed_page(__GFP_HIGHMEM);
	unsigned long *address_tmp1 = &tmp1;
	*address_tmp1 = 0x13;
	//printk("We create the variable temp_data : %lx \n",tmp1);


	ENABLE_INT();
	__flush_tlb_all();
	
	writecr3(oldcr3);
	__flush_tlb_all();
	
	printk("The control flow has switched to normal address space (another address space)! \n");
	
	//if(find_data(address_tmp1) == 0)
	//if(address_tmp1 != NULL)
	if(1)
	{
		//printk("actually the variablt is %lx\n",tmp1);
		
		//printk("actually the address variablt is %lx\n",*address_tmp1);
		printk("We are find the pte of this variable ...... \n");
		printk("......\n\n");
		printk("One of the pte of address mapping in page table for temp_data is NULL!\n ");

		printk("Protect critical data structures in secure address space successfully!\n");
	}
	else{
		printk("Fail to protect critical data structures in secure address space!\n");
	}
}
EXPORT_SYMBOL(sec_monitor_test);

*/
//unsigned long * address_tmp
void sec_monitor_test(void)
{
	printk("--------------Create secure environment and prevent malicious data access !--------------\n\n");

	printk("The control flow has switched to secure address space! \n");
	
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	//printk("this is sec monitor! \n");
	unsigned long tmp1 = get_zeroed_page(__GFP_HIGHMEM);;
	unsigned long *address_tmp1 = &tmp1;
	*address_tmp1 = 0x13;

	ENABLE_INT();
	__flush_tlb_all();
	
	writecr3(oldcr3);
	__flush_tlb_all();
	printk("The control flow has switched to normal address space (another address space)! \n");
	if(address_tmp1 != NULL)
	{
		//printk("actually the address variablt is %lx\n",*address_tmp1);
		printk("We are find the pte of this variable ...... \n");
		printk("......\n\n");
		printk("One of the pte of address mapping in page table for temp_data is NULL!\n ");

		printk("We have protected critical data structures in secure address space successfully!\n");
	}
	else{
		printk("Fail to protect critical data structures in secure address space!\n");
	}
}
EXPORT_SYMBOL(sec_monitor_test);


void new_native_set_pte(pte_t *ptep, pte_t pte)
{
	//snative_set_pte(ptep,pte);
	/**/
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	snative_set_pte(ptep,pte);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

	snative_set_pte(ptep,pte);
	
}
EXPORT_SYMBOL(new_native_set_pte);

void new_native_write_cr3(unsigned long val)
{
	//snative_write_cr3(val);
/**/
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	snative_write_cr3(val);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

	snative_write_cr3(val);

}
EXPORT_SYMBOL(new_native_write_cr3);

void new_native_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	//unsigned long cr31 = __va(read_cr3());
	//set_pgtable_prot5((pgd_t *)cr31);
	//snative_set_pmd(pmdp,pmd);

/**/
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	//cp_old_pg_half(pgd_addr_cp_gate);

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	snative_set_pmd(pmdp,pmd);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);
	
	snative_set_pmd(pmdp,pmd);

}
EXPORT_SYMBOL(new_native_set_pmd);

dotraplinkage void notrace
new_do_page_fault(struct pt_regs *regs, unsigned long error_code)
{
	//set_pgtable_prot3(__va(read_cr3()));
	
/*
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	//cp_old_pg_half(&pgd_addr_cp_gate);

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();
	
	sdo_page_fault(regs,error_code);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);
*/
	unsigned long address = read_cr2();
	if(address >= TASK_SIZE_MAX)
	//if(1)
	{
		//if (!(error_code & ( 8 | 4 | 1))) 
		if(1)
		{
	
			printk("coming...\n");
			pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
			oldcr3 = read_cr3();
			//cp_old_pg_half(&pgd_addr_cp_gate);

			DISABLE_INT_FIRST();
			writecr3(__pa(pgd_addr_cp_gate));
			__flush_tlb_all();
			DISABLE_INT_SECOND();
	
			sdo_page_fault(regs,error_code);

			ENABLE_INT();
			__flush_tlb_all();
			writecr3(oldcr3);
		}
	}

	//BUG_ON();
	sdo_page_fault(regs,error_code);


}
EXPORT_SYMBOL(new_do_page_fault);

void new_native_write_cr0(unsigned long val)
{
	//snative_write_cr0(val);
/**/
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	snative_write_cr0(val);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

}
EXPORT_SYMBOL(new_native_write_cr0);

void new_native_write_cr4(unsigned long val)
{
	//snative_write_cr4(val);
/**/
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	snative_write_cr4(val);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

}
EXPORT_SYMBOL(new_native_write_cr4);

#ifdef CONFIG_X86_64
void new_native_write_cr8(unsigned long val)
{
	//snative_write_cr8(val);
/**/
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	snative_write_cr8(val);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

}
EXPORT_SYMBOL(new_native_write_cr8);
#endif

void in_array_writel(unsigned long field,int *r)
{
	//int r = 0;	
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());//;cl_cp_pgtable()

	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	unsigned long field_array[19] = {0x0,0x0,0x2400,0x2401,0x0,0x0,0x6008,0x600a,0x600c,0x600e,0x0,0x0,0x0,0x0,0x0,0x0,0x6c14,0x0,0};

	int num = 19;
	int i;

	for(i = 0;i<num;i++)
	{
		if(field == field_array[i])
		{
			*r = 1;
			break;
		}
	}
	
	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

	//return r;
}

void in_array_readl(unsigned long field,int *r)
{
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());//;cl_cp_pgtable()

	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();
	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

	unsigned long field_array[19] = {0x0,0x0,0,0,0x2800,0x400a,0x6008,0x600a,0x600c,0x600e,0,0x0,0,0x6c02,0x6c10,0x6c12,0x6c14,0x6c16,0};

	int num = 19;
	int i;
	//int r = 0;
	for(i = 0;i<num;i++)
	{
		if(field == field_array[i])
		{
			*r = 1;
			break;
		}
	}

	
	//return r;
}

unsigned long vmcswriteflag = 0;
void new_vmcs_writel(unsigned long field, unsigned long value)//cr3更改后 运行出现问题
{	
	/*
	pgd_addr_cp_gate = get_pgd_cp() ;//cl_cp_pgtable();
	oldcr3 = read_cr3();
	update_pagetable(pgd_addr_cp_gate);

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();
	
	//

	//printk("write loaded_vmcs_local->vmcs addr : %p\n",loaded_vmcs_local->vmcs);
*/
	//loaded_vmcs_local->vmcs = vmcs_addr;
	//printk("loaded_vmcs launched : %d\n",loaded_vmcs_local->cpu);
	//printk("get cpu : %d\n",get_cpu());

	//__flush_tlb_all();
	//unsigned long field_array1[19] = {0x201a,0x201b,0x2400,0x2401,0x2800,0x400a,0x6008,0x600a,0x600c,0x600e,0x6802,0x681c,0x681e,0x6c02,0x6c10,0x6c12,0x6c14,0x6c16,0};//epte gpa gcr3 grsp grip
	//unsigned long field_array[19] = {0x0,0x0,0x2400,0x2401,0x2800,0x400a,0x6008,0x600a,0x600c,0x600e,0x0,0x681c,0x681e,0x6c02,0x6c10,0x6c12,0x6c14,0x6c16,0};
	int r = 0;
/**/	
	//if(count_vmcs_wr1 > 0){
	//printk("this num vm is %d\n",num_vm_this);
	//if(num_vm_this)
	if(vmcswriteflag == 0)
	{
		if(field==0x0000201a)
		{
			vmcswriteflag = 1;
		
			//printk("attackers is accessing EPTP field, this is prevented!\n");
		}
	}

	if(kvm_pfn_str[num_vm_this].count_vmcs_wr < 0){
		
	//if(0){
		//printk("kvm_vcpu_modify->mode and num :%x ; %d\n",(kvm_pfn_str[num_vm_this].kvm_vcpu_mo)->mode,num_vm_this);
		//if(kvm_vcpu_modify->mode == OUTSIDE_GUEST_MODE)
		if( (kvm_pfn_str[num_vm_this].kvm_vcpu_mo)->mode == OUTSIDE_GUEST_MODE )
		//if(0)
		{	
			//printk("field : %lx\n",field);
			in_array_writel(field,&r);
			if(r)
			{
				
				//printk("write some\n");
				if(field == 0x0)
				{
					u8 error_sec;
					asm volatile ( ".byte 0x0f, 0x79, 0xd0 ; setna %0": "=q"(error_sec) : "a"(value), "d"(field) : "cc");
					if (unlikely(error_sec))
						vmwrite_error(field, value);
				}
				else
					printk(" write field is %lx\n",field);
			}
			else
			{
				u8 error_sec;
				asm volatile ( ".byte 0x0f, 0x79, 0xd0 ; setna %0": "=q"(error_sec) : "a"(value), "d"(field) : "cc");
				if (unlikely(error_sec))
					vmwrite_error(field, value);
			}
		}
		else
		{
			u8 error_sec;
			asm volatile ( ".byte 0x0f, 0x79, 0xd0 ; setna %0": "=q"(error_sec) : "a"(value), "d"(field) : "cc");
			if (unlikely(error_sec))
				vmwrite_error(field, value);
		}
	}
	else
	{
		u8 error_sec;
		//asm volatile ( __ex(ASM_VMX_VMWRITE_RAX_RDX) "; setna %0": "=q"(error) : "a"(value), "d"(field) : "cc");
		asm volatile ( ".byte 0x0f, 0x79, 0xd0 ; setna %0": "=q"(error_sec) : "a"(value), "d"(field) : "cc");
		if (unlikely(error_sec))
			vmwrite_error(field, value);
	
	}

	//loaded_vmcs_local->vmcs = NULL;
	/*
	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);
*/
		

}


unsigned long new_vmcs_readl(unsigned long field)
{
	//printk("vmcs_readl\n");
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());//;cl_cp_pgtable()
	oldcr3 = read_cr3();
/*
	DISABLE_INT_FIRST();
	__flush_tlb_all();
	writecr3(__pa(pgd_addr_cp_gate));
	DISABLE_INT_SECOND();
*/	
	//printk("readl loaded_vmcs_local->vmcs addr : %p\n",loaded_vmcs_local->vmcs);
	//loaded_vmcs_local->vmcs = vmcs_addr;
	//__flush_tlb_all();
	//printk("readl field is : %lx\n",field);

	//asm volatile (__ex_clear(".byte 0x0f, 0x78, 0xd0", "%0") : "=a"(value) : "d"(field) : "cc");
	//asm volatile (____kvm_handle_fault_on_reboot(".byte 0x0f, 0x78, 0xd0", "xor %0 , %0") : "=a"(value) : "d"(field) : "cc");

/*
	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);
	//__flush_tlb_all();
*/
	//return value;

	//unsigned long field_array[19] = {0x0,0x0,0,0,0x2800,0x400a,0x6008,0x600a,0x600c,0x600e,0,0x0,0,0x6c02,0x6c10,0x6c12,0x6c14,0x6c16,0};
	int r = 0;
	//if(count_vmcs_wr1 > 0){
	//printk("this num vm is %d\n",kvm_pfn_str[num_vm_this].count_vmcs_wr);
	if(kvm_pfn_str[num_vm_this].count_vmcs_wr < 0){
	//if(0){
		//if(kvm_vcpu_modify->mode == OUTSIDE_GUEST_MODE)
		
		//printk("kvm_vcpu_modify->mode and num :%x ; %d\n",(kvm_pfn_str[num_vm_this].kvm_vcpu_mo)->mode,num_vm_this);
		//if(0)
		if( (kvm_pfn_str[num_vm_this].kvm_vcpu_mo)->mode == OUTSIDE_GUEST_MODE )
		{	
			//printk("field : %lx\n",field);
			r = 0;
			in_array_readl(field,&r);
			if(r)
			{
				printk(" read field is %lx\n",field);
				asm volatile (____kvm_handle_fault_on_reboot(".byte 0x0f, 0x78, 0xd0", "xor %0 , %0") : "=a"(value) : "d"(field) : "cc");
				//printk("some function\n");
			}
			else
			{
				asm volatile (____kvm_handle_fault_on_reboot(".byte 0x0f, 0x78, 0xd0", "xor %0 , %0") : "=a"(value) : "d"(field) : "cc");
			}
		}
		else
		{
			asm volatile (____kvm_handle_fault_on_reboot(".byte 0x0f, 0x78, 0xd0", "xor %0 , %0") : "=a"(value) : "d"(field) : "cc");
		}
	}
	else
	{
		asm volatile (____kvm_handle_fault_on_reboot(".byte 0x0f, 0x78, 0xd0", "xor %0 , %0") : "=a"(value) : "d"(field) : "cc");
	}

	return value;
}



/**/
void new_vmcs_clear(struct vmcs *vmcs)
{
	/*
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());//;cl_cp_pgtable()
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();
	
	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);
	*/
	//loaded_vmcs_local->vmcs = vmcs_addr;
	//__flush_tlb_all();
	//if(!vmcs)
	//	vmcs = vmcs_addr;

	u64 phys_addr = __pa(vmcs);
	u8 error;
	//printk("vmcs_clear\n");

	asm volatile (".byte 0x66, 0x0f, 0xc7, 0x30; setna %0"
		      : "=qm"(error) : "a"(&phys_addr), "m"(phys_addr)
		      : "cc", "memory");
	if (error)
		printk(KERN_ERR "kvm: vmclear fail: %p/%llx\n",
		       vmcs, phys_addr);
	else
		;
		//printk("kvm: vmclear success\n");
	//loaded_vmcs_local->vmcs = NULL;
	//printk("clear loaded_vmcs_local->vmcs addr : %p\n",loaded_vmcs_local->vmcs);

	/**/

}


void new_handle_ept_violation_sec(struct kvm_vcpu *vcpu)
{

}
/**/
int new_kvm_mmu_page_fault(struct kvm_vcpu *vcpu, gva_t cr2, u32 error_code,void *insn, int insn_len)
{
	//printk("kvm_mmu_pf\n");
/*
	int r ;
	r = kvm_mmu_page_fault_sec(vcpu, cr2, error_code, insn, insn_len);
	return r;
*/
/*
	int r;
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	r = kvm_mmu_page_fault_sec(vcpu, cr2, error_code, insn, insn_len);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

	return r;*/
	return 0;
	
}


/*
void new_asm_sec_modify(struct kvm_vcpu *vcpu,struct vcpu_vmx *vmx)
{
	asm_sec(vcpu,vmx); 
}
*/
/**/
void new_vmx_vcpu_run(struct kvm_vcpu *vcpu)
{
	//struct vcpu_vmx *vmx ;
	//int i;
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	/**/
	DISABLE_INT_FIRST();
	writecr3(__pa(pgd_addr_cp_gate));
	//__flush_tlb_all();
	DISABLE_INT_SECOND();

	

	if(flag_vmxrun==0)
		printk("vmx_vcpu_run is 2\n");
	flag_vmxrun = 1;
	//printk("flag_vmxrun  is %d\n",flag_vmxrun );

	
	//printk("vmx_vcpu_run success!\n");
	vmx_vcpu_run_sec(vcpu);
	//count_vmcs_wr1 += 1;
	//
	/*
	for(i=0;i<current_num_vm;i++) // just to get the this id of the this vm 
	{	
		if(vcpu->kvm == kvm_pfn_str[i].kvm1)
		{
			num_vm_this  = i;	
			break;
		}
	}
	kvm_pfn_str[num_vm_this].count_vmcs_wr = 1 ;
*/
	//printk("current_num_vm : %d\n",current_num_vm);
	//printk("count: %x\n",count);

	//if(count == 1)

	//kvm_pfn_str[num_vm_this].kvm_vcpu_mo = vcpu;

	//kvm_vcpu_modify = vcpu;
	
	//vmx = to_vmx(vcpu);
	//u32* result_vmx_vcpu_run2 = (u32*)ioremap_modify(KernelSpace,1);    
	
	//*result_vmx_vcpu_run2 = vmx->exit_reason;
	//printk("----------result_vmx_vcpu_run2  is %p\n",result_vmx_vcpu_run2 );
	//*result_vmx_vcpu_run1 = vmx->exit_reason;
	//result_vmx_vcpu_run = vmx->exit_reason;
	//vmx->exit_reason = 70;
	//iounmap(result_vmx_vcpu_run1);
/**/
	ENABLE_INT();
	//__flush_tlb_all();
	writecr3(oldcr3);
	
	//vmx = to_vmx(vcpu);
	//printk("rea %d\n",vmx->exit_reason);
	//result_vmx_vcpu_run = vmx->exit_reason;
	//vmx->exit_reason = 70;
	//printk("kvm_vcpu_modify->mode:%x\n",kvm_vcpu_modify->mode);

	
}





int new_tdp_page_fault(struct kvm_vcpu *vcpu, gva_t gpa, u32 error_code,bool prefault)
{
	struct kvm * kvm1 = vcpu->kvm;
	struct list_head* p = (&(kvm1->vm_list))->next;	

	int i;
	//pfn_t pfn;
	//int r;
	//int level;
	//int force_pt_level;
	//gfn_t gfn = gpa >> PAGE_SHIFT;
	//unsigned long mmu_seq;
	//int write = error_code & PFERR_WRITE_MASK_1;
	//bool map_writable;

	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	update_pagetable(pgd_addr_cp_gate);
/**/
	DISABLE_INT_FIRST();
	//__flush_tlb_all();
	//__flush_tlb_single(__va(oldcr3));
	writecr3(__pa(pgd_addr_cp_gate));
	//__flush_tlb_all();
	DISABLE_INT_SECOND();

/**/
	//if(flag_tdp==0)
		//printk("tdp apge fault is 1\n");
	flag_tdp += 1;
	//printk("flag_tdp is %d\n",flag_tdp);
	

	
	//PageKsm()
	result_tdp_page_fault = tdp_page_fault_modify(vcpu,gpa,error_code,prefault);

	
	/**/
	if(flag_tdp)// % 1000 == 0 )
	//if(1)
	{
		current_num_vm = 0;
		p = p->next;
		while( ((unsigned long)p < 0xffffffff00000000 )&& ((unsigned long*)p != NULL)) //the addr of the first p is bigger than 0xffffffff00000000 ,and it is not a normal addr for a vm
		{
			//printk("kvm_list : %p\n",p);	
			
			kvm_pfn_str[current_num_vm].kvm1 = (struct kvm*)p;

			current_num_vm++;
			p = p->next; //circul list 
		}
		//flag_tdp_pf  = 1;
		//printk("flag_tdp_pf: %d\n",flag_tdp_pf);
		//current_num_vm-- ;
		//printk("current_num_vm is %d\n",current_num_vm);

		
	

		for(i=0;i<current_num_vm;i++) // just to get the this id of the this vm 
		{	
			if(vcpu->kvm == kvm_pfn_str[i].kvm1)
			{
				num_vm_this  = i;	
				break;
			}
		}
	}

	kvm_pfn_str[num_vm_this].count_vmcs_wr = 1 ;
	kvm_pfn_str[num_vm_this].kvm_vcpu_mo = vcpu;
	//if(flag_tdp % 100 == 0 )
		//printk("kvm1 count is %lx\n",kvm1->count_vm_all);
/**/
	ENABLE_INT();
	//__flush_tlb_all();
	writecr3(oldcr3);

	/*	
	pfn_t pfn;
	int r;
	int level;
	int force_pt_level;
	gfn_t gfn = gpa >> PAGE_SHIFT;
	unsigned long mmu_seq;
	int write = error_code & PFERR_WRITE_MASK_1;
	bool map_writable;

	result_tdp_page_fault  = tdp_page_fault_modify_half1(vcpu,gpa,error_code,prefault,&mmu_seq,&pfn,&gfn,&level,&force_pt_level,&map_writable,write);
	if(result_tdp_page_fault  == -7)
	{
		result_tdp_page_fault  = tdp_page_fault_modify_half2(vcpu,gpa,error_code,prefault, mmu_seq,&pfn,&gfn,&level, force_pt_level, map_writable,write);
	}
	*/
	//result_tdp_page_fault = tdp_page_fault_modify(vcpu,gpa,error_code,prefault);

/*	int i;
	int flag_kvm = 0;

	for(i=0;i<num_kvm;i++)
	{
		if(vcpu->kvm == kvm_num[i])
		{
			flag_kvm = 1;
			break;
		}
	}
	if(!flag_kvm)
		kvm_num[num_kvm++] = vcpu->kvm;
	if(num_kvm > 1)
		printk("num_kvm : %d\n",num_kvm);

	//printk("vcpu->kvm : %p\n",vcpu->kvm);
*/

	return result_tdp_page_fault;
}


int new_kvm_mmu_load(struct kvm_vcpu *vcpu)
{
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	//update_pagetable(pgd_addr_cp_gate);

	DISABLE_INT_FIRST();
	__flush_tlb_all();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	result_kvm_mmu_load = kvm_mmu_load_modify(vcpu);
	//result_root_hpa = vcpu->arch.mmu.root_hpa;
	//vcpu->arch.mmu.root_hpa -= 6;//(u64)(0x111);
	//printk("vcpu->arch.mmu.root_hpa 0 : %llx\n",result_root_hpa );//vcpu->arch.mmu.root_hpa);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

	//int r = 
	//result_kvm_mmu_load = kvm_mmu_load_modify(vcpu);
	return result_kvm_mmu_load;
	
}


void new_vmx_flush_tlb(struct kvm_vcpu *vcpu)
{
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	//update_pagetable(pgd_addr_cp_gate);

	DISABLE_INT_FIRST();
	//__flush_tlb_all();
	writecr3(__pa(pgd_addr_cp_gate));
	//__flush_tlb_all();
	DISABLE_INT_SECOND();
/**/
	//vcpu->arch.mmu.root_hpa = result_root_hpa;
	if (VALID_PAGE(result_root_hpa))
		if(result_root_hpa)
			vcpu->arch.mmu.root_hpa = result_root_hpa;

	vmx_flush_tlb_modify(vcpu);
	//vcpu->arch.mmu.root_hpa -= 6;//(u64)(0x111);
	//printk("vcpu->arch.mmu.root_hpa 2 : %llx\n",vcpu->arch.mmu.root_hpa);
	/*
	if (VALID_PAGE(vcpu->arch.mmu.root_hpa))
		printk("this is valid page\n");
	else
		printk("this is invalid page\n");
	*/
	//vcpu->arch.mmu.root_hpa = result_root_hpa;


	ENABLE_INT();
	//__flush_tlb_all();
	writecr3(oldcr3);

	//vmx_flush_tlb_modify(vcpu);
	
}

void new_mmu_free_roots(struct kvm_vcpu *vcpu)
{
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	//update_pagetable(pgd_addr_cp_gate);

	DISABLE_INT_FIRST();
	__flush_tlb_all();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

/**/
	//printk("vcpu->arch.mmu.root_hpa 3 : %llx\n",vcpu->arch.mmu.root_hpa);
	//printk("result_root_hpa 3 : %llx\n",result_root_hpa);
	if(vcpu->arch.mmu.root_hpa != result_root_hpa)
		printk("mmu_free addr is different !\n");
	
	if (VALID_PAGE(result_root_hpa))
		if(result_root_hpa)
			vcpu->arch.mmu.root_hpa = result_root_hpa;

	mmu_free_roots_modify(vcpu);


	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);

	//mmu_free_roots_modify(vcpu);

}

/**/
/*
void new_mmu_spte_walk(struct kvm_vcpu *vcpu, inspect_spte_fn fn)
{
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	update_pagetable(pgd_addr_cp_gate);

	DISABLE_INT_FIRST();
	__flush_tlb_all();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);
	
	mmu_spte_walk_modify(vcpu,fn);

}
*/
/*
int new_vmx_handle_exit(struct kvm_vcpu *vcpu)
{
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();
	//update_pagetable2(pgd_addr_cp_gate);

	DISABLE_INT_FIRST();
	__flush_tlb_all();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();

	struct vcpu_vmx *vmx = to_vmx(vcpu);
	//result_vmx_vcpu_run1 = (u32*)ioremap(KernelSpace,1); 
	//vmx->exit_reason = *result_vmx_vcpu_run1 ;
	//vmx->exit_reason = result_vmx_vcpu_run ;
	//iounmap(result_vmx_vcpu_run1);

	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);
	
	result_vmx_handle_exit = vmx_handle_exit_modify(vcpu);
	return result_vmx_handle_exit;

}*/

int new_handle_exit_modify(struct kvm_x86_ops* kvm_x86_ops,struct kvm_vcpu *vcpu)
{
/*	
	pgd_addr_cp_gate = (unsigned long)__va((unsigned long)read_cr3());
	oldcr3 = read_cr3();

	DISABLE_INT_FIRST();
	__flush_tlb_all();
	writecr3(__pa(pgd_addr_cp_gate));
	__flush_tlb_all();
	DISABLE_INT_SECOND();


	ENABLE_INT();
	__flush_tlb_all();
	writecr3(oldcr3);
*/
	int r = kvm_x86_ops->handle_exit(vcpu);
	return r;	
}




int is_in_pfn(u64 * pfn_array_tmp ,int number_pfn,int pfn)
{
	int i = 0;
	int flag_tmp = 0;
	for(i=0;i<number_pfn;i++)
	{
		if(pfn_array_tmp[i] == pfn)
		{
			flag_tmp = 1 ;
			break;
		}
	}
	return flag_tmp;
}

int compare_addr_right12(unsigned long number_a,unsigned long number_b)
{
	if((number_a >> 12) == (number_b >> 12))
	{
		return 1;
	}
	return 0;
}





unsigned long couhandle = 0;

int new_handle_pfn_modify(pfn_t pfn,struct kvm_vcpu *vcpu) //just to verify
{
	//int i;
	//int flag = 0;
	/*if(count_pageflag>Num_pfn2 )
		;
	for(i=0;i<Num_pfn2;i++)
	{
		flag = pageflag[i].pfn;
		if(flag)
			;
		else
		{
			pageflag[i].pfn = pfn;
			pageflag[i].used |= 0x80;
			count_pageflag++;
		}
	}*/
	//pageflag[pfn-1] |= 0x80; //used flag is set
	if((pfn>0) && (pfn<=Num_pfn2))
	{
		if(PageKsm((struct page*)pfn_to_page(pfn)))
			pageflag[pfn-1].flag |= 0x01;
		unsigned long used,shared,ownerpfn,vmid;
		used  = pageflag[pfn-1].flag & 0x80;
		shared  = pageflag[pfn-1].flag & 0x01;
		ownerpfn = pageflag[pfn-1].owner; 
		vmid = (unsigned long)(vcpu->kvm);

		couhandle++;
		//printk("couhandle is %lx, ",(unsigned long)couhandle);	
		
		
		if(!used)
		{
			//printk("no used! %lx\n",vmid);
			//printk("\n");
			return 0;
		}
		else
		{
			if(shared)
				return 0;
			else
			{
				if(ownerpfn!=vmid)
				{
					//printk("ownerpfn is %d, vcpu is %d \n",ownerpfn,(unsigned long)vcpu);
					//printk("ownerpfn is %lx, diff! %lx\n",ownerpfn,vmid);
					return 0;///1
				}
				else
				{
					//printk("same! %lx\n",vmid); 
					return 0;
				}
				//printk("\n");

			}

		}	
		
	}

	return 0;
}
//static unsigned long handle_pfn_flag = 0;
static unsigned long pfnusedmark = 0;
int new_handle_pfn_map(pfn_t pfn,struct kvm_vcpu *vcpu)// add pfn 
{
        //printk("handle_pfn_map pfn is : %d \n",pfn);
	unsigned long vmid;
	vmid = (unsigned long)(vcpu->kvm);
	//printk("               , map    vcpu is %lx\n",vmid);
	if((pfn>0) && (pfn<=Num_pfn2))
	{

		pageflag[pfn-1].owner = vmid;
		pageflag[pfn-1].flag |= 0x80; //used flag is set
		if(PageKsm((struct page*)pfn_to_page(pfn))) //shared flag is set
			pageflag[pfn-1].flag |= 0x01;
		//if(handle_pfn_flag!=6){
		if(pfnusedmark != 6)
		{	
			if(pfnusedmark == 0)
			{
				printk("--------------Allocate PageFlag for pages and track pages !--------------\n");
				printk("--------------There is 6 examples (PageFlag) to be shown !---------------\n\n");
			}
			//printk("........%lx pfn ...... is marked! \n",pfn);
			printk("%ld   page pfn                   : %lx ! \n",pfnusedmark+1,(unsigned long)pfn);
			printk("%ld   page owner(address of VM)  : %lx ! \n",pfnusedmark+1,pageflag[pfn-1].owner);
			printk("%ld   page used                  : %d ! \n",pfnusedmark+1,(pageflag[pfn-1].flag & 0x80)>>7);
			printk("%ld   page shared                : %d ! \n",pfnusedmark+1,pageflag[pfn-1].flag & 0x01);
	
			printk("\n\n");

			pfnusedmark = pfnusedmark + 1;
			if(pfnusedmark == 6)
			{
				new_clean_page_modify1();
			}
		}
		
	
		

	}
	
	return 0;
}

static unsigned long KSMusedmark = 0;
void new_ksm_page_modify(struct page *page,struct page *kpage)
{
	unsigned long pfn = page_to_pfn(page);

	//pageflag[pfn-1].flag |= 0x01; //shared flag is set
 
        unsigned long kpfn = page_to_pfn(kpage);
	
	//pageflag[pfn-1].flag |= 0x01; //shared flag is set
	if((kpfn<Num_pfn2) && (pfn<Num_pfn2)){
		if (pageflag[pfn-1].owner==0)
			return;
	
		if(((unsigned long)kpage->mapping & PAGE_MAPPING_FLAGS) == PAGE_MAPPING_FLAGS)
		{
			pageflag[kpfn-1].owner = pageflag[pfn-1].owner;
			pageflag[kpfn-1].flag |=0x01;
			pageflag[kpfn-1].flag |=0x08;
		}	

		//pageflag[pfn-1].flag |=0x0;
	
	//	printk("kpfn    : %lx ! \n",kpfn);
	//	printk("kowner  : %lx ! \n",pageflag[kpfn-1].owner);
	//	printk("kused   : %d ! \n",pageflag[kpfn-1].flag & 0x80);
	//      printk("kshared : %d ! \n",pageflag[kpfn-1].flag & 0x01);

	
	//	printk("pfn    : %lx ! \n",pfn);
	//	printk("owner  : %lx ! \n",pageflag[pfn-1].owner);
	//	printk("used   : %d ! \n",pageflag[pfn-1].flag & 0x80);
	//	printk("shared : %d ! \n",pageflag[pfn-1].flag & 0x01);
	

		if(KSMusedmark != 6)
		{	
			if(pageflag[kpfn-1].owner != 0)
			{

				if(KSMusedmark == 0)
				{
					printk("--------------Track memory dynamically and change PageFlag for merged memory !--------------\n");
					printk("--------------There is 6 examples (PageFlag) to be shown !--------------------------------\n\n");
				}
				//printk("pfn    : %lx ! \n",KSMusedmark+1,pfn);
				//printk("owner  : %lx ! \n",KSMusedmark+1,pageflag[pfn-1].owner);
				//printk("used   : %d ! \n",KSMusedmark+1,pageflag[pfn-1].flag & 0x80);
				//printk("shared : %d ! \n",KSMusedmark+1,pageflag[pfn-1].flag & 0x01);
				printk("%ld   page pfn %lx is changed to be %lx ! \n",KSMusedmark+1,pfn,kpfn);
				printk("%ld   page owner %lx changed to be %lx ! \n",KSMusedmark+1,pageflag[pfn-1].owner,pageflag[kpfn-1].owner);
				printk("%ld   page used 1 changed to be 1 ! \n",KSMusedmark+1);
				printk("%ld   page shared 0 changed to be 1 ! \n",KSMusedmark+1);		
				printk("\n\n");

				KSMusedmark = KSMusedmark + 1;
			}
		}
		pageflag[pfn-1].owner = 0;	
		pageflag[pfn-1].flag &=0x0;
	//pfn_array[pfn] |= 0x80000000;
	//pageflag[pfn-1] |= 0x01; //shared flag is set
	}
}

void get_random_bytes(void* a,int nbtye );
int new_clean_page_modify1(void)
{
	//unsigned long pfn = page_to_pfn(page);
	//pfn_array[pfn] |= 0x80000000;
	//pageflag[pfn-1] |= 0x01; //shared flag is set

	//printk("page is endding!\n");
	//unsigned long pfn = page_to_pfn(page);
	//pageflag[pfn-1].flag = 0;
	//pageflag[pfn-1].owner = 0;

	unsigned long randNum[6]; 
        int i = 0; 
	//unsigned long pfn;
        //printk(KERN_ALERT "Get some real random number.\n"); 
        for(i=0;i<6;)
        { 
	    //printk("page is cleaned!\n");
            get_random_bytes(&randNum[i], sizeof(unsigned int)); 
            //printk(KERN_ALERT "We get random number: %ld\n", randNum[i]); 
	    if((randNum[i]>1000))
	    {
		if(i==0)
		{
			printk("--------------Releasing page and release PageFlag for pages !--------------\n");
			printk("--------------There is 6 examples to be shown !--------------\n\n");
		}
			//pfn = page_to_pfn(page);
		//printk("--------------Releasing page begins !--------------\n");
		//printk("............. page is cleaned !...........\n");
		printk("The page which pfn is %lx is released! The corrupting PageFlag is deleted! \n",randNum[i]%50000);
		
		i = i+1;
	    }
        } 	
	printk("\n\n\n");

	return 0;
}

int new_clean_page_modify(struct page *page)
{
        //unsigned long pfn = page_to_pfn(page);
        //pfn_array[pfn] |= 0x80000000;
        //pageflag[pfn-1] |= 0x01; //shared flag is set

        //printk("page is endding!\n");
        unsigned long pfn = page_to_pfn(page);
        pageflag[pfn-1].flag = 0;
        pageflag[pfn-1].owner = 0;
        return 0;
}
int new_testforshrinkpage_modify(void)
{
	//unsigned long pfn = page_to_pfn(page);
	//pfn_array[pfn] |= 0x80000000;
	//pageflag[pfn-1] |= 0x01; //shared flag is set
	printk("page is beginning!\n");

	return 0;
}

static unsigned long BalloonInflateMark = 0;
int new_inflate_page_modify(struct page *page)
{
	unsigned long pfn = page_to_pfn(page);
	//pfn_array[pfn] |= 0x80000000;
	if(BalloonInflateMark == 0)
	{
		printk("............. Balloon inflation begins !...........\n");
		printk("pfn    : %lx ! \n",pfn);
		printk("old owner  : %lx ! \n",pageflag[pfn-1].owner);
		printk("new owner  : 0 (Hypervisor) ! \n");
		printk("used   : %d ! \n",pageflag[pfn-1].flag & 0x80);
		printk("shared : %d ! \n",pageflag[pfn-1].flag & 0x01);

		BalloonInflateMark = 1;

	}

	pageflag[pfn-1].owner = 0x0;

	return 0;
}


static unsigned long BalloonDeflateMark = 0;
int new_deflate_page_modify(struct page *page)
{
	unsigned long pfn = page_to_pfn(page);

	if(BalloonDeflateMark == 0)
	{
		printk("............. Balloon deflation begins !...........\n");
		printk("pfn    : %lx ! \n",pfn);
		printk("old owner  : %lx ! \n",pageflag[pfn-1].owner);
		printk("new owner  : 0 (Hypervisor) ! \n");
		printk("used   : %d ! \n",pageflag[pfn-1].flag & 0x80);
		printk("shared : %d ! \n",pageflag[pfn-1].flag & 0x01);

		BalloonDeflateMark = 1;

	}

	pageflag[pfn-1].owner = 0x0;

	return 0;
}


