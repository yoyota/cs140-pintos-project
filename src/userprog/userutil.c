#include "userprog/userutil.h"
#include "threads/vaddr.h"

int get_user_byte(const uint8_t *uaddr)
{
	if (!is_user_vaddr(uaddr)) {
		return -1;
	}
	int result;
	asm("movl $1f, %0; movzbl %1, %0; 1:" : "=&a"(result) : "m"(*uaddr));
	return result;
}

int get_user_int(const uint8_t *uaddr)
{
	if (!is_user_vaddr(uaddr)) {
		return -1;
	}
	int result;
	asm("movl $1f, %0; movl %1, %0; 1:" : "=&a"(result) : "m"(*uaddr));
	return result;
}

bool copy_string_from_user(const char *src, char *dst, size_t max_len)
{
	for (size_t i = 0; i < max_len - 1; i++) {
		int byte = get_user_byte((const uint8_t *)(src + i));
		if (byte == -1) {
			return false;
		}

		char c = (char)byte;
		dst[i] = c;

		if (c == '\0') {
			return true;
		}
	}
	dst[max_len - 1] = '\0';
	return true;
}

bool put_user_byte(uint8_t *udst, uint8_t byte)
{
	if (!is_user_vaddr(udst)) {
		return false;
	}
	int error_code;
	asm("movl $1f, %0; movb %b2, %1; 1:"
	    : "=&a"(error_code), "=m"(*udst)
	    : "q"(byte));
	return error_code != -1;
}