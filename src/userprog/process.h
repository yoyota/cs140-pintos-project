#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

struct child_info {
	tid_t tid;
	bool is_done;
	int exit_code;
	struct semaphore sema_exit;
	struct list_elem children_elem;
};

tid_t process_execute(const char *file_name);
int process_wait(tid_t);
void process_exit(int status);
void process_activate(void);

#endif /* userprog/process.h */
