#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool flg = true;
	if(ref_r->eax != reg_l(R_EAX)){
		printf("pc: 0x%x\tcpu.eax: 0x%x\tref_r.eax: 0x%x\n", cpu.pc, reg_l(R_EAX), ref_r->eax);
		flg = false;
	}
	if (ref_r->ecx != reg_l(R_ECX))
	{
		printf("pc: 0x%x\tcpu.ecx: 0x%x\tref_r.ecx: 0x%x\n", cpu.pc, reg_l(R_ECX), ref_r->ecx);
		flg = false;
	}
	if (ref_r->edx != reg_l(R_EDX))
	{
		printf("pc: 0x%x\tcpu.edx: 0x%x\tref_r.edx: 0x%x\n", cpu.pc, reg_l(R_EDX), ref_r->edx);
		flg = false;
	}
	if (ref_r->ebx != reg_l(R_EBX))
	{
		printf("pc: 0x%x\tcpu.ebx: 0x%x\tref_r.ebx: 0x%x\n", cpu.pc, reg_l(R_EBX), ref_r->ebx);
		flg = false;
	}
	if (ref_r->esp != reg_l(R_ESP))
	{
		printf("pc: 0x%x\tcpu.esp: 0x%x\tref_r.esp: 0x%x\n", cpu.pc, reg_l(R_ESP), ref_r->esp);
		flg = false;
	}
	if (ref_r->ebp != reg_l(R_EBP))
	{
		printf("pc: 0x%x\tcpu.ebp: 0x%x\tref_r.ebp: 0x%x\n", cpu.pc, reg_l(R_EBP), ref_r->ebp);
		flg = false;
	}
	if (ref_r->esi != reg_l(R_ESI))
	{
		printf("pc: 0x%x\tcpu.esi: 0x%x\tref_r.esi: 0x%x\n", cpu.pc, reg_l(R_ESI), ref_r->esi);
		flg = false;
	}
	if (ref_r->edi != reg_l(R_EDI))
	{
		printf("pc: 0x%x\tcpu.edi: 0x%x\tref_r.edi: 0x%x\n", cpu.pc, reg_l(R_EDI), ref_r->edi);
		flg = false;
	}
	if (ref_r->pc != cpu.pc)
	{
		printf("cpu.pc: 0x%x\tref.pc: 0x%x\n", cpu.pc, ref_r->pc);
		flg = false;
	}
	if (ref_r->eflags.OF != cpu.eflags.OF)
	{
		printf("cpu.OF: 0x%x\tref.OF: 0x%x\n", cpu.eflags.OF, ref_r->eflags.OF);
		flg = false;
	}
	if (ref_r->eflags.CF != cpu.eflags.CF)
	{
		printf("cpu.CF: 0x%x\tref.CF: 0x%x\n", cpu.eflags.CF, ref_r->eflags.CF);
		flg = false;
	}
	// if (ref_r->eflags.ZF != cpu.eflags.ZF)
	// {
	// 	printf("cpu.ZF: 0x%x\tref.ZF: 0x%x\n", cpu.eflags.ZF, ref_r->eflags.ZF);
	// 	flg = false;
	// }
	if (ref_r->eflags.SF != cpu.eflags.SF)
	{
		printf("cpu.SF: 0x%x\tref.SF: 0x%x\n", cpu.eflags.SF, ref_r->eflags.SF);
		flg = false;
	}
	return flg;
}

void isa_difftest_attach(void) {
}
