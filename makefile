deafult: Shell
Shell: Shell.o 
	gcc -o Shell Shell.o
Shell.o: Shell.c
	gcc -c Shell.c
