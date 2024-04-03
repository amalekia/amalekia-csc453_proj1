all: schedule

schedule: schedule.c
		gcc -Wall -Werror -g -o $@ $?
		
clean:
		-rm -rf schedule