#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

//GIGABAJT POCETNI ZA MAPIRANJE
#define SHSTART 0x40000000

int
exec(char *path, char **argv)
{
	char *s, *last;
	int i, off;
	uint argc, sz, sp, ustack[3+MAXARG+1];
	struct elfhdr elf;
	struct inode *ip;
	struct proghdr ph;
	pde_t *pgdir, *oldpgdir;
	struct proc *curproc = myproc();

	begin_op();

	if((ip = namei(path)) == 0){
		end_op();
		cprintf("exec: fail\n");
		return -1;
	}
	ilock(ip);
	pgdir = 0;

	// Check ELF header
	if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
		goto bad;
	if(elf.magic != ELF_MAGIC)
		goto bad;

	if((pgdir = setupkvm()) == 0)
		goto bad;

	// Load program into memory.
	sz = 0;
	for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
		if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
			goto bad;
		if(ph.type != ELF_PROG_LOAD)
			continue;
		if(ph.memsz < ph.filesz)
			goto bad;
		if(ph.vaddr + ph.memsz < ph.vaddr)
			goto bad;
		if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
			goto bad;
		if(ph.vaddr % PGSIZE != 0)
			goto bad;
		if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
			goto bad;
	}
	iunlockput(ip);
	end_op();
	ip = 0;

	// Allocate two pages at the next page boundary.
	// Make the first inaccessible.  Use the second as the user stack.
	sz = PGROUNDUP(sz);
	if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
		goto bad;
	clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
	sp = sz;

	// Push argument strings, prepare rest of stack in ustack.
	for(argc = 0; argv[argc]; argc++) {
		if(argc >= MAXARG)
			goto bad;
		sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
		if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
			goto bad;
		ustack[3+argc] = sp;
	}
	ustack[3+argc] = 0;

	ustack[0] = 0xffffffff;  // fake return PC
	ustack[1] = argc;
	ustack[2] = sp - (argc+1)*4;  // argv pointer

	sp -= (3+argc+1) * 4;
	if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
		goto bad;

	// Save program name for debugging.
	for(last=s=path; *s; s++)
		if(*s == '/')
			last = s+1;
	safestrcpy(curproc->name, last, sizeof(curproc->name));

	// Commit to the user image.
	oldpgdir = curproc->pgdir;
	curproc->pgdir = pgdir;
	curproc->sz = sz;
	curproc->tf->eip = elf.entry;  // main
	curproc->tf->esp = sp;
	switchuvm(curproc);

	// OD 1GB ALOCIRAMO SHARED STRUKTURE
	int j;
	uint VIRTA = PGROUNDUP(SHSTART);

	for (j = 0; j < SHNUM; j++)
	{
		if (curproc->shares[j].size == 0)break;
		//cprintf("%p" ,VIRTA);
		//uint PA = walkpgdirLEGIT(curproc->PARENTpgdir, curproc->shares[j].addr, 0);
		//PA += PTE_FLAGS(curproc->shares[j].addr);
		//cprintf("%s %p %d\n", curproc->shares[j].name, curproc->shares[j].addr,curproc->shares[j].size);
		pte_t* PA = walkpgdirLEGIT(curproc->PARENTpgdir, curproc->shares[j].addr, 0);
		//cprintf("%p\n", PTE_ADDR(*PA)+PTE_FLAGS(curproc->shares[j].addr));

		//(*PA) = (*PA) | PTE_P;

		uint off = PTE_FLAGS(curproc->shares[j].addr);
		//cprintf("%d\n", off);
	
		//cprintf("%d", j);
		//cprintf("EXEC %p\n",(PTE_ADDR(*PA))+PTE_FLAGS(curproc->shares[j].addr));
		mappagesLEGIT(curproc->pgdir, (char*)VIRTA, curproc->shares[j].size, PTE_ADDR(*PA));
		//cprintf("%p\n",PTE_ADDR(*PA) + off);
		curproc->shares[j].addr = (char*)(VIRTA + off);
		//cprintf("curproc->shares[j].addr = %p\n", curproc->shares[j].addr);
		//uint check = walkpgdirLEGIT(curproc->pgdir, curproc->shares[j].addr, 0);
		//cprintf("VA: %p\n", curproc->shares[j].addr);
		//cprintf("PA: %p\n", PGROUNDDOWN(PTE_ADDR(*walkpgdirLEGIT(curproc->pgdir, curproc->shares[j].addr, 0))) + PTE_FLAGS(curproc->shares[j].addr));
		VIRTA+=PGSIZE;
	}

	//if (curproc->pgdir == curproc->PARENTpgdir)
		freevm(oldpgdir);
	//else
	//	freevmLEGIT(oldpgdir);
	return 0;

	bad:
	if(pgdir)
		freevm(pgdir);
	if(ip){
		iunlockput(ip);
		end_op();
	}
	return -1;
}
