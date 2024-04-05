all: schedule

schedule: schedule.c
		gcc -Wall -Werror -std=c99 -g -o $@ $?
		
clean:
		-rm -rf schedule