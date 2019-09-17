#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kernel.h> 
#include <asm/pgtable.h> 
#include <asm/page_types.h>
#include <linux/mm_types.h>
#include <linux/mm.h> 
#include <linux/sched.h> 
#define PAGE_SHIFT 12

static unsigned long mask_pgprot1 = 0xfffffffff000;

void set_pgtable_prot3(pgd_t* addr) // just for kernel pgd page ( > 512 / 2)
{
	int i1,i2,i3,i4;
	pgd_t* pgd_new;
	pud_t* pud_new;
	pmd_t* pmd_new;
	pte_t* pte_new;

	pgd_new = addr;	


	for(i1 = 512/2  ;i1 < 512;i1++)
	{
		if( !(pgd_new[i1].pgd & mask_pgprot1) )
			continue;
		if(pgd_none(pgd_new[i1]))
			continue;
		pgd_new[i1].pgd |= _PAGE_RW;
	}
}



void set_pgtable_prot5(pgd_t* addr) // pmd is  writable ( > 512 / 2)
{
	int i1,i2,i3,i4;
	pgd_t* pgd_new;
	pud_t* pud_new;
	pmd_t* pmd_new;
	pte_t* pte_new;

	pgd_new = addr;	


	for(i1 = 512/2  ;i1 < 512;i1++)
	{
		if( !(pgd_new[i1].pgd & mask_pgprot1) )
			continue;
		//printk("pgd_new[%d] : 0x%lx\n",i1,pgd_new[i1].pgd);

		pud_new = (pud_t*)__va(pgd_new[i1].pgd & mask_pgprot1 );
		for(i2 = 0;i2 < 512;i2++){
			if( !(pud_new[i2].pud & mask_pgprot1) )
				continue;
			//printk("       pud_new[%d] : 0x%lx\n",i2,pud_new[i2].pud);

			pmd_new = (pmd_t*)__va(pud_new[i2].pud & mask_pgprot1 );
			if( pud_large(pud_new[i2]) )
			{
				//if( (pud_new[i2].pud &=_PAGE_RW) == _PAGE_RW )
 					//pud_new[i2].pud ^=_PAGE_RW;
				continue;
			}
			for(i3 = 0;i3 < 512;i3++){
				if( !(pmd_new[i3].pmd & mask_pgprot1) )
					continue;
				//printk("                pmd_new[%d] : 0x%lx\n",i3,pmd_new[i3].pmd);
				if( (pud_new[i2].pud &_PAGE_RW) == _PAGE_RW )//this pmd protect bit is  writable
					pmd_new[i3].pmd |=_PAGE_RW;
				pte_new = (pte_t*)__va(pmd_new[i3].pmd & mask_pgprot1 );
				if( pmd_large(pmd_new[i3]) )
				{
					//if( (pmd_new[i3].pmd &=_PAGE_RW) == _PAGE_RW )//this pmd protect bit is  writable
						//pmd_new[i3].pmd ^=_PAGE_RW;
					continue;
				}
			}	
		}
	}
}
