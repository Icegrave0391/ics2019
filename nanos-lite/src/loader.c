#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

/* loader: load elf-format file
 * Seek for information: https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
 */
static uintptr_t loader(PCB *pcb, const char *filename)
{
	// TODO();

	/* PA3.3 ignore params, only loading file in ramdisk */
	Elf_Ehdr elf_ehdr;
	/* read elf-header from ramdisk */
	ramdisk_read((void *)&elf_ehdr, 0, sizeof(Elf_Ehdr));
	/* check format */
	if(elf_ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
		 elf_ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
		 elf_ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
		 elf_ehdr.e_ident[EI_MAG3] != ELFMAG3 ){
			 Log("ELF format error in ramdisk\n");
			 assert(0);
		 }
	/* read elf-program header from ramdisk */
	/* The program header table tells the system how to create a process image.
	 * It is found at file offset e_phoff, and consists of e_phnum entries, each with size e_phentsize.
	 */
	Elf_Phdr elf_phdr;
	/* find load-type program headers */
	for(uint16_t i = 0; i < elf_ehdr.e_phnum; i++){
		ramdisk_read((void *)&elf_phdr, elf_ehdr.e_phoff + i * elf_ehdr.e_phentsize, sizeof(Elf_Phdr));
		Log("segment p_offset from phdr: 0x%x, from ehdr: 0x%x\n", elf_phdr.p_offset, elf_ehdr.e_phoff + i * elf_ehdr.e_phentsize);
		Log("current type: %d\n", elf_phdr.p_type);
		if(elf_phdr.p_type != PT_LOAD){
			continue;
		}
		/* load to memory */
		ramdisk_read((void *)elf_phdr.p_vaddr, elf_phdr.p_offset, elf_phdr.p_filesz);
		if(elf_phdr.p_memsz > elf_phdr.p_filesz){
			/* set 0 */
			memset((void *)elf_phdr.p_vaddr + elf_phdr.p_filesz, 0, elf_phdr.p_memsz - elf_phdr.p_filesz);
		}
	}
	return elf_ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
