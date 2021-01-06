#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  if(!s) return 0;
	size_t len = 0;
	while((*s++) != '\0'){
		len += 1;
	}
	return len;
}

char *strcpy(char* dst,const char* src) {
	/*
	 * The stpcpy() and strcpy() functions copy the string src to dst (including
   * the terminating `\0' character.)
	 */
	if(!dst || !src) return dst;
	char *cpy = dst;
	while((*cpy++ = *src++) != '\0'){
		/* just copy and iterate */
	}
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  if(dst == NULL || src == NULL) return dst;
	char *cpy = dst;
	size_t i = 0;
	while(i < n && *src != '\0'){
		*cpy++ = *src++;
		i += 1;
	}
	while(i++ < n){
		*cpy++ = '\0';
	}
	return dst;
}

char* strcat(char* dst, const char* src) {
  if(dst == NULL || src == NULL) return dst;
	char *cpy = dst;
	while(*cpy != '\0'){ /* iterate to the end of dst */
		cpy += 1;
	}
	while((*cpy++ = *src++) != '\0'){
		/* just copy and iterate */
	}
	return dst;
}

int strcmp(const char* s1, const char* s2) {
  assert(s1 != NULL && s2 != NULL);
	while(*s1 != '\0'){
		if(*s1 == *s2){
			s1 += 1;
			s2 += 1;
		}
		else break;
	}
	int ret = *(unsigned char *)s1 - *(unsigned char *)s2;
	if(ret > 0) ret = 1;
	else if(ret < 0) ret = -1;
	return ret;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert(s1 != NULL && s2 != NULL);
	int ret = 0;
	while(n--){
		ret = *s1 - *s2;
		if(ret || *s1 != '\0') break;
		s1 += 1;
		s2 += 1;
	}
	if(ret > 0) ret = 1;
	else if(ret < 0) ret = -1;
	return ret;
}

void* memset(void* v,int c,size_t n) {
  if(v == NULL) return NULL;
	/*
	 * The memset() function writes len bytes of value c (converted to an
   * unsigned char) to the string b.
	 */
	char * cpy = (char *)v;
	while(n--){
		*cpy++ = c;
	}
	return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *src = (char *)in;
	char *dst = (char *)out;
	assert(dst <= src || dst >= src + n);
	while(n--){
		*dst++ = *src++;
	}
	return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert(s1 != NULL && s2 != NULL);
	unsigned char *tmp1 = (unsigned char *)s1;
	unsigned char *tmp2 = (unsigned char *)s2;
	int ret = 0;
	while(n--){
		ret = *tmp1 - *tmp2;
		if(ret) break;
		tmp1 += 1;
		tmp2 += 1;
	}
	if(ret > 0) ret = 1;
	else if(ret < 0) ret = -1;
	return ret;
}

#endif
