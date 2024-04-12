### Adrick Malekian

#### Excecuting
When compiling, type make in the command line then execute with "./schedule"
with appropriate arguments.

#### Small Issue
Everything works correctly aside from a commandline with only one function.
Everything in the code reaches the appropriate places except when the 
kill(childpid, SIGCONT) happens and for some reason it doenst continue the function. If I put extra print statemets to debug, the program starts to work for one argument but I am assuming that this isnt an issue with my program but rather just a atomicity thing within the OS and what gets printed in the output. I think by adding more print statements my program is elongated and thus allows the two.c program to print the result.