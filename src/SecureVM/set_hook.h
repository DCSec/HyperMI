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



uint8_t orig_code_cr3[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_cr31[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
            //jmpq *%rax
        };
uint8_t orig_code_pte1[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
            //jmpq *%rax
        };

uint8_t orig_code_pte[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_pagefault[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_pmd[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_vmcs_write64[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_vmcs_writel[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_vmcs_readl[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_vmcs_clear[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_handle_ept[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_kvm_mmu_page_fault[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_asm_sec_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_vmx_vcpu_run[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_alloc_vmcs_sec[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_loaded_vmcs_init[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_free_loaded_vmcs[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_loaded_vmcs_init_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_free_loaded_vmcs_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_alloc_vmcs_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_vmx_vcpu_load_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_handle_vmcs_addr[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code__loaded_vmcs_clear[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_free_vmcs[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_vmcs_load[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_alloc_kvm_area[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_vmx_create_vcpu[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_tdp_page_fault[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };


uint8_t orig_code_vmalloc[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_kvm_vm_ioctl[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_kvm_mmu_load[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_vmx_flush_tlb[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_mmu_free_roots[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_mmu_spte_walk[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_vmx_handle_exit[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_handle_exit_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code___direct_map_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_try_async_pf_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_handle_pfn_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_handle_pfn_map[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_ioremap[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
/*
uint8_t orig_code_raw_readb[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_raw_readw[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_raw_readl[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_raw_readq[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_raw_writeb[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_raw_writew[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_raw_writel[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_raw_writeq[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
*/

uint8_t orig_code_memset_io[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
uint8_t orig_code_memcpy_fromio[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_memcpy_toio[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_kvm_arch_free_vm[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };

uint8_t orig_code_kvm_arch_alloc_vm[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };


uint8_t orig_code_ksm_page_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };



uint8_t orig_code_clean_page_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };


uint8_t orig_code_testforshrinkpage_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };


uint8_t orig_code_inflate_page_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };


uint8_t orig_code_deflate_page_modify[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };
/*

uint8_t orig_code_vmalloc[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0x0, 0x0
        };


*/

uint8_t *mem_cr3;
uint8_t *mem;
uint8_t *mem_pmd;
uint8_t *mem_pte;
uint8_t *mem_pagefault;
uint8_t *mem_vmcs_write64;
uint8_t *mem_vmcs_writel;
uint8_t *mem_vmcs_readl;
uint8_t *mem_vmcs_clear;
uint8_t *mem_handle_ept;
uint8_t *mem_kvm_mmu_page_fault;
uint8_t *mem_asm_sec_modify;
uint8_t *mem_vmx_vcpu_run;

uint8_t *mem_alloc_vmcs_sec;
uint8_t *mem_loaded_vmcs_init;
uint8_t *mem_free_loaded_vmcs;

uint8_t *mem_loaded_vmcs_init_modify;
uint8_t *mem_free_loaded_vmcs_modify;
uint8_t *mem_alloc_vmcs_modify;
uint8_t *mem_vmx_vcpu_load_modify;
uint8_t *mem_handle_vmcs_addr;

uint8_t *mem__loaded_vmcs_clear;
uint8_t *mem_free_vmcs;
uint8_t *mem_vmcs_load;

uint8_t *mem_alloc_kvm_area;
uint8_t *mem_vmx_create_vcpu;
uint8_t *mem_tdp_page_fault;
uint8_t *mem_vmalloc;
uint8_t *mem_kvm_vm_ioctl;

uint8_t *mem_kvm_mmu_load;
uint8_t *mem_vmx_flush_tlb;
uint8_t *mem_mmu_free_roots;
uint8_t *mem_mmu_spte_walk;

uint8_t *mem_vmx_handle_exit;
uint8_t *mem_handle_exit_modify;
uint8_t *mem___direct_map_modify;
uint8_t *mem_try_async_pf_modify;
uint8_t *mem_handle_pfn_modify;
uint8_t *mem_handle_pfn_map;

uint8_t *mem_ioremap;
/*
uint8_t *mem_raw_readb;
uint8_t *mem_raw_readw;
uint8_t *mem_raw_readl;
uint8_t *mem_raw_readq;

uint8_t *mem_raw_writeb;
uint8_t *mem_raw_writew;
uint8_t *mem_raw_writel;
uint8_t *mem_raw_writeq;
*/
uint8_t *mem_memcpy_fromio;
uint8_t *mem_memset_io;
uint8_t *mem_memcpy_toio;

uint8_t *mem_kvm_arch_alloc_vm;
uint8_t *mem_kvm_arch_free_vm;

uint8_t *mem_ksm_page_modify;

uint8_t *mem_clean_page_modify;
uint8_t *mem_testforshrinkpage_modify;


uint8_t *mem_inflate_page_modify;
uint8_t *mem_deflate_page_modify;

unsigned long old_cr3;

//vmalloc
/* make the page writable */
void make_rw(unsigned long address)
{
        unsigned int level;
        pte_t *pte = lookup_address(address, &level);//查找虚拟地址所在的页表地址
        //设置页表读写属性
        pte->pte |=  _PAGE_RW;
	//printk("pte : %lx\n",pte->pte);
}
EXPORT_SYMBOL(make_rw);


void make_invalid_addr(unsigned long address)
{
        unsigned int level;
        pte_t *pte = lookup_address(address, &level);//查找虚拟地址所在的页表地址
        //pmd_t *pmd = lookup_address((unsigned long)pte, &level);
        //设置页表读写属性
        //pmd->pmd |=  _PAGE_RW;
        pte->pte &= (0x0);
    //printk("pte : %lx\n",pte->pte);
}
EXPORT_SYMBOL(make_invalid_addr);


void find_make_invalid_addr(unsigned long address)
{
        unsigned int level;
        pte_t *pte = lookup_address(address, &level);//查找虚拟地址所在的页表地址
        //pmd_t *pmd = lookup_address((unsigned long)pte, &level);
        //设置页表读写属性
        //pmd->pmd |=  _PAGE_RW;
        printk("pte->pte is %lx\n", pte->pte );
    //printk("pte : %lx\n",pte->pte);
}
EXPORT_SYMBOL(find_make_invalid_addr);

/*
void set_hook(void *to_mock_func, void *mock_func , uint8_t* mem , uint8_t* orig_code)
{
        uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
  	//

        mem = (uint8_t *) to_mock_func;
        void *p;
	p = (uint8_t*) (mem - ((uint64_t) mem % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code, mem, sizeof(orig_code));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem, machine_code, sizeof(machine_code));

}

*/
void set_hook(void *to_mock_func, void *mock_func)
{
        uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
        void *p; 	//

        mem = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem - ((uint64_t) mem % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code, mem, sizeof(orig_code));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem, machine_code, sizeof(machine_code));

}
EXPORT_SYMBOL(set_hook);

void set_hook_cr31(void *to_mock_func, void *mock_func)
{
        uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
            //jmpq *%rax
        };

        int pagesize = 4096;
  	//
        void *p;
        mem_cr3 = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_cr3 - ((uint64_t) mem_cr3 % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_cr3, mem_cr3, sizeof(orig_code_cr3));
        memcpy(machine_code + 1, &mock_func, sizeof(mock_func));
        memcpy(mem_cr3, machine_code, sizeof(machine_code));

}


void set_hook_pte1(void *to_mock_func, void *mock_func)
{
        uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
            //jmpq *%rax
        };

        int pagesize = 4096;
  	//
        void *p;
        mem_pte = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_pte - ((uint64_t) mem_pte % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_pte, mem_pte, sizeof(orig_code_pte));
        memcpy(machine_code + 1, &mock_func, sizeof(mock_func));
        memcpy(mem_pte, machine_code, sizeof(machine_code));

}

void set_hook_cr3(void *to_mock_func, void *mock_func)
{
        uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
  	//
        void *p;
        mem_cr3 = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_cr3 - ((uint64_t) mem_cr3 % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_cr3, mem_cr3, sizeof(orig_code_cr3));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_cr3, machine_code, sizeof(machine_code));

}
EXPORT_SYMBOL(set_hook_cr3);

void set_hook_pte(void *to_mock_func, void *mock_func)
{
        uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
  	//
        void *p;
        mem_pte = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_pte - ((uint64_t) mem_pte % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_pte, mem_pte, sizeof(orig_code_pte));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_pte, machine_code, sizeof(machine_code));

}
EXPORT_SYMBOL(set_hook_pte);

void set_hook_pmd(void *to_mock_func, void *mock_func)
{
        uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
  	//
        void *p;
        mem_pmd = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_pmd - ((uint64_t) mem_pmd % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_pmd, mem_pmd, sizeof(orig_code_pmd));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_pmd, machine_code, sizeof(machine_code));

}
EXPORT_SYMBOL(set_hook_pmd);

void set_hook_pagefault(void *to_mock_func, void *mock_func) //mock new function //to mock func : old function
{
        uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
        void *p;

        mem_pagefault = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_pagefault - ((uint64_t) mem_pagefault % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_pagefault, mem_pagefault, sizeof(orig_code_pagefault));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_pagefault, machine_code, sizeof(machine_code));

}
EXPORT_SYMBOL(set_hook_pagefault);

void make_no_rw(unsigned long address)
{
        unsigned int level;
        pte_t *pte = lookup_address(address, &level);//查找虚拟地址所在的页表地址
        //设置页表读写属性
        pte->pte ^=  _PAGE_RW;
	printk("pte no rw : %lx\n",pte->pte);
}
EXPORT_SYMBOL(make_no_rw);

void set_hook_vmcs_write64(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
        void *p;

        mem_vmcs_write64 = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_vmcs_write64 - ((uint64_t) mem_vmcs_write64 % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_vmcs_write64, mem_vmcs_write64, sizeof(orig_code_vmcs_write64));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmcs_write64, machine_code, sizeof(machine_code));
}
EXPORT_SYMBOL(set_hook_vmcs_write64);

void set_hook_vmcs_writel(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
        void *p;

        mem_vmcs_writel = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_vmcs_writel - ((uint64_t) mem_vmcs_writel % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_vmcs_writel, mem_vmcs_writel, sizeof(orig_code_vmcs_writel));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmcs_writel, machine_code, sizeof(machine_code));
}
EXPORT_SYMBOL(set_hook_vmcs_writel);

void set_hook_vmcs_readl(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
        void *p;

        mem_vmcs_readl = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_vmcs_readl - ((uint64_t) mem_vmcs_readl % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_vmcs_readl, mem_vmcs_readl, sizeof(orig_code_vmcs_readl));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmcs_readl, machine_code, sizeof(machine_code));
}
EXPORT_SYMBOL(set_hook_vmcs_readl);

void set_hook_vmcs_clear(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        int pagesize = 4096;
        void *p;

        mem_vmcs_clear = (uint8_t *) to_mock_func;

	p = (uint8_t*) (mem_vmcs_clear - ((uint64_t) mem_vmcs_clear % pagesize));
        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_vmcs_clear, mem_vmcs_clear, sizeof(orig_code_vmcs_clear));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmcs_clear, machine_code, sizeof(machine_code));
}
EXPORT_SYMBOL(set_hook_vmcs_clear);

void set_hook_handle_ept_violation_sec(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_handle_ept = (uint8_t *) to_mock_func;

        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_handle_ept, mem_handle_ept, sizeof(orig_code_handle_ept));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_handle_ept, machine_code, sizeof(machine_code));
}
EXPORT_SYMBOL(set_hook_handle_ept_violation_sec);


void set_hook_kvm_mmu_page_fault(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_kvm_mmu_page_fault = (uint8_t *) to_mock_func;

        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_kvm_mmu_page_fault, mem_kvm_mmu_page_fault, sizeof(orig_code_kvm_mmu_page_fault));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_kvm_mmu_page_fault, machine_code, sizeof(machine_code));
}
EXPORT_SYMBOL(set_hook_kvm_mmu_page_fault);


void set_hook_asm_sec_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_asm_sec_modify = (uint8_t *) to_mock_func;

        make_rw((unsigned long)to_mock_func);

        //改写立即数为mock的地址,写入函数入口处
	memcpy(orig_code_asm_sec_modify, mem_asm_sec_modify, sizeof(orig_code_asm_sec_modify));
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_asm_sec_modify, machine_code, sizeof(machine_code));
}
EXPORT_SYMBOL(set_hook_asm_sec_modify);


void set_hook_vmx_vcpu_run(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_vmx_vcpu_run = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_vmx_vcpu_run, mem_vmx_vcpu_run, sizeof(orig_code_vmx_vcpu_run));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmx_vcpu_run, machine_code, sizeof(machine_code));
}

void set_hook_alloc_vmcs_sec(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_alloc_vmcs_sec = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_alloc_vmcs_sec, mem_alloc_vmcs_sec, sizeof(orig_code_alloc_vmcs_sec));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_alloc_vmcs_sec, machine_code, sizeof(machine_code));
}

void set_hook_loaded_vmcs_init(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_loaded_vmcs_init = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_loaded_vmcs_init, mem_loaded_vmcs_init, sizeof(orig_code_loaded_vmcs_init));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_loaded_vmcs_init, machine_code, sizeof(machine_code));
}


void set_hook_free_loaded_vmcs(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_free_loaded_vmcs = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_free_loaded_vmcs, mem_free_loaded_vmcs, sizeof(orig_code_free_loaded_vmcs));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_free_loaded_vmcs, machine_code, sizeof(machine_code));
}


void set_hook_alloc_vmcs_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_alloc_vmcs_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_alloc_vmcs_modify, mem_alloc_vmcs_modify, sizeof(orig_code_alloc_vmcs_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_alloc_vmcs_modify, machine_code, sizeof(machine_code));
}

void set_hook_free_loaded_vmcs_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_free_loaded_vmcs_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_free_loaded_vmcs_modify, mem_free_loaded_vmcs_modify, sizeof(orig_code_free_loaded_vmcs_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_free_loaded_vmcs_modify, machine_code, sizeof(machine_code));
}

void set_hook_loaded_vmcs_init_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_loaded_vmcs_init_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_loaded_vmcs_init_modify, mem_loaded_vmcs_init_modify, sizeof(orig_code_loaded_vmcs_init_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_loaded_vmcs_init_modify, machine_code, sizeof(machine_code));
}

void set_hook_vmx_vcpu_load_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_vmx_vcpu_load_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_vmx_vcpu_load_modify, mem_vmx_vcpu_load_modify, sizeof(orig_code_vmx_vcpu_load_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmx_vcpu_load_modify, machine_code, sizeof(machine_code));
}

void set_hook_handle_vmcs_addr(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_handle_vmcs_addr = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_handle_vmcs_addr, mem_handle_vmcs_addr, sizeof(orig_code_handle_vmcs_addr));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_handle_vmcs_addr, machine_code, sizeof(machine_code));
}

void set_hook__loaded_vmcs_clear(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem__loaded_vmcs_clear = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code__loaded_vmcs_clear, mem__loaded_vmcs_clear, sizeof(orig_code__loaded_vmcs_clear));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem__loaded_vmcs_clear, machine_code, sizeof(machine_code));
}


void set_hook_free_vmcs(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_free_vmcs = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_free_vmcs, mem_free_vmcs, sizeof(orig_code_free_vmcs));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_free_vmcs, machine_code, sizeof(machine_code));
}

void set_hook_vmcs_load(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_vmcs_load = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_vmcs_load, mem_vmcs_load, sizeof(orig_code_vmcs_load));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmcs_load, machine_code, sizeof(machine_code));
}


void set_hook_alloc_kvm_area(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_alloc_kvm_area = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_alloc_kvm_area, mem_alloc_kvm_area, sizeof(orig_code_alloc_kvm_area));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_alloc_kvm_area, machine_code, sizeof(machine_code));
}

void set_hook_vmx_create_vcpu(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_vmx_create_vcpu = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_vmx_create_vcpu, mem_vmx_create_vcpu, sizeof(orig_code_vmx_create_vcpu));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmx_create_vcpu, machine_code, sizeof(machine_code));
}


void set_hook_tdp_page_fault(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_tdp_page_fault = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_tdp_page_fault, mem_tdp_page_fault, sizeof(orig_code_tdp_page_fault));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_tdp_page_fault, machine_code, sizeof(machine_code));
}
//vmalloc
/*
void set_hook_vmalloc(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_vmalloc = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_vmalloc, mem_vmalloc, sizeof(orig_code_vmalloc));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmalloc, machine_code, sizeof(machine_code));
}


*/
void set_hook_kvm_vm_ioctl(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_kvm_vm_ioctl = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_kvm_vm_ioctl, mem_kvm_vm_ioctl, sizeof(orig_code_kvm_vm_ioctl));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_kvm_vm_ioctl, machine_code, sizeof(machine_code));
}


void set_hook_kvm_mmu_load(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_kvm_mmu_load = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_kvm_mmu_load, mem_kvm_mmu_load, sizeof(orig_code_kvm_mmu_load));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_kvm_mmu_load, machine_code, sizeof(machine_code));
}

void set_hook_vmx_flush_tlb(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_vmx_flush_tlb = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_vmx_flush_tlb, mem_vmx_flush_tlb, sizeof(orig_code_vmx_flush_tlb));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmx_flush_tlb, machine_code, sizeof(machine_code));
}



void set_hook_mmu_free_roots(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_mmu_free_roots = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_mmu_free_roots, mem_mmu_free_roots, sizeof(orig_code_mmu_free_roots));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_mmu_free_roots, machine_code, sizeof(machine_code));
}

/*
void set_hook_mmu_spte_walk(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_mmu_spte_walk = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_mmu_spte_walk, mem_mmu_spte_walk, sizeof(orig_code_mmu_spte_walk));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_mmu_spte_walk, machine_code, sizeof(machine_code));
}
*/


void set_hook_vmx_handle_exit(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_vmx_handle_exit = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_vmx_handle_exit, mem_vmx_handle_exit, sizeof(orig_code_vmx_handle_exit));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_vmx_handle_exit, machine_code, sizeof(machine_code));
}


void set_hook_handle_exit_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_handle_exit_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_handle_exit_modify, mem_handle_exit_modify, sizeof(orig_code_handle_exit_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_handle_exit_modify, machine_code, sizeof(machine_code));
}

void set_hook___direct_map_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem___direct_map_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code___direct_map_modify, mem___direct_map_modify, sizeof(orig_code___direct_map_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem___direct_map_modify, machine_code, sizeof(machine_code));
}



void set_hook_try_async_pf_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_try_async_pf_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_try_async_pf_modify, mem_try_async_pf_modify, sizeof(orig_code_try_async_pf_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_try_async_pf_modify, machine_code, sizeof(machine_code));
}

void set_hook_handle_pfn_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_handle_pfn_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_handle_pfn_modify, mem_handle_pfn_modify, sizeof(orig_code_handle_pfn_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_handle_pfn_modify, machine_code, sizeof(machine_code));
}

void set_hook_handle_pfn_map(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_handle_pfn_map = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_handle_pfn_map, mem_handle_pfn_map, sizeof(orig_code_handle_pfn_map));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_handle_pfn_map, machine_code, sizeof(machine_code));
}

void set_hook_ioremap(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_ioremap = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_ioremap, mem_ioremap, sizeof(orig_code_ioremap));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_ioremap, machine_code, sizeof(machine_code));
}

/*
void set_hook_raw_readb(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_raw_readb = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_raw_readb, mem_raw_readb, sizeof(orig_code_raw_readb));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_raw_readb, machine_code, sizeof(machine_code));
}

void set_hook_raw_readw(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_raw_readw = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_raw_readw, mem_raw_readw, sizeof(orig_code_raw_readw));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_raw_readw, machine_code, sizeof(machine_code));
}


void set_hook_raw_readl(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_raw_readl = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_raw_readl, mem_raw_readl, sizeof(orig_code_raw_readl));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_raw_readl, machine_code, sizeof(machine_code));
}

void set_hook_raw_readq(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_raw_readq = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_raw_readq, mem_raw_readq, sizeof(orig_code_raw_readq));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_raw_readq, machine_code, sizeof(machine_code));
}



void set_hook_raw_writeb(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_raw_writeb = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_raw_writeb, mem_raw_writeb, sizeof(orig_code_raw_writeb));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_raw_writeb, machine_code, sizeof(machine_code));
}

void set_hook_raw_writew(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_raw_writew = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_raw_writew, mem_raw_writew, sizeof(orig_code_raw_writew));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_raw_writew, machine_code, sizeof(machine_code));
}

void set_hook_raw_writel(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_raw_writel = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_raw_writel, mem_raw_writel, sizeof(orig_code_raw_writel));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_raw_writel, machine_code, sizeof(machine_code));
}

void set_hook_raw_writeq(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_raw_writeq = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_raw_writeq, mem_raw_writeq, sizeof(orig_code_raw_writeq));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_raw_writeq, machine_code, sizeof(machine_code));
}
*/

void set_hook_memcpy_fromio(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_memcpy_fromio = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_memcpy_fromio, mem_memcpy_fromio, sizeof(orig_code_memcpy_fromio));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_memcpy_fromio, machine_code, sizeof(machine_code));
}


void set_hook_memset_io(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_memset_io = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_memset_io, mem_memset_io, sizeof(orig_code_memset_io));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_memset_io, machine_code, sizeof(machine_code));
}

void set_hook_memcpy_toio(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_memcpy_toio = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_memcpy_toio, mem_memcpy_toio, sizeof(orig_code_memcpy_toio));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_memcpy_toio, machine_code, sizeof(machine_code));
}

void set_hook_kvm_arch_alloc_vm(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_kvm_arch_alloc_vm = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_kvm_arch_alloc_vm, mem_kvm_arch_alloc_vm, sizeof(orig_code_kvm_arch_alloc_vm));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_kvm_arch_alloc_vm, machine_code, sizeof(machine_code));
}


void set_hook_kvm_arch_free_vm(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_kvm_arch_free_vm = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_kvm_arch_free_vm, mem_kvm_arch_free_vm, sizeof(orig_code_kvm_arch_free_vm));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_kvm_arch_free_vm, machine_code, sizeof(machine_code));
}


void set_hook_ksm_page_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_ksm_page_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_ksm_page_modify, mem_ksm_page_modify, sizeof(orig_code_ksm_page_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_ksm_page_modify, machine_code, sizeof(machine_code));
}

void set_hook_clean_page_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_clean_page_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_clean_page_modify, mem_clean_page_modify, sizeof(orig_code_clean_page_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_clean_page_modify, machine_code, sizeof(machine_code));
}

void set_hook_testforshrinkpage_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_testforshrinkpage_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_testforshrinkpage_modify, mem_testforshrinkpage_modify, sizeof(orig_code_testforshrinkpage_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_testforshrinkpage_modify, machine_code, sizeof(machine_code));
}


void set_hook_inflate_page_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_inflate_page_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_inflate_page_modify, mem_inflate_page_modify, sizeof(orig_code_inflate_page_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_inflate_page_modify, machine_code, sizeof(machine_code));
}



void set_hook_deflate_page_modify(void *to_mock_func, void *mock_func)
{
	uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

        mem_deflate_page_modify = (uint8_t *) to_mock_func; //原来函数

        make_rw((unsigned long)to_mock_func);//原来函数所在物理页可写

	//保存原来函数代码段中前2行机器码，等待后续模块卸载的时候恢复原来函数
	memcpy(orig_code_deflate_page_modify, mem_deflate_page_modify, sizeof(orig_code_deflate_page_modify));
	
        //改写立即数为mock的地址,写入函数入口处
        memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
        memcpy(mem_deflate_page_modify, machine_code, sizeof(machine_code));
}


