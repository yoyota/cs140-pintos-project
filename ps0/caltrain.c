#include "pintos_thread.h"

struct station {
  struct lock lock;
  struct condition cond;
  int seats;
  int passengers;
};

void station_init(struct station *station) {
  station->seats = 0;
  station->passengers = 0;
  lock_init(&station->lock);
  cond_init(&station->cond);
}

void station_load_train(struct station *station, int count) {
  lock_acquire(&station->lock);
  station->seats = count;
  for (int i = 0; i < count; i += 1) {
    cond_signal(&station->cond, &station->lock);
  }
  while (station->passengers > 0 && station->seats > 0) {
    lock_release(&station->lock);
    lock_acquire(&station->lock);
  }
  lock_release(&station->lock);
}

void station_wait_for_train(struct station *station) {
  lock_acquire(&station->lock);
  station->passengers += 1;
  cond_wait(&station->cond, &station->lock);
  lock_release(&station->lock);
}

void station_on_board(struct station *station) {
  lock_acquire(&station->lock);
  station->seats -= 1;
  station->passengers -= 1;
  lock_release(&station->lock);
}
