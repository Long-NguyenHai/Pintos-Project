#include "pintos_thread.h"
#include <stdio.h>
struct station
{
	int free_seats;
	int waiting_passengers;
	int onboard_passengers;
	struct lock *khoa;
	struct condition *all_passengers_onboard;
	struct condition *train_arrives;
};
void station_init(struct station *x)
{
	x->free_seats = 0;
	x->waiting_passengers = 0;
	x->onboard_passengers = 0;
	x->khoa = malloc(sizeof(struct lock));
	lock_init(x->khoa);
	x->all_passengers_onboard = malloc(sizeof(struct condition));
	cond_init(x->all_passengers_onboard);
	x->train_arrives = malloc(sizeof(struct condition));
	cond_init(x->train_arrives);
};
void station_load_train(struct station *station, int count)
{
	lock_acquire(station->khoa);
	station->free_seats = count;

	while ((station->waiting_passengers > 0) && (station->free_seats > 0))
	{
		cond_broadcast(station->train_arrives, station->khoa);
		cond_wait(station->all_passengers_onboard, station->khoa);
	}

	station->free_seats = 0;
	lock_release(station->khoa);
}

void station_wait_for_train(struct station *station)
{
	lock_acquire(station->khoa);
	station->waiting_passengers++;
	while (station->onboard_passengers == station->free_seats || station->free_seats == 0)
	{
		cond_wait(station->train_arrives, station->khoa);
	}
	station->waiting_passengers--;
	station->onboard_passengers++;
	lock_release(station->khoa);
}
void station_on_board(struct station *station)
{
	lock_acquire(station->khoa);
	station->onboard_passengers--;
	station->free_seats--;
	if ((station->free_seats == 0) || (station->onboard_passengers == 0))
	{
		cond_signal(station->all_passengers_onboard, station->khoa);
	}

	lock_release(station->khoa);
}
