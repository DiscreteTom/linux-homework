#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

static const char *optString = "ral:h:m:";
static const int MAX_PATH_LENGTH = 255;

struct
{
	int r; // 0 for false, 1 for true
	int a; // 0 for false, 1 for true
	int l;
	int h;
	int m;
} config;

time_t now;
struct stat buf;

void getConfig(int argc, char **argv);
void printItem(char *path);
void printDir(char *path);
int judgeMLH();

int main(int argc, char **argv)
{
	int i;
	time(&now);

	getConfig(argc, argv);

	if (optind >= argc) // print current dir
		printItem(".");
	else
	{
		if (!strcmp(argv[optind], "--"))
			optind++; // ignore the first '--'
		for (i = optind; i < argc; ++i)
			printItem(argv[i]);
	}

	return 0;
}

void printItem(char *path)
{
	if (stat(path, &buf))
	{
		printf("Failed to get stat of %s: %m\n", path);
		return;
	}

	if (S_ISDIR(buf.st_mode))
	{
		printf("%s:\n", path);
		printDir(path);
	}
	else if (judgeMLH())
		// ignore config.a and config.r
		printf("%s\n", path);
}

int judgeMLH()
{
	if ((config.m <= 0 || now - buf.st_ctime < config.m * 60 * 60 * 24) && (config.l <= 0 || buf.st_size >= config.l) && (config.h <= 0 || buf.st_size <= config.h))
		return 1;
	else
		return 0;
}

void printDir(char *path)
{
	DIR *dir = opendir(path);
	if (!dir)
	{
		printf("Failed open dir %s: %m", path);
		return;
	}

	struct dirent *item;
	char fullPath[MAX_PATH_LENGTH];

	while (item = readdir(dir))
	{
		// hide dirs and files start with '.'
		if (!config.a && item->d_name[0] == '.')
			continue;

		if (config.r)
		{
			// print full path
			strcpy(fullPath, path);
			if (fullPath[strlen(fullPath) - 1] != '/')
				strcat(fullPath, "/");
			strcat(fullPath, item->d_name);
			printf("%s\t", fullPath);
		}
		else
			printf("%s\t", item->d_name);
	}
	printf("\n\n");

	if (config.r)
	{
		// print dir recursively
		rewinddir(dir);
		while (item = readdir(dir))
		{
			// still hide dirs and files start with '.'
			if (!config.a && item->d_name[0] == '.')
				continue;

			// ignore . and ..
			if (item->d_type == DT_DIR && strcmp(item->d_name, ".") && strcmp(item->d_name, ".."))
			{
				// get dir path and print
				strcpy(fullPath, path);
				if (fullPath[strlen(fullPath) - 1] != '/')
					strcat(fullPath, "/");
				strcat(fullPath, item->d_name);
				printf("%s:\n", fullPath);
				printDir(fullPath);
			}
		}
	}

	closedir(dir);
}

void getConfig(int argc, char **argv)
{
	// init config
	config.r = 0;
	config.a = 0;
	config.l = 0;
	config.h = 0;
	config.m = 0;

	int i;

	int opt;
	while ((opt = getopt(argc, argv, optString)) != -1)
	{
		switch (opt)
		{
		case 'r':
			config.r = 1;
			break;
		case 'a':
			config.a = 1;
			break;
		case 'l':
			config.l = atoi(optarg);
			if (config.l <= 0)
				printf("Warning: -l is lower than 0, ignore.\n");
			break;
		case 'h':
			config.h = atoi(optarg);
			if (config.h <= 0)
				printf("Warning: -h is lower than 0, ignore.\n");
			break;
		case 'm':
			config.m = atoi(optarg);
			if (config.m <= 0)
				printf("Warning: -m is lower than 0, ignore.\n");
			break;
		default:
			break;
		}
	}
}