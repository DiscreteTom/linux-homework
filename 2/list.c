#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>

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

void getConfigAndItems(int argc, char **argv, int *items);
void printItem(char *path);
void printDir(char *path);

int main(int argc, char **argv)
{
	int i;
	int *items = (int *)malloc(sizeof(int) * (argc));
	if (!items)
	{
		printf("Failed to allocate memory.\n");
		return 1;
	}

	getConfigAndItems(argc, argv, items);

	if (argc == 1)
	{
		// list all files in current directory
		char cwd[MAX_PATH_LENGTH + 1];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
			printDir(cwd);
		else
		{
			printf("Failed to get current work directory: %m.\n");
			return 2;
		}
	}
	else
	{
		for (i = 0; i < argc; ++i)
		{
			if (items[i])
			{
				printf("%s\n\n", argv[i]);
				printItem(argv[i]);
			}
		}
	}

	free(items);

	return 0;
}

void printItem(char *path)
{
}

void printDir(char *path)
{
	DIR *dir = opendir(path);
	if (!dir)
	{
		printf("Can not open dir: %s", path);
		return;
	}
	struct dirent *item;
	struct stat buf;
	while (item = readdir(dir))
	{
		printf("%s\n", item->d_name);
	}
	closedir(dir);
}

void getConfigAndItems(int argc, char **argv, int *items)
{
	// init config
	config.r = 0;
	config.a = 0;
	config.l = 0;
	config.h = 0;
	config.m = 0;

	int i;
	// init items
	items[0] = 0; // argv[0] is not an item
	for (i = 1; i < argc; ++i)
		items[i] = 1; // means it's an item, not an option

	int opt;
	while ((opt = getopt(argc, argv, optString)) != -1)
	{
		switch (opt)
		{
		case 'r':
			items[optind - 1] = 0;
			config.r = 1;
			break;
		case 'a':
			items[optind - 1] = 0;
			config.a = 1;
			break;
		case 'l':
			items[optind - 2] = 0;
			items[optind - 1] = 0;
			config.l = atoi(optarg);
			if (config.l <= 0)
				printf("Warning: -l is lower than 0, ignore.\n");
			break;
		case 'h':
			items[optind - 2] = 0;
			items[optind - 1] = 0;
			config.h = atoi(optarg);
			if (config.h <= 0)
				printf("Warning: -h is lower than 0, ignore.\n");
			break;
		case 'm':
			items[optind - 1] = 0;
			items[optind - 2] = 0;
			config.m = atoi(optarg);
			if (config.m <= 0)
				printf("Warning: -m is lower than 0, ignore.\n");
			break;
		default:
			break;
		}
	}
}