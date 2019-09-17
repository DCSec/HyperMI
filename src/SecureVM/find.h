#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kernel.h> 
#include <asm/pgtable.h> 
#include <asm/page_types.h>
#include <linux/mm_types.h>
#include <linux/mm.h> 
#include <linux/sched.h> 
#include <asm/tlbflush.h>
#include "struct_pt.h"
#define PAGE_SHIFT 12

static unsigned long mask_pt = 0xfffffffff000;

unsigned long find(unsigned long address,unsigned long current_pgd,struct pt * it)
{
	unsigned long addr_tmp1,index_trans_pgd,index_trans_pud,index_trans_pmd,index_trans_pte,index_trans_offset; //index
	//unsigned long a;	
	unsigned long *offset;
	unsigned long *aa;
	unsigned long a2s;
	unsigned long aaa;
	pgd_t * pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte1;


	addr_tmp1 = address ;//<< 16;
	index_trans_pgd = addr_tmp1 & 0x0000ff8000000000;
	index_trans_pgd = index_trans_pgd >> 39;
	index_trans_pud = addr_tmp1 & 0x0000007fc0000000;
	index_trans_pud = index_trans_pud >> 30;
	index_trans_pmd = addr_tmp1 & 0x000000003fe00000;
	index_trans_pmd = index_trans_pmd >> 21;
	index_trans_pte = addr_tmp1 & 0x00000000001ff000;
	index_trans_pte = index_trans_pte >> 12;
	index_trans_offset = addr_tmp1 & 0x0000000000000fff;

	it->index_pgd = index_trans_pgd;
	it->index_pud = index_trans_pud;
	it->index_pmd = index_trans_pmd;
	it->index_pte = index_trans_pte;
	it->index_offset = index_trans_offset;
	it->addr = address;

	//unsigned long mask_pt = 0x3fffff000; 
	pgd = (pgd_t*)(current_pgd );
	pud = (pud_t*)__va( (pgd[index_trans_pgd].pgd) & (mask_pt) );
	if(( (pgd[index_trans_pgd].pgd) & (mask_pt) ) == 0)
	{
		//printk("pgd\n");
		it->flag_valid = 0;
		return 0;
	}
	it->pgd = pgd[index_trans_pgd];	
	it->pgd_addr = &(pgd[index_trans_pgd]);	//pgd + index_trans_pgd * sizeof(unsigned long);
	it->level = 4;

	pgd[index_trans_pgd].pgd |= _PAGE_RW; 
	
	
	if(( (pud[index_trans_pud].pud) & (mask_pt)  ) == 0)
	{	//printk("pud\n");
		it->flag_valid = 0;
		return 0;
	}
	pmd = (pmd_t*)__va( (pud[index_trans_pud].pud) & (mask_pt) );
	
	(pud[index_trans_pud].pud) |= _PAGE_RW; 

	it->pud = pud[index_trans_pud];
	it->pud_addr = &(pud[index_trans_pud]);
	it->level = 3;

	if (pud_large( (pud[index_trans_pud]) ))
	{
		//printk("1 GB page \n");
		a2s = (unsigned long)__va( (pud[index_trans_pud].pud) & (mask_pt) ) + (index_trans_pte << 12) + (index_trans_offset) + (index_trans_pmd << 21);
		//printk("value-pud1G : %lx\n",*(unsigned long*)((a2s)));
		it->value = *(unsigned long*)((a2s));
		it->flag_valid = 1;
		return 0;
	}
	
	
	if(( (pmd[index_trans_pmd].pmd) & (mask_pt)  ) == 0)
	{	//printk("pmd\n");
		it->flag_valid = 0;
		return 0;
	}

	it->pmd = pmd[index_trans_pmd];
	it->pmd_addr = &(pmd[index_trans_pmd]);
	it->level = 2;

	pmd[index_trans_pmd].pmd |= _PAGE_RW;

	if (pmd_large( (pmd[index_trans_pmd]) ))
	{
		a2s = (unsigned long)__va( (pmd[index_trans_pmd].pmd) & (mask_pt) ) + (index_trans_pte << 12) + (index_trans_offset);
		//printk("value-pmd2m : %lx\n",*(unsigned long*)((a2s)));
		it->value = *(unsigned long*)((a2s));
		//printk("2 MB page \n");
		pmd[index_trans_pmd].pmd ^= _PAGE_RW;
		pmd[index_trans_pmd].pmd ^= _PAGE_RW;
		aaa = pmd[index_trans_pmd].pmd;
		//pmd[index_trans_pmd].pmd &=0xfff;
		pmd[index_trans_pmd].pmd = aaa;
		it->flag_valid = 1;
		return 0;
	}
	pte1 = (pte_t*)__va( (pmd[index_trans_pmd].pmd) & (mask_pt) );

	pte1[index_trans_pte].pte |= _PAGE_RW;
	pte1[index_trans_pte].pte ^= _PAGE_RW;
	pte1[index_trans_pte].pte ^= _PAGE_RW;

	offset = (unsigned long *)__va( (pte1[index_trans_pte].pte) & (mask_pt) );
	if((  (pte1[index_trans_pte].pte) & (mask_pt)  ) == 0)
	{	//printk("pte\n");
		it->flag_valid = 0;
		return 0;
	}
	it->pte = pte1[index_trans_pte];
	it->pte_addr = &(pte1[index_trans_pte]);
	it->level = 1;
	aa = (offset);//+index_trans_offset);
	//printk("aa  : %p\n",aa);
	//printk("value-find1 : %lx\n",*(unsigned long*)((aa+index_trans_offset/8)));
	it->value = *(unsigned long*)((aa+index_trans_offset/8));
	it->flag_valid = 1;
	return 0;
	//return (&(pte1[index_trans_pte]));
}
EXPORT_SYMBOL(find);




