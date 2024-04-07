# Multithreading

## Building

Command line:

* navigate into the directory 1_project_multithreading
* `make` to build. An executable called `hellothread` will be produced.

## Files

* `hellothread.c`: The main source code

## Data

* `threads` is an array of thread identifiers for each thread created.
* `thread_names` is an array of character array pointers storing thread names.

## Functions

* `main()` Main funtion.
* `identify()` Thread function prints a sequence of numbers using the provided thread name.
* `pthread_create()` Creates the threads. Is passed thread identifier and thread_name.
* `pthread_join()` Stops execution of the main function untill the threads end.

## Notes
* Race condition in code allows for different output each time the program is run.
