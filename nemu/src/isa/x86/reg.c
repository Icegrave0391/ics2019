#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
  printf("General Purpose Registers:\n");
  int num_regs = sizeof(cpu.gpr) / sizeof(cpu.gpr[0]);
  for(int i = 0; i < num_regs; i++){
    int index = check_reg_index(i);
    printf("\t%-3s = 0x%08x (%d)\n", reg_name(index, 4), reg_l(index), reg_l(index));
  }
  printf("\t pc = 0x%08x (%d)\n", cpu.pc, cpu.pc);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  int width = 0;
  int num_regs = sizeof(cpu.gpr) / sizeof(cpu.gpr[0]);
  if (strlen(s) == 3){  // 32-bit register
    width = 4;
  }
  else if (strlen(s) == 2){
    width = 2;
    if (s[1] == 'H' || s[1] == 'h' || s[1] == 'L' || s[1] == 'l')
    {
      width = 1;
    }
  }

  for(int i = 0; i < num_regs; i++){
    if (strcmp(reg_name(i, width), s) == 0){
      if (width == 4){
        return reg_l(i);
      }
      else if (width == 2){
        return reg_w(i);
      }
      else if (width == 1){
        return reg_b(i);
      }
    }
  }
  return 0;
}
