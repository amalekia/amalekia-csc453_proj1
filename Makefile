all: schedule two

schedule: schedule.c
		gcc -Wall -Werror -std=c99 -g -o $@ $?
		
two: two.c
		gcc -g -o $@ $?
		
clean:
		-rm -rf schedule