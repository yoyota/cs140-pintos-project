#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
	int h_count;
	int h_reacted;
	struct lock lock;
	struct condition cond;
};

void reaction_init(struct reaction *reaction)
{
	reaction->h_count = 0;
	reaction->h_reacted = 0;
	lock_init(&reaction->lock);
	cond_init(&reaction->cond);
}

void reaction_h(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);
	reaction->h_count += 1;
	cond_broadcast(&reaction->cond, &reaction->lock);

	while (reaction->h_reacted == 0) {
		cond_wait(&reaction->cond, &reaction->lock);
	}
	reaction->h_reacted -= 1;
	lock_release(&reaction->lock);
}

void reaction_o(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);
	while (reaction->h_count < 2) {
		cond_wait(&reaction->cond, &reaction->lock);
	}
	reaction->h_count -= 2;
	reaction->h_reacted += 2;
	cond_broadcast(&reaction->cond, &reaction->lock);
	lock_release(&reaction->lock);
	make_water();
}
