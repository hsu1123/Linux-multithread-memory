#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <linux/syscalls.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE2(vir_to_physical_addresses, unsigned long*, initial, unsigned long*, result){
	//struct mm_struct *mm = current->mm;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	unsigned long phys_addr = 0;
    unsigned long page_addr = 0;
    unsigned long page_offset = 0;

	//down_read(&mm->mmap_sem);

    unsigned long *vir_adds = kmalloc(1 * sizeof(unsigned long), GFP_KERNEL);
    unsigned long *phy_adds = kmalloc(1 * sizeof(unsigned long), GFP_KERNEL);

    unsigned long a = copy_from_user(vir_adds, initial, 1 * sizeof(unsigned long));
    printk("%lu", a);

    int i = 0; //input address : *(initial + i)

	pgd = pgd_offset(current->mm, *(vir_adds + i));
	printk("pgd_val = 0x%lx\n", pgd_val(*pgd));
   	printk("pgd_index = 0x%lx\n", pgd_index(*(vir_adds + i)));
	if(pgd_none(*pgd)){
		printk("not map in pgdn");
	}

	p4d = p4d_offset(pgd, *(vir_adds + i));
    printk("p4d_val = 0x%lx\n", p4d_val(*p4d));
   	printk("p4d_index = 0x%lx\n", p4d_index(*(vir_adds + i)));
    if(p4d_none(*p4d)){
        printk("not map in p4d");

    }

	pud = pud_offset(p4d, *(vir_adds + i));
    printk("pud_val = 0x%lx\n", pud_val(*pud));
   	printk("pud_index = 0x%lx\n", pud_index(*(vir_adds + i)));
    if(pud_none(*pud)){
        printk("not map in pudn");

    }

    pmd = pmd_offset(pud, *(initial + i));
    printk("pmd_val = 0x%lx\n", pmd_val(*pmd));
   	printk("pmd_index = 0x%lx\n", pmd_index(*(initial + i)));
    if(pmd_none(*pmd)){
        printk("not map in pmdn");

    }

    pte = pte_offset_kernel(pmd, *(initial + i));
    printk("pte_val = 0x%lx\n", pte_val(*pte));
   	printk("pte_index = 0x%lx\n", pte_index(*(initial + i)));
    if(pte_none(*pte)){
        printk("not map in pten");

    }
	//phys_addr = (unsigned long)page_to_phys(pte_page(*pte)) + (vaddr & ~PAGE_MASK);
	//phys_addr = (pte_val(*pte) & PAGE_MASK) | (vaddr & ~PAGE_MASK);

    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = *(initial + i) & ~PAGE_MASK;

    *(phy_adds + i) = page_addr | page_offset;

    printk("page_address = %lx\n",  page_addr);
    printk("page_offset = %lx\n",  page_offset);

    printk("vaddr = %lx\n, paddr = %lx\n", *(vir_adds + i), *(phy_adds + i));

    copy_to_user(result, phy_adds, 1 * sizeof(unsigned long));
    kfree(vir_adds);
    kfree(phy_adds);
	/*done:
		if(pte)
			pte_unmap(pte);
		//up_read(&mm->mmap_sem);

		return phys_addr;*/
    return 0;
}