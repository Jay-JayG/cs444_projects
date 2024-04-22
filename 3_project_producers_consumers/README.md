# Producers and Consumers

## Building

Command line:

* `make` to build. An executable called `pc` will be produced.

## Files

* `pc.c`: The main code
* `eventbuf.c`: Code implementing the event buffer and associated commands
* `eventbuf.h`: header file for eventbuf.c

## Data

- producer_event_buff and consumer_event_buff semaphores.
- global_num_events determines how many events each producer thread will make.
- global_eventbuf initializes the event buffer data structure.
- producers_done a flag variable used in the consumer_run function to make consumers exit.

## Functions

* 
* `main()`
	* `sem_open_temp()`: Helper function for initializing semaphores
	* `producer_run()`: Adds events to the event buffer.
	* `consumer_run()`: Removes events from the event buffer
