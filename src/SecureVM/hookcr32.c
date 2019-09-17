#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kernel.h> 
#include <asm/pgtable.h> 
#include <asm/page.h>
#include <asm/page_types.h>
#include <linux/mm_types.h>
#include <linux/mm.h> 
#include <linux/sched.h> 
#include <asm/tlbflush.h>
//#define CREATE_TRACE_POINTS//
#include <asm/trace/exceptions.h>
#include <asm/vmx.h>
#include <asm/kvm_host.h>
#include <linux/kvm_host.h>
//#include <asm/vmx_modify.h>
//#include <asm/kvm_main.h>
//#include <asm/vmx_mmu_audit.h>
//#include <linux/ratelimit.h>
#include <asm/io.h>
#include <linux/ksm.h>
#include <linux/secureVM.h>
//#include <asm-generic/io.h>

#include "gate_function.h"
#include "set_hook.h"
#include "sec_monitor.h"
#include "gate_headerfile.h"

extern void new_native_set_pte(pte_t *ptep, pte_t pte);
extern void new_native_write_cr3(unsigned long val);
extern void new_native_set_pmd(pmd_t *pmdp, pmd_t pmd);
extern dotraplinkage  void notrace new_do_page_fault(struct pt_regs *regs, unsigned long error_code);
//extern void new_vmcs_write64(unsigned long field, u64 value);

unsigned long old_cr3;


static int __init my_init(void)  
{	
	unsigned long pgd_new;
	unsigned long addr_to_protected;
	//unsigned long * address_tmp = NULL;

	addr_to_protected =  init_struct();
	__flush_tlb_all();
	//set_pgd_cp();
	__flush_tlb_all();
	sec_monitor_test();

	set_hook_pte( (void*)native_set_pte,(void*)new_native_set_pte );
	set_hook_pmd( (void*)native_set_pmd,(void*)new_native_set_pmd );
	set_hook_pagefault( (void*)do_page_fault,(void*)new_do_page_fault);

	set_hook_vmcs_readl( (void*)vmcs_readl,(void*)new_vmcs_readl);//不可行
	set_hook_vmcs_writel( (void*)vmcs_writel,(void*)new_vmcs_writel);
	set_hook_vmcs_clear( (void*)vmcs_clear,(void*)new_vmcs_clear);
		
	//set_hook_vmx_vcpu_run((void*)vmx_vcpu_run,(void*)new_vmx_vcpu_run);	//可行
	//set_hook_vmx_handle_exit((void*)vmx_handle_exit,(void*)new_vmx_handle_exit);//handle exit 
	
	//set_hook_tdp_page_fault((void*)tdp_page_fault,(void*)new_tdp_page_fault);
	
	set_hook_handle_pfn_modify((void*)handle_pfn_modify,(void*)new_handle_pfn_modify);	
	set_hook_handle_pfn_map((void*)handle_pfn_map,(void*)new_handle_pfn_map);

	set_hook_clean_page_modify((void*)clean_page_modify,(void*)new_clean_page_modify);
	set_hook_testforshrinkpage_modify((void*)testforshrinkpage_modify,(void*)new_testforshrinkpage_modify);

	set_hook_ksm_page_modify((void*)ksm_page_modify,(void*)new_ksm_page_modify);

	set_hook_inflate_page_modify( (void*)inflate_page_modify,(void*)new_inflate_page_modify );
	set_hook_deflate_page_modify( (void*)deflate_page_modify,(void*)new_deflate_page_modify );

	//new_clean_page_modify1();
	__flush_tlb_all();
	//printk("old cr3 1: %lx\n",(unsigned long)__va(read_cr3()));
	pgd_new = get_pgd_cp();//
	//printk("pged_new : %lx\n",pgd_new);
	old_cr3 = read_cr3();
	//cp_old_pg_half(pgd_new);
	//printk("kernel cr3 : %lx\n",(unsigned long)__va(old_cr3));

	return 0;	
}


static void __exit my_exit(void)  
{
	memcpy(mem_pte,orig_code_pte,sizeof(orig_code_pte));
	memcpy(mem_pmd,orig_code_pmd,sizeof(orig_code_pmd));
	memcpy(mem_pagefault,orig_code_pagefault,sizeof(orig_code_pagefault));
	
	memcpy(mem_vmcs_writel,orig_code_vmcs_writel,sizeof(orig_code_vmcs_writel));
	memcpy(mem_vmcs_readl,orig_code_vmcs_readl,sizeof(orig_code_vmcs_readl));
	memcpy(mem_vmcs_clear,orig_code_vmcs_clear,sizeof(orig_code_vmcs_clear));

	//memcpy(mem_vmx_vcpu_run,orig_code_vmx_vcpu_run,sizeof(orig_code_vmx_vcpu_run));
	//memcpy(mem_vmx_handle_exit,orig_code_vmx_handle_exit,sizeof(orig_code_vmx_handle_exit));// handle exit
	
	//memcpy(mem_tdp_page_fault,orig_code_tdp_page_fault,sizeof(orig_code_tdp_page_fault));

	memcpy(mem_handle_pfn_modify,orig_code_handle_pfn_modify,sizeof(orig_code_handle_pfn_modify));
	memcpy(mem_handle_pfn_map,orig_code_handle_pfn_map,sizeof(orig_code_handle_pfn_map));
        memcpy(mem_ksm_page_modify,orig_code_ksm_page_modify,sizeof(orig_code_ksm_page_modify));
	memcpy(mem_clean_page_modify,orig_code_clean_page_modify,sizeof(orig_code_clean_page_modify));
	memcpy(mem_testforshrinkpage_modify,orig_code_testforshrinkpage_modify,sizeof(orig_code_testforshrinkpage_modify));

	memcpy(mem_inflate_page_modify,orig_code_inflate_page_modify,sizeof(orig_code_inflate_page_modify));
	memcpy(mem_deflate_page_modify,orig_code_deflate_page_modify,sizeof(orig_code_deflate_page_modify));


	printk("Good bye! The secureVM system is closed!\n");

	
}

module_init(my_init);  
module_exit(my_exit);  
  
MODULE_LICENSE("GPL");	
