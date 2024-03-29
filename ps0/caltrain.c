#include "pintos_thread.h"

struct station {
	int seats;
	int count;
	int passengers;
	struct lock lock;
	struct condition arrival;
	struct condition departure;
};

void station_init(struct station *station)
{
	station->seats = 0;
	station->count = 0;
	station->passengers = 0;
	lock_init(&station->lock);
	cond_init(&station->arrival);
	cond_init(&station->departure);
}

void station_load_train(struct station *station, int count)
{
	lock_acquire(&station->lock);
	station->seats = count;
	station->count = count;

	while (station->seats > 0 && station->passengers > 0) {
		cond_broadcast(&station->arrival, &station->lock);
		cond_wait(&station->departure, &station->lock);
	}
	station->seats = 0;
	station->count = 0;
	lock_release(&station->lock);
}

void station_wait_for_train(struct station *station)
{
	lock_acquire(&station->lock);
	station->passengers += 1;

	while (station->count == 0) {
		cond_wait(&station->arrival, &station->lock);
	}
	station->count -= 1;
	lock_release(&station->lock);
}

void station_on_board(struct station *station)
{
	lock_acquire(&station->lock);
	station->seats -= 1;
	station->passengers -= 1;
	if (station->seats == 0 || station->passengers == 0) {
		cond_signal(&station->departure, &station->lock);
	}
	lock_release(&station->lock);
}
