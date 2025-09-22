#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/pagedir.h"

static void syscall_handler(struct intr_frame *);
static void handle_halt(void);
static void handle_write(struct intr_frame *);
static void handle_exit(struct intr_frame *);
static void handle_exec(struct intr_frame *);
static void handle_wait(struct intr_frame *);
static void exit(int);
static int get_user_int(const uint8_t *uaddr);
static int get_user_byte(const uint8_t *uaddr);
static int get_next_user_int(void **esp);

static size_t ptr_size = sizeof(void *);

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void syscall_handler(struct intr_frame *f)
{
	void *esp = f->esp;
	int sys_call_number = get_user_byte(esp);
	if (sys_call_number == -1) {
		exit(-1);
		return;
	}

	switch (sys_call_number) {
	case SYS_HALT:
		handle_halt();
		break;
	case SYS_WRITE:
		handle_write(f);
		break;
	case SYS_WAIT:
		handle_wait(f);
		break;
	case SYS_EXEC:
		handle_exec(f);
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

static void handle_exec(struct intr_frame *f)
{
	void *esp = f->esp;
	int cmdline_addr = get_next_user_int(&esp);
	const char *cmdline = (const char *)cmdline_addr;

	int i;
	char kernel_cmdline[256];
	for (i = 0; i < 255; i++) {
		int copied = get_user_byte((const uint8_t *)(cmdline + i));
		if (copied == -1) {
			exit(-1);
			return;
		}
		char b = (char)copied;
		if (b == '\0') {
			kernel_cmdline[i] = b;
			break;
		}
		kernel_cmdline[i] = b;
	}
	kernel_cmdline[i] = '\0';
	f->eax = process_execute(kernel_cmdline);
}

static void handle_wait(struct intr_frame *f)
{
	void *esp = f->esp;
	pid_t pid = get_next_user_int(&esp);
	f->eax = process_wait(pid);
}

static void handle_write(struct intr_frame *f)
{
	void *esp = f->esp;
	int fd = get_next_user_int(&esp);
	int buf_addr = get_next_user_int(&esp);
	char *buf = (char *)buf_addr;
	unsigned size = (unsigned)get_next_user_int(&esp);

	if (fd == STDOUT_FILENO) {
		putbuf(buf, size);
	}
}

static void handle_exit(struct intr_frame *f)
{
	void *esp = f->esp;
	int status_code = get_next_user_int(&esp);
	exit(status_code);
}

static void exit(int status_code)
{
	char file_name[16];
	strlcpy(file_name, thread_name(), strcspn(thread_name(), " ") + 1);
	printf("%s: exit(%d)\n", file_name, status_code);
	thread_exit(status_code);
}

static int get_next_user_int(void **esp)
{
	*esp += ptr_size;
	int result = get_user_int(*esp);
	if (result == -1) {
		exit(-1);
	}
	return result;
}

static int get_user_int(const uint8_t *uaddr)
{
	if (!is_user_vaddr(uaddr)) {
		return -1;
	}
	int result;
	asm("movl $1f, %0; movl %1, %0; 1:" : "=&a"(result) : "m"(*uaddr));
	return result;
}

static int get_user_byte(const uint8_t *uaddr)
{
	if (!is_user_vaddr(uaddr)) {
		return -1;
	}
	int result;
	asm("movl $1f, %0; movzbl %1, %0; 1:" : "=&a"(result) : "m"(*uaddr));

	return result;
}