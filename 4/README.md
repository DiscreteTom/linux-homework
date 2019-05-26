# 作业4

## 题目

使用`fork()`, `exec()`, `dup2()`, `pipe()`, `open()`系统调用完成与下列shell命令等价的功能。

`grep -v usr</etc/passwd|wc -l>result.txt`

## 解

1. 使用`pipe()`初始化管道文件描述符`pfd[2]`
2. 使用`fork()`生成两个子进程，父进程使用两个`wait()`等待子进程防止孤儿进程的产生
3. 子进程A执行`grep -v usr</etc/passwd`。使用`dup2()`修改文件描述符表内容，使文件`/etc/passwd`作为标准输入，管道入口`pfd[1]`作为标准输出，然后通过`execlp`调用`grep -v usr`执行
4. 子进程B执行`wc -l>result.txt`。使用`dup2()`修改文件描述符表的内容，使文件`result.txt`作为标准输出，管道出口`pfd[0]`作为标准输入，然后通过`execlp`调用`wc -l`执行

## 源代码

```cpp
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
```