#ifndef USERPROG_USERUTIL_H
#define USERPROG_USERUTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Constants for buffer sizes */
#define MAX_FILENAME_LEN 256
#define MAX_CMDLINE_LEN 256
#define PROCESS_NAME_LEN 16

/* User memory access functions */
int get_user_byte(const uint8_t *uaddr);
int get_user_int(const uint8_t *uaddr);
bool copy_string_from_user(const char *src, char *dst, size_t max_len);

#endif /* userprog/userutil.h */