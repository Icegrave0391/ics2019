#ifndef __X86_REG_H__
#define __X86_REG_H__

#include "common.h"

#define PC_START IMAGE_START

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  /* Do NOT change the order of the GPRs' definitions. */

  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
   * in PA2 able to directly access these registers.
   */

  /* PA1: RTFSC */
	/* general registers */
  union {
    union {
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    }gpr[8];
    struct {
      rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    };
  };
	/* pc(eip) */
  vaddr_t pc;
	/* flags registers */
	/*
	                                              16-BIT FLAGS REGISTER
                                                         A
                                     +-------------------+---------------+
  31                  23                  15               7            0
 +-------------------+---------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                                   |V|R| |N| IO|O|D|I|T|S|Z| |A| |P| |C|
 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | | |0| |   | | | | | | |0| |0| |1| |
 |                                   |M|F| |T| PL|F|F|F|F|F|F| |F| |F| |F|
 +-------------------+---------------+++++-+++-+-+++++++++++++-+++-+++-+++
                                      | |   |  |  | | | | | |   |   |   |
       VIRTUAL 8086 MODE---X----------+ |   |  |  | | | | | |   |   |   |
             RESUME FLAG---X------------+   |  |  | | | | | |   |   |   |
        NESTED TASK FLAG---X----------------+  |  | | | | | |   |   |   |
     I/O PRIVILEGE LEVEL---X-------------------+  | | | | | |   |   |   |
                OVERFLOW---S----------------------+ | | | | |   |   |   |
          DIRECTION FLAG---C------------------------+ | | | |   |   |   |
        INTERRUPT ENABLE---X--------------------------+ | | |   |   |   |
               TRAP FLAG---S----------------------------+ | |   |   |   |
               SIGN FLAG---S------------------------------+ |   |   |   |
               ZERO FLAG---S--------------------------------+   |   |   |
         AUXILIARY CARRY---S------------------------------------+   |   |
             PARITY FLAG---S----------------------------------------+   |
              CARRY FLAG---S--------------------------------------------+

          S = STATUS FLAG, C = CONTROL FLAG, X = SYSTEM FLAG

          NOTE: 0 OR 1 INDICATES INTEL RESERVED. DO NOT DEFINE
	*/
	union{
		struct{
			uint32_t CF: 1;
			uint32_t: 1;
			uint32_t PF: 1;
			uint32_t: 1;
			uint32_t AF: 1;
			uint32_t: 1;
			uint32_t ZF: 1;
			uint32_t SF: 1;
			uint32_t TF: 1;
			uint32_t IF: 1;
			uint32_t DF: 1;
			uint32_t OF: 1;
			uint32_t IOPL: 2;
			uint32_t NT: 1;
			uint32_t: 1;
			uint32_t RF: 1;
			uint32_t VM: 1;
		};
		rtlreg_t eflags_32;
	} eflags;
} CPU_state;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

#define reg_ef(f) (cpu.eflags.f)

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  extern const char* regsw[];
  extern const char* regsb[];
  assert(index >= 0 && index < 8);
  switch (width) {
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

#endif
