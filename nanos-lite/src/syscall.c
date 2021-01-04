#include "common.h"
#include "syscall.h"

uint32_t sys_yield();
uint32_t sys_exit();

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
	/* type for syscall */
  a[0] = c->GPR1;
	/* args for syscall */
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;
	uintptr_t res = 0;
  switch (a[0]) {
		case SYS_yield: res = sys_yield();break;
		// case SYS_exit: sys_exit(a[1])
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

	/* set return value in GPR */
	c->GPRx = res;
  return NULL;
}

uint32_t sys_yield(){
	_yield();
	return 0;
}

uint32_t sys_exit(int state){
	_halt(state);
}
