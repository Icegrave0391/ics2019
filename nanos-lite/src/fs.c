#include "fs.h"
/* ramdisk APIs */
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

/* file system APIs */
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

/* special file APIs */
/* serial API */
size_t serial_write(const void *buf, size_t offset, size_t len);
/* event API */
size_t events_read(void *buf, size_t offset, size_t len) ;
/* vga APIs */
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t fbsync_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
	size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
	{"/dev/events", 0, 0, 0, events_read, invalid_write},
	{"/dev/fb", 0, 0, 0, invalid_read, fb_write},
	{"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
	{"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
	int fd = fs_open("/dev/fb", 0, 0);
	Finfo * file = &file_table[fd];
	file->size = sizeof(uint32_t) * screen_height() * screen_width();
}

// int fs_open(const char *pathname, int flags, int mode){
// 	/* Treat the casen that fs_open didn't find the pathname as exception,
// 	 * For simplify, we don't need to care flags & mode
// 	 */

// 	/* try to find the filename */
// 	int fd = -1;
// 	for (int i = 0; i < NR_FILES; i++){
// 		if(!strcmp(pathname, file_table[i].name)){
// 			fd = i;
// 			/* init open offset */
// 			file_table[i].open_offset = 0;
// 			break;
// 		}
// 	}
// 	assert(fd >= 0);
// 	printf("[fs_open] tried to open file %s, fd: %d\n", pathname, fd);
// 	return fd;
// }

// size_t fs_read(int fd, void *buf, size_t len){
// 	assert(fd >= 0 && fd < NR_FILES);
// 	/* operate reading via ramdisk APIs */
// 	Finfo file = file_table[fd];
// 	size_t rlen = -1;
// 	if (file.read){
// 		rlen = file.read(buf, file.open_offset, len);
// 		(&file_table[fd])->open_offset += rlen;
// 	}
// 	else{
// 		/* set the correct read size */
// 		if(file.open_offset > file.size){
// 			printf("[fs_read failed] file %d current open offset %lu> file size %lu.\n", fd, file.open_offset, file.size);
// 			return -1;
// 		}
// 		/* set correct base offset */
// 		size_t base_offset = file.open_offset < 0 ? 0 : file.open_offset;
// 		len = file.open_offset < 0 ? len + file.open_offset : len;
// 		if (len < 0){
// 			return -1;
// 		}
// 		if(base_offset + len > file.size){
// 			len = file.size - base_offset;
// 		}
// 		/* read via ramdisk API */
// 		rlen = ramdisk_read(buf, file.disk_offset + base_offset, len);
// 		/* update open offset */
// 		(&file_table[fd])->open_offset = base_offset + rlen;
// 	}
// 	return rlen;
// }

// size_t fs_write(int fd, const void *buf, size_t len)
// {
// 	assert(fd >= 0 && fd < NR_FILES);
// 	size_t wlen = -1;
// 	Finfo file = file_table[fd];
// 	if(file.write){
// 		wlen = file.write(buf, file.open_offset, len);
// 		(&file_table[fd])->open_offset += wlen;
// 	}
// 	else{
// 		if(file.open_offset > file.size){
// 			printf("[fs_write failed] file %d current open offset %lu> file size %lu.\n", fd, file.open_offset, file.size);
// 			return -1;
// 		}
// 		/* set correct base offset */
// 		size_t base_offset = file.open_offset < 0 ? 0 : file.open_offset;
// 		len = file.open_offset < 0 ? len + file.open_offset : len;
// 		if(len < 0){
// 			return -1;
// 		}
// 		if(base_offset + len > file.size){
// 			len = file.size - base_offset;
// 		}
// 		/* write via ramdisk API */
// 		wlen = ramdisk_write(buf, file.disk_offset + base_offset, len);
// 		/* update open offset */
// 		(&file_table[fd])->open_offset = base_offset + wlen;
// 	}
// 	return wlen;
// }

// size_t fs_lseek(int fd, size_t offset, int whence)
// {
// 	assert(fd < NR_FILES && fd > FD_STDERR);
// 	Finfo *file = &file_table[fd];
// 	size_t ret = -1;
// 	/* lseek() allows the file offset to be set beyond the end of the file */

// 	/* set f_offset to offset bytes */
// 	if(whence == SEEK_SET){
// 		file->open_offset = offset;
// 		ret = offset;
// 	}
// 	/* file offset is set to its current location plus offset bytes */
// 	else if(whence == SEEK_CUR){
// 		file->open_offset += offset;
// 		ret = file->open_offset;
// 	}
// 	/* file offset is set to the size of the file plus offset bytes */
// 	else if(whence == SEEK_END){
// 		file->open_offset = file->size + offset;
// 		ret = file->open_offset;
// 	}
// 	/* should not reach here! */
// 	else{
// 		printf("[fs_lseek] should not reach here.\n");
// 		assert(0);
// 	}
// 	return ret;
// }

// int fs_close(int fd)
// {
// 	return 0;
// }

int fs_open(const char *pathname, int flags, int mode)
{
	int i;
	for (i = 0; i < NR_FILES; ++i)
	{
		if (strcmp(file_table[i].name, pathname) == 0)
		{
			//Log("Open %s success", pathname);
			file_table[i].open_offset = 0;
			return i;
		}
	}
	panic("There is no such pathname: [%s]", pathname);
	return -1;
}
size_t fs_read(int fd, void *buf, size_t len)
{
	assert(fd >= 0 && fd < NR_FILES);

	int r_len = len;
	if (file_table[fd].size > 0 && file_table[fd].open_offset + len > file_table[fd].size)
	{
		r_len = file_table[fd].size - file_table[fd].open_offset;
	}
	assert(r_len >= 0);

	size_t length = 0;
	if (file_table[fd].read == NULL)
	{
		length = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, r_len);
	}
	else
	{
		length = file_table[fd].read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, r_len);
	}

	file_table[fd].open_offset += length;
	return length;
}

int fs_close(int fd)
{
	return 0;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
	assert(fd >= 0 && fd < NR_FILES);
	int w_len = len;
	if (file_table[fd].size > 0 && file_table[fd].open_offset + len > file_table[fd].size)
	{
		w_len = file_table[fd].size - file_table[fd].open_offset;
	}

	assert(w_len >= 0);

	size_t length = 0;
	if (file_table[fd].write == NULL)
	{
		length = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, w_len);
	}
	else
	{
		length = file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, w_len);
	}

	file_table[fd].open_offset += length;
	return length;
}

// maybe change the type of offset to int ?
size_t fs_lseek(int fd, size_t offset, int whence)
{
	assert(fd >= 0 && fd < NR_FILES);
	size_t open_offset = file_table[fd].open_offset;
	switch (whence)
	{
	case SEEK_SET:
		open_offset = offset;
		break;
	case SEEK_CUR:
		open_offset += offset;
		break;
	case SEEK_END:
		open_offset = file_table[fd].size + offset;
		break;
	default:
		panic("There is no such whence");
	}
	file_table[fd].open_offset = open_offset;
	return open_offset;
}
