#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
  int h_atom_count;
  struct condition h2o_reaction;
  struct condition h_atom_creation;
  struct lock lock;
};

void reaction_init(struct reaction *reaction) {
  reaction->h_atom_count = 0;
  cond_init(&reaction->h2o_reaction);
  cond_init(&reaction->h_atom_creation);
  lock_init(&reaction->lock);
}

void reaction_h(struct reaction *reaction) {
  lock_acquire(&reaction->lock);
  reaction->h_atom_count += 1;
  cond_signal(&reaction->h_atom_creation, &reaction->lock);
  cond_wait(&reaction->h2o_reaction, &reaction->lock);
  lock_release(&reaction->lock);
}

void reaction_o(struct reaction *reaction) {
  lock_acquire(&reaction->lock);
  while (reaction->h_atom_count < 2) {
    cond_wait(&reaction->h_atom_creation, &reaction->lock);
  }
  reaction->h_atom_count -= 2;
  make_water();
  cond_signal(&reaction->h2o_reaction, &reaction->lock);
  cond_signal(&reaction->h2o_reaction, &reaction->lock);
  lock_release(&reaction->lock);
}
