#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kernel.h> 
#include <asm/pgtable.h> 
#include <asm/page_types.h>
#include <linux/mm_types.h>
#include <linux/mm.h> 
#include <linux/sched.h> 
#include <asm/tlbflush.h>
#include "find.h"
#define PAGE_SHIFT 12

static unsigned long mask = 0xfffffffff000;
static unsigned long pgd_addr_cp;
pte_t unlong2pte(unsigned long a);

int is_invalid(unsigned long addr)
{
	unsigned long a = addr & (mask);
	if(!a)
		return 1;//invalid
	else
		return 0;//valid

}
unsigned long getflag_pgx(unsigned long addr)
{
	return ( addr & (0xfc00000000000fff) );
}

void printk_pgtable(pgd_t* addr)
{
	int i1,i2,i3,i4;
	pgd_t* pgd_new;
	pud_t* pud_new;
	pmd_t* pmd_new;
	pte_t* pte_new;
	int count = 0;	

	pgd_new = addr;	


	for(i1 = 0;i1 < 512;i1++)
	{
		if( !(pgd_new[i1].pgd & mask) )
			continue;
		//printk("pgd_new[%d] : 0x%lx\n",i1,pgd_new[i1].pgd);
		count++;

		pud_new = (pud_t*)__va(pgd_new[i1].pgd & mask );
		for(i2 = 0;i2 < 512;i2++){
			if( !(pud_new[i2].pud & mask) )
				continue;
			//printk("       pud_new[%d] : 0x%lx\n",i2,pud_new[i2].pud);
			count++;

			pmd_new = (pmd_t*)__va(pud_new[i2].pud & mask );
			if( pud_large(pud_new[i2]) )
				continue;
			for(i3 = 0;i3 < 512;i3++){
				if( !(pmd_new[i3].pmd & mask) )
					continue;
				//printk("                pmd_new[%d] : 0x%lx\n",i3,pmd_new[i3].pmd);
				count++;

				pte_new = (pte_t*)__va(pmd_new[i3].pmd & mask );
				if( pmd_large(pmd_new[i3]) )
					continue;
				for(i4 = 0;i4 < 512;i4++){
					if( !(pte_new[i4].pte & mask) )
						continue;
					//printk("                pte_new[%d] : 0x%lx\n",i4,pte_new[i4].pte);
					//count++;
				}
			}
		}
		
	}
	printk("count - p : %d\n",count);
}
unsigned long copy_pagetable(void)
{
	pgd_t* pgd_new, *pgd_old;
	pud_t* pud_new, *pud_old;
	pmd_t* pmd_new, *pmd_old;
	pte_t* pte_new, *pte_old;

	

	int i1,i2,i3,i4;
	unsigned long a,b;
	unsigned long flag_pgd,flag_pud,flag_pmd;
	unsigned long count = 0;	

	pgd_old = (pgd_t*)__va(read_cr3());
	a = get_zeroed_page(__GFP_HIGHMEM);
	if(a)
		count++;
	while(!a)
	{
		a = get_zeroed_page(__GFP_HIGHMEM);
		if(a)
			count++;
		printk("a is NULL\n");
	}
	
	pgd_new = (pgd_t*)a;
	
	for(i1 = 512/2;i1 < 512;i1++)
	{
		//pgd_new[i1].pgd = pgd_old[i1].pgd;
		set_pgd(pgd_new+i1,pgd_old[i1]);

		b = is_invalid(pgd_old[i1].pgd);
		if(b)
		{
			//pgd_new[i1].pgd = 0x0;
			continue;
		}				
		else
		{
			flag_pgd = getflag_pgx(pgd_old[i1].pgd);
			a = get_zeroed_page(__GFP_HIGHMEM);
			if(a)
				count++;
			while(!a)
	  		{
				a = get_zeroed_page(__GFP_HIGHMEM);
				if(a)
					count++;
				printk("a is NULL\n");
			}
			//pgd_new[i1].pgd = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pgd );
			set_pgd(pgd_new+i1,__pgd( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pgd ));

			pud_old = (pud_t*)__va(pgd_old[i1].pgd & mask); //real vitual address for pud
			pud_new = (pud_t*)a;

			for(i2 = 0;i2 < 512;i2++)
			{
				//pud_new[i2].pud = pud_old[i2].pud;
				set_pud(pud_new+i2,pud_old[i2]);

				b = is_invalid(pud_old[i2].pud);
				if(b)
				{
					//pud_new[i2].pud = 0x0;
					continue;
				}				
				else
				{
					if(pud_large(pud_old[i2]))
					{
						pud_new[i2].pud = pud_old[i2].pud;
						continue; //1GB page
					}
					else
					{
						flag_pud = getflag_pgx(pud_old[i2].pud);
						a = get_zeroed_page(__GFP_HIGHMEM);
						if(a)
							count++;
						while(!a)
		  				{
							a = get_zeroed_page(__GFP_HIGHMEM);
							if(a)
								count++;
							printk("a is NULL\n");
						}
	
						pud_new[i2].pud = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pud );
						set_pud(pud_new+i2,__pud( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pud ));

						pmd_old = (pmd_t*)__va(pud_old[i2].pud & mask); //real vitual address for pmd
						pmd_new = (pmd_t*)a;

					
					
						for(i3 = 0;i3 < 512;i3++)
						{
							pmd_new[i3].pmd = pmd_old[i3].pmd;
							set_pmd(pmd_new+i3,pmd_old[i3]);

							b = is_invalid(pmd_old[i3].pmd);
							if(b)
							{
								pmd_new[i3].pmd = 0x0;
								continue;
							}				
							else
							{
								if(pmd_large(pmd_old[i3]))
								{
									pmd_new[i3].pmd = pmd_old[i3].pmd;
									continue; //2M page
								}
								else
								{
									flag_pmd = getflag_pgx(pmd_old[i3].pmd);
									a = get_zeroed_page(__GFP_HIGHMEM);
									if(a)
										count++;
									while(!a)
		  							{
										a = get_zeroed_page(__GFP_HIGHMEM);
										if(a)
											count++;
										printk("a is NULL\n");
									}
									pmd_new[i3].pmd = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pmd );
									set_pmd(pmd_new+i2,__pmd( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pmd ));

									pte_old = (pte_t*)__va(pmd_old[i3].pmd & mask); //real vitual address for pte
									pte_new = (pte_t*)a ;

								
								
									for(i4 = 0;i4 < 512;i4++)
									{
										pte_new[i4].pte= pte_old[i4].pte;
										set_pte(pte_new+i4,pte_old[i4]);
									
									}
								}
							}
						}
					}
				}
			}
			
		}
		
	}
	
	//printk_pgtable(pgd_new);

	//printk("\ncount : %d\n",count);
	__flush_tlb_all();
	return ((unsigned long )pgd_new);
}
EXPORT_SYMBOL(copy_pagetable);

pte_t unlong2pte(unsigned long a)
{
	pte_t b;
	b.pte = a;
	return b;
}


void clear_newpgd_addr(unsigned long pgd)
{
	struct pt it;
	find(pgd,(unsigned long) __va(read_cr3()),&it);
	it.pte.pte &= 0x0;

}

unsigned long cl_cp_pgtable(void)
{
	unsigned long pgd_new = copy_pagetable();
	clear_newpgd_addr(pgd_new);
	return pgd_new;
}
EXPORT_SYMBOL(cl_cp_pgtable);

void set_pgd_cp(void)
{
	pgd_addr_cp = cl_cp_pgtable();
	//printk("setpgd is done! \n");
}

unsigned long get_pgd_cp(void)
{
	return pgd_addr_cp ;
}


void copy_pagetable_half(unsigned long cp_new_pg)
{
	pgd_t* pgd_new, *pgd_old;
	pud_t* pud_new, *pud_old;
	pmd_t* pmd_new, *pmd_old;
	pte_t* pte_new, *pte_old;

	

	int i1,i2,i3,i4;
	unsigned long a,b;
	unsigned long flag_pgd,flag_pud,flag_pmd;
	unsigned long count = 0;	

	pgd_old = (pgd_t*)__va(read_cr3());
	/*
	a = get_zeroed_page(__GFP_HIGHMEM);
	if(a)
		count++;
	while(!a)
	{
		a = get_zeroed_page(__GFP_HIGHMEM);
		if(a)
			count++;
		printk("a is NULL\n");
	}
	*/
	pgd_new = (pgd_t*)(cp_new_pg);
	
	for(i1 = 512/2;i1 < 512;i1++)
	{
		//pgd_new[i1].pgd = pgd_old[i1].pgd;
		set_pgd(pgd_new+i1,pgd_old[i1]);

		b = is_invalid(pgd_old[i1].pgd);
		if(b)
		{
			//pgd_new[i1].pgd = 0x0;
			continue;
		}				
		else
		{
			flag_pgd = getflag_pgx(pgd_old[i1].pgd);
			a = get_zeroed_page(__GFP_HIGHMEM);
			if(a)
				count++;
			while(!a)
	  		{
				a = get_zeroed_page(__GFP_HIGHMEM);
				if(a)
					count++;
				printk("a is NULL\n");
			}
			//pgd_new[i1].pgd = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pgd );
			set_pgd(pgd_new+i1,__pgd( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pgd ));

			pud_old = (pud_t*)__va(pgd_old[i1].pgd & mask); //real vitual address for pud
			pud_new = (pud_t*)a;

			for(i2 = 0;i2 < 512;i2++)
			{
				//pud_new[i2].pud = pud_old[i2].pud;
				set_pud(pud_new+i2,pud_old[i2]);

				b = is_invalid(pud_old[i2].pud);
				if(b)
				{
					//pud_new[i2].pud = 0x0;
					continue;
				}				
				else
				{
					if(pud_large(pud_old[i2]))
					{
						pud_new[i2].pud = pud_old[i2].pud;
						continue; //1GB page
					}
					else
					{
						flag_pud = getflag_pgx(pud_old[i2].pud);
						a = get_zeroed_page(__GFP_HIGHMEM);
						if(a)
							count++;
						while(!a)
		  				{
							a = get_zeroed_page(__GFP_HIGHMEM);
							if(a)
								count++;
							printk("a is NULL\n");
						}
	
						pud_new[i2].pud = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pud );
						set_pud(pud_new+i2,__pud( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pud ));

						pmd_old = (pmd_t*)__va(pud_old[i2].pud & mask); //real vitual address for pmd
						pmd_new = (pmd_t*)a;

					
					
						for(i3 = 0;i3 < 512;i3++)
						{
							pmd_new[i3].pmd = pmd_old[i3].pmd;
							set_pmd(pmd_new+i3,pmd_old[i3]);

							b = is_invalid(pmd_old[i3].pmd);
							if(b)
							{
								pmd_new[i3].pmd = 0x0;
								continue;
							}				
							else
							{
								if(pmd_large(pmd_old[i3]))
								{
									pmd_new[i3].pmd = pmd_old[i3].pmd;
									continue; //2M page
								}
								else
								{
									flag_pmd = getflag_pgx(pmd_old[i3].pmd);
									a = get_zeroed_page(__GFP_HIGHMEM);
									if(a)
										count++;
									while(!a)
		  							{
										a = get_zeroed_page(__GFP_HIGHMEM);
										if(a)
											count++;
										printk("a is NULL\n");
									}
									pmd_new[i3].pmd = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pmd );
									set_pmd(pmd_new+i2,__pmd( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pmd ));

									pte_old = (pte_t*)__va(pmd_old[i3].pmd & mask); //real vitual address for pte
									pte_new = (pte_t*)a ;

								
								
									for(i4 = 0;i4 < 512;i4++)
									{
										pte_new[i4].pte= pte_old[i4].pte;
										set_pte(pte_new+i4,pte_old[i4]);
									
									}
								}
							}
						}
					}
				}
			}
			
		}
		
	}
	
	//printk_pgtable(pgd_new);

	//printk("\ncount : %d\n",count);
	__flush_tlb_all();
	//return ((unsigned long )pgd_new);
}

void copy_pagetable_half2(unsigned long cp_new_pg)
{
	pgd_t* pgd_new, *pgd_old;
	pud_t* pud_new, *pud_old;
	pmd_t* pmd_new, *pmd_old;
	pte_t* pte_new, *pte_old;

	

	int i1,i2,i3,i4;
	unsigned long a,b;
	unsigned long flag_pgd,flag_pud,flag_pmd;
	unsigned long count = 0;	

	pgd_old = (pgd_t*)__va(read_cr3());
	/*
	a = get_zeroed_page(__GFP_HIGHMEM);
	if(a)
		count++;
	while(!a)
	{
		a = get_zeroed_page(__GFP_HIGHMEM);
		if(a)
			count++;
		printk("a is NULL\n");
	}
	*/
	pgd_new = (pgd_t*)(cp_new_pg);
	
	for(i1 = 0;i1 < 512/2;i1++)
	{
		//pgd_new[i1].pgd = pgd_old[i1].pgd;
		set_pgd(pgd_new+i1,pgd_old[i1]);

		b = is_invalid(pgd_old[i1].pgd);
		if(b)
		{
			//pgd_new[i1].pgd = 0x0;
			continue;
		}				
		else
		{
			flag_pgd = getflag_pgx(pgd_old[i1].pgd);
			a = get_zeroed_page(__GFP_HIGHMEM);
			if(a)
				count++;
			while(!a)
	  		{
				a = get_zeroed_page(__GFP_HIGHMEM);
				if(a)
					count++;
				printk("a is NULL\n");
			}
			//pgd_new[i1].pgd = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pgd );
			set_pgd(pgd_new+i1,__pgd( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pgd ));

			pud_old = (pud_t*)__va(pgd_old[i1].pgd & mask); //real vitual address for pud
			pud_new = (pud_t*)a;

			for(i2 = 0;i2 < 512;i2++)
			{
				//pud_new[i2].pud = pud_old[i2].pud;
				set_pud(pud_new+i2,pud_old[i2]);

				b = is_invalid(pud_old[i2].pud);
				if(b)
				{
					//pud_new[i2].pud = 0x0;
					continue;
				}				
				else
				{
					if(pud_large(pud_old[i2]))
					{
						pud_new[i2].pud = pud_old[i2].pud;
						continue; //1GB page
					}
					else
					{
						flag_pud = getflag_pgx(pud_old[i2].pud);
						a = get_zeroed_page(__GFP_HIGHMEM);
						if(a)
							count++;
						while(!a)
		  				{
							a = get_zeroed_page(__GFP_HIGHMEM);
							if(a)
								count++;
							printk("a is NULL\n");
						}
	
						pud_new[i2].pud = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pud );
						set_pud(pud_new+i2,__pud( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pud ));

						pmd_old = (pmd_t*)__va(pud_old[i2].pud & mask); //real vitual address for pmd
						pmd_new = (pmd_t*)a;

					
					
						for(i3 = 0;i3 < 512;i3++)
						{
							pmd_new[i3].pmd = pmd_old[i3].pmd;
							set_pmd(pmd_new+i3,pmd_old[i3]);

							b = is_invalid(pmd_old[i3].pmd);
							if(b)
							{
								pmd_new[i3].pmd = 0x0;
								continue;
							}				
							else
							{
								if(pmd_large(pmd_old[i3]))
								{
									pmd_new[i3].pmd = pmd_old[i3].pmd;
									continue; //2M page
								}
								else
								{
									flag_pmd = getflag_pgx(pmd_old[i3].pmd);
									a = get_zeroed_page(__GFP_HIGHMEM);
									if(a)
										count++;
									while(!a)
		  							{
										a = get_zeroed_page(__GFP_HIGHMEM);
										if(a)
											count++;
										printk("a is NULL\n");
									}
									pmd_new[i3].pmd = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pmd );
									set_pmd(pmd_new+i2,__pmd( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pmd ));

									pte_old = (pte_t*)__va(pmd_old[i3].pmd & mask); //real vitual address for pte
									pte_new = (pte_t*)a ;

								
								
									for(i4 = 0;i4 < 512;i4++)
									{
										pte_new[i4].pte= pte_old[i4].pte;
										set_pte(pte_new+i4,pte_old[i4]);
									
									}
								}
							}
						}
					}
				}
			}
			
		}
		
	}
	
	//printk_pgtable(pgd_new);

	//printk("\ncount : %d\n",count);
	__flush_tlb_all();
	//return ((unsigned long )pgd_new);
}


void cp_old_pg_half(unsigned long to_cp_addr)//tocpaddr is a addr of new pagetable
{
	//unsigned long newaddr = *to_cp_addr;
	copy_pagetable_half(to_cp_addr);
	
}

void update_pagetable(unsigned long cp_new_pg)
{
	pgd_t* pgd_new, *pgd_old;
	pud_t* pud_new, *pud_old;
	pmd_t* pmd_new, *pmd_old;
	pte_t* pte_new, *pte_old;

	

	int i1,i2,i3,i4;
	unsigned long a,b,c;
	unsigned long flag_pgd,flag_pud,flag_pmd;
	unsigned long count = 0;	

	pgd_old = (pgd_t*)__va(read_cr3());
	/*a = get_zeroed_page(__GFP_HIGHMEM);
	if(a)
		count++;
	while(!a)
	{
		a = get_zeroed_page(__GFP_HIGHMEM);
		if(a)
			count++;
		printk("a is NULL\n");
	}
	*/
	pgd_new = (pgd_t*)(cp_new_pg);
	
	for(i1 = 512/2;i1 < 512;i1++)
	{
		//pgd_new[i1].pgd = pgd_old[i1].pgd;
		set_pgd(pgd_new+i1,pgd_old[i1]);//just prepare for sth

		b = is_invalid(pgd_old[i1].pgd);
		c = is_invalid(pgd_new[i1].pgd);
		if(b || ((!b)&&(!c)) )
		{
			//pgd_new[i1].pgd = 0x0;
			continue;
		}				
		else
		{
			flag_pgd = getflag_pgx(pgd_old[i1].pgd);
			a = get_zeroed_page(__GFP_HIGHMEM);
			if(a)
				count++;
			while(!a)
	  		{
				a = get_zeroed_page(__GFP_HIGHMEM);
				if(a)
					count++;
				printk("a is NULL\n");
			}
			//pgd_new[i1].pgd = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pgd );
			set_pgd(pgd_new+i1,__pgd( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pgd ));

			pud_old = (pud_t*)__va(pgd_old[i1].pgd & mask); //real vitual address for pud
			pud_new = (pud_t*)a;

			for(i2 = 0;i2 < 512;i2++)
			{
				//pud_new[i2].pud = pud_old[i2].pud;
				set_pud(pud_new+i2,pud_old[i2]);

				b = is_invalid(pud_old[i2].pud);
				c = is_invalid(pgd_new[i1].pgd);
				if(b || ((!b)&&(!c)) )
				{
					//pud_new[i2].pud = 0x0;
					continue;
				}				
				else
				{
					if(pud_large(pud_old[i2]))
					{
						pud_new[i2].pud = pud_old[i2].pud;
						continue; //1GB page
					}
					else
					{
						flag_pud = getflag_pgx(pud_old[i2].pud);
						a = get_zeroed_page(__GFP_HIGHMEM);
						if(a)
							count++;
						while(!a)
		  				{
							a = get_zeroed_page(__GFP_HIGHMEM);
							if(a)
								count++;
							printk("a is NULL\n");
						}
	
						pud_new[i2].pud = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pud );
						set_pud(pud_new+i2,__pud( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pud ));

						pmd_old = (pmd_t*)__va(pud_old[i2].pud & mask); //real vitual address for pmd
						pmd_new = (pmd_t*)a;

					
					
						for(i3 = 0;i3 < 512;i3++)
						{
							pmd_new[i3].pmd = pmd_old[i3].pmd;
							set_pmd(pmd_new+i3,pmd_old[i3]);

							b = is_invalid(pmd_old[i3].pmd);
							c = is_invalid(pgd_new[i1].pgd);
							if(b || ((!b)&&(!c)) )
							{
								pmd_new[i3].pmd = 0x0;
								continue;
							}				
							else
							{
								if(pmd_large(pmd_old[i3]))
								{
									pmd_new[i3].pmd = pmd_old[i3].pmd;
									continue; //2M page
								}
								else
								{
									flag_pmd = getflag_pgx(pmd_old[i3].pmd);
									a = get_zeroed_page(__GFP_HIGHMEM);
									if(a)
										count++;
									while(!a)
		  							{
										a = get_zeroed_page(__GFP_HIGHMEM);
										if(a)
											count++;
										printk("a is NULL\n");
									}
									pmd_new[i3].pmd = (unsigned long)( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pmd );
									set_pmd(pmd_new+i2,__pmd( __pa((a >> PAGE_SHIFT) << PAGE_SHIFT) | flag_pmd ));

									pte_old = (pte_t*)__va(pmd_old[i3].pmd & mask); //real vitual address for pte
									pte_new = (pte_t*)a ;

								
								
									for(i4 = 0;i4 < 512;i4++)
									{
										pte_new[i4].pte= pte_old[i4].pte;
										set_pte(pte_new+i4,pte_old[i4]);
									
									}
								}
							}
						}
					}
				}
			}
			
		}
		
	}
	
	//printk_pgtable(pgd_new);

	//printk("\ncount : %d\n",count);
	__flush_tlb_all();
	//return ((unsigned long )pgd_new);
}
EXPORT_SYMBOL(update_pagetable);

void update_pagetable2(unsigned long cp_new_pg)
{
	copy_pagetable_half2(cp_new_pg);
}
EXPORT_SYMBOL(update_pagetable2);
