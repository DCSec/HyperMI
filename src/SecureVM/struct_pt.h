#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kernel.h> 
#include <asm/pgtable.h> 
#include <asm/page_types.h>
#include <linux/mm_types.h>
#include <linux/mm.h> 
#include <linux/sched.h> 
#include <asm/tlbflush.h>

struct pt 
{
	unsigned long addr;
	int level;
	pgd_t pgd;
	pud_t pud;
	pmd_t pmd;
	pte_t pte;

	unsigned long index_pgd;
	unsigned long index_pud;
	unsigned long index_pmd;
	unsigned long index_pte;
	unsigned long index_offset;

	unsigned long value;

	int flag_valid;//0 invalid  1 for valid

	pgd_t *pgd_addr;
	pud_t *pud_addr;
	pmd_t *pmd_addr;
	pte_t *pte_addr;

};



/*
void set_prot_own(struct pt* it)
{
	unsigned long *address;
	if(it->level == 3)
	{
		address = (unsigned long *)it->addr_pgd ;
		*address |= _PAGE_RW;
		address = (unsigned long *)it->addr_pud ;
		*address |= _PAGE_RW;
		*address ^= _PAGE_RW;
	}
	if(it->level == 2)
	{
		address = (unsigned long *)it->addr_pgd ;
		*address |= _PAGE_RW;
		address = (unsigned long *)it->addr_pud ;
		*address |= _PAGE_RW;
		address = (unsigned long *)it->addr_pmd ;
		*address |= _PAGE_RW;
		*address ^= _PAGE_RW;
	}
	if(it->level == 1)
	{
		address = (unsigned long *)it->addr_pgd ;
		*address |= _PAGE_RW;
		address = (unsigned long *)it->addr_pud ;
		*address |= _PAGE_RW;
		address = (unsigned long *)it->addr_pmd ;
		*address |= _PAGE_RW;
		address = (unsigned long *)it->addr_pte ;
		*address |= _PAGE_RW;
		*address ^= _PAGE_RW;
	}
}
*/


