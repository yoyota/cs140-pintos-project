#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"

static void syscall_handler(struct intr_frame *);
static void handle_halt(void);
static void handle_write(struct intr_frame *);
static void handle_exit(struct intr_frame *);
static void exit(int);

static size_t ptr_size = sizeof(void *);

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void syscall_handler(struct intr_frame *f)
{
	void *esp = f->esp;
	int sys_call_number = *(uint8_t *)esp;
	switch (sys_call_number) {
	case SYS_HALT:
		handle_halt();
		break;
	case SYS_WRITE:
		handle_write(f);
		break;
	case SYS_EXIT:
		handle_exit(f);
		break;
	default:
		exit(-1);
	}
}
static void handle_halt()
{
	shutdown_power_off();
}

static void handle_write(struct intr_frame *f)
{
	void *esp = f->esp;
	esp += ptr_size;
	int fd = *(int *)esp;
	esp += ptr_size;
	char *buf = *(char **)esp;
	esp += ptr_size;
	unsigned size = *(unsigned *)esp;
	if (fd == STDOUT_FILENO) {
		putbuf(buf, size);
	}
}

static void handle_exit(struct intr_frame *f)
{
	void *esp = f->esp;
	esp += ptr_size;
	int status_code = *(int *)esp;
	exit(status_code);
}

static void exit(int status_code)
{
	char file_name[16];
	strlcpy(file_name, thread_name(), strcspn(thread_name(), " ") + 1);
	printf("%s: exit(%d)\n", file_name, status_code);
	thread_exit(status_code);
}
