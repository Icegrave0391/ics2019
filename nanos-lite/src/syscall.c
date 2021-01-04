#include "common.h"
#include "syscall.h"

uint32_t sys_yield();
uint32_t sys_exit(int state);
uint32_t sys_write(int fd, const void *buf, size_t count);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
	/* type for syscall */
  a[0] = c->GPR1;
	/* args for syscall */
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;
	uintptr_t res = 0;
  switch (a[0]) {  /* syscall.type (called by user at _syscall_(type, args)) */
		case SYS_yield: res = sys_yield(); break;
		case SYS_exit: res = sys_exit(a[1]); break;
		case SYS_write: res = sys_write(a[1], (void *)a[2], a[3]); break;
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
uint32_t sys_write(int fd, const void *buf, size_t count){
	/* only write to stdout or stderr,
	 * for didn't implement file system yet.
	 */
	Log("called write!!\n");
	if(fd == 1 || fd == 2){
		size_t i;
		for(i = 0; i < count; i++){
			_putc(((char *)buf)[i]);
		}
	}
	return count;
}
