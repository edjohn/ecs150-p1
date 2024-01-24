sshell: sshell.c
	gcc sshell.c -o sshell -Wall -Wextra -Werror 

clean:
	rm *.o