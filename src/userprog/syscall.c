#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"

static void syscall_handler(struct intr_frame *);
static void handle_halt();
static void handle_write(struct intr_frame *);
static void handle_exit(struct intr_frame *);
static void exit(int);

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
	size_t pointer_size = sizeof(void *);
	esp += pointer_size;
	int fd = *(int *)esp;
	esp += pointer_size;
	char *buf = *(char **)esp;
	esp += pointer_size;
	unsigned size = *(unsigned *)esp;
	if (fd == STDOUT_FILENO) {
		putbuf(buf, size);
	}
}

static void handle_exit(struct intr_frame *f)
{
	void *esp = f->esp;
	esp += sizeof(int *);
	int status_code = *(int *)esp;
	exit(status_code);
}

static void exit(int status_code)
{
	printf("%s: exit(%d)\n", thread_name(), status_code);
	thread_exit();
}
