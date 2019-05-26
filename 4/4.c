#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
	int pfd[2]; // pipe file descriptor
	if (pipe(pfd) == -1)
	{
		fprintf(stderr, "create pipe error\n");
		exit(1);
	}
	if (fork() == 0)
	{
		// first child, run `grep -v usr</etc/passwd`
		dup2(pfd[1], 1); // stdout => pipe
		close(pfd[0]);
		close(pfd[1]);
		int fd = open("/etc/passwd", O_RDONLY);
		if (fd == -1)
		{
			fprintf(stderr, "open /etc/passwd error\n");
			exit(1);
		}
		dup2(fd, 0); // file => stdin
		close(fd);
		execlp("grep", "grep", "-v", "usr", NULL);
	}
	else if (fork() == 0)
	{
		// second child, run `wc -l>result.txt`
		dup2(pfd[0], 0); // pipe => stdin
		close(pfd[0]);
		close(pfd[1]);
		int fd = open("result.txt", O_WRONLY | O_CREAT);
		if (fd == -1)
		{
			fprintf(stderr, "open result.txt error\n");
			exit(1);
		}
		dup2(fd, 1); // file to stdout
		close(fd);
		execlp("wc", "wc", "-l", NULL);
	}
	close(pfd[0]);
	close(pfd[1]);
	int state;
	wait(&state);
	wait(&state);
}