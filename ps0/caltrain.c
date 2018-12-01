#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "pintos_thread.h"

struct station
{
	int passengers_wait_for_train;
	int available_seats;
	int passengers_can_board;
	struct lock lock;
	struct condition condition;
};

void station_init(struct station *station)
{
	station->passengers_wait_for_train = 0;
	station->available_seats = 0;
	station->passengers_can_board = 0;
	lock_init(&station->lock);
	cond_init(&station->condition);
}

void station_load_train(struct station *station, int count)
{
	lock_acquire(&station->lock);
	if (count == 0 || station->passengers_wait_for_train == 0)
	{
		lock_release(&station->lock);
		return;
	}
	station->available_seats = count;
	station->passengers_can_board = count;
	cond_broadcast(&station->condition, &station->lock);
	cond_wait(&station->condition, &station->lock);
	lock_release(&station->lock);
}

void station_wait_for_train(struct station *station)
{
	lock_acquire(&station->lock);
	station->passengers_wait_for_train += 1;
	while (station->available_seats == 0)
	{
		cond_wait(&station->condition, &station->lock);
		lock_release(&station->lock);
		lock_acquire(&station->lock);
	}
	station->available_seats -= 1;
	lock_release(&station->lock);
}

void station_on_board(struct station *station)
{
	lock_acquire(&station->lock);
	station->passengers_can_board -= 1;
	station->passengers_wait_for_train -= 1;
	if (station->passengers_can_board == 0 ||
		station->passengers_wait_for_train == 0)
	{
		cond_signal(&station->condition, &station->lock);
	}
	lock_release(&station->lock);
}
