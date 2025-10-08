#include "userprog/syscall.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "userprog/userutil.h"
#include "devices/input.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"

static void syscall_handler(struct intr_frame *);
static void handle_halt(void);
static void handle_write(struct intr_frame *);
static void handle_read(struct intr_frame *);
static void handle_exit(struct intr_frame *);
static void handle_exec(struct intr_frame *);
static void handle_wait(struct intr_frame *);
static void handle_create(struct intr_frame *);
static void handle_remove(struct intr_frame *);
static void handle_open(struct intr_frame *);
static void handle_close(struct intr_frame *);
static int get_next_user_int(void **esp);

static size_t ptr_size = sizeof(void *);
static struct lock filesys_lock;

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
	lock_init(&filesys_lock);
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
	case SYS_CREATE:
		handle_create(f);
		break;
	case SYS_REMOVE:
		handle_remove(f);
		break;
	case SYS_OPEN:
		handle_open(f);
		break;
	case SYS_CLOSE:
		handle_close(f);
		break;
	case SYS_WRITE:
		handle_write(f);
		break;
	case SYS_READ:
		handle_read(f);
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

static void handle_create(struct intr_frame *f)
{
	void *esp = f->esp;
	int filename_addr = get_next_user_int(&esp);
	const char *filename = (const char *)filename_addr;

	char kernel_filename[MAX_FILENAME_LEN];
	if (!copy_string_from_user(filename, kernel_filename,
				   MAX_FILENAME_LEN)) {
		exit(-1);
		return;
	}

	unsigned initial_size = (unsigned)get_next_user_int(&esp);

	lock_acquire(&filesys_lock);
	bool success = filesys_create(kernel_filename, initial_size);
	lock_release(&filesys_lock);

	f->eax = success;
}

static void handle_remove(struct intr_frame *f)
{
	void *esp = f->esp;
	int filename_addr = get_next_user_int(&esp);
	const char *filename = (const char *)filename_addr;

	char kernel_filename[MAX_FILENAME_LEN];
	if (!copy_string_from_user(filename, kernel_filename,
				   MAX_FILENAME_LEN)) {
		exit(-1);
		return;
	}

	lock_acquire(&filesys_lock);
	bool success = filesys_remove(kernel_filename);
	lock_release(&filesys_lock);

	f->eax = success;
}

static void handle_open(struct intr_frame *f)
{
	void *esp = f->esp;
	int filename_addr = get_next_user_int(&esp);
	const char *filename = (const char *)filename_addr;

	char kernel_filename[MAX_FILENAME_LEN];
	if (!copy_string_from_user(filename, kernel_filename,
				   MAX_FILENAME_LEN)) {
		exit(-1);
		return;
	}

	lock_acquire(&filesys_lock);
	struct file *file_opened = filesys_open(kernel_filename);
	lock_release(&filesys_lock);

	if (file_opened == NULL) {
		f->eax = -1;
		return;
	}
	struct thread *cur = thread_current();

	cur->fd_table[cur->next_fd] = file_opened;
	f->eax = cur->next_fd;
	cur->next_fd += 1;
}

static void handle_close(struct intr_frame *f)
{
	void *esp = f->esp;
	int fd = get_next_user_int(&esp);

	if (fd > MAX_OPEN_FILES || fd < 2) {
		return;
	}

	struct thread *cur = thread_current();
	struct file *fd_file = cur->fd_table[fd];
	file_close(fd_file);
	cur->fd_table[fd] = NULL;
}

static void handle_exec(struct intr_frame *f)
{
	void *esp = f->esp;
	int cmdline_addr = get_next_user_int(&esp);
	const char *cmdline = (const char *)cmdline_addr;

	char kernel_cmdline[MAX_CMDLINE_LEN];
	if (!copy_string_from_user(cmdline, kernel_cmdline, MAX_CMDLINE_LEN)) {
		exit(-1);
		return;
	}

	tid_t tid = process_execute(kernel_cmdline);
	f->eax = tid;
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

static void handle_read(struct intr_frame *f)
{
	void *esp = f->esp;
	int fd = get_next_user_int(&esp);
	int buf_addr = get_next_user_int(&esp);
	unsigned size = (unsigned)get_next_user_int(&esp);

	if (fd == STDOUT_FILENO) {
		f->eax = -1;
		return;
	}

	if (fd == STDIN_FILENO) {
		uint8_t *uint8_buf = (uint8_t *)buf_addr;
		unsigned i = 0;
		for (i = 0; i < size; i++) {
			if (!put_user_byte(uint8_buf + i, input_getc())) {
				f->eax = -1;
				return;
			}
		}
		f->eax = i;
		return;
	}
}

static void handle_exit(struct intr_frame *f)
{
	void *esp = f->esp;
	int status_code = get_next_user_int(&esp);
	exit(status_code);
}

void exit(int status_code)
{
	char file_name[PROCESS_NAME_LEN];
	strlcpy(file_name, thread_name(), strcspn(thread_name(), " ") + 1);
	printf("%s: exit(%d)\n", file_name, status_code);
	thread_exit(status_code);
}

static int get_next_user_int(void **esp)
{
	*esp += ptr_size;
	int result = get_user_int((const uint8_t *)*esp);
	if (result == -1) {
		exit(-1);
	}
	return result;
}
