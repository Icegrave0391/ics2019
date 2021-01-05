#include "common.h"
#include "syscall.h"

uint32_t sys_yield();
uint32_t sys_exit(int state);
uint32_t sys_write(int fd, const void *buf, size_t count);
uint32_t sys_brk(void *addr);
uint32_t sys_open(const char *pathname, int flags);

/* file system operations */
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

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
		case SYS_open: res = sys_open((const char *)a[1], a[2]);break;
		case SYS_write: res = sys_write(a[1], (void *)a[2], a[3]); break;
		case SYS_brk: res = sys_brk((void *)a[1]); break;
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
	// Log("called write!!\n");
	if(fd == 1 || fd == 2){
		size_t i;
		for(i = 0; i < count; i++){
			_putc(((char *)buf)[i]);
		}
	}
	return count;
}

uint32_t sys_brk(void *addr){
	/* should do nothing!!
	 * the real funtion should be implemented at nanos.c _sbrk
	 * it seems strange to implement sys_brk in _sbrk
	 * because we must use a static variable to record the change of _end
	 */

	/* semantic: _end <- addr */
	// _end <- addr;

	/* always successful for pa3.3 */
	return 0;
}

uint32_t sys_open(const char *pathname, int flags){
	/* return the file descriptor */
	return fs_open(pathname, 0, 0);
}
