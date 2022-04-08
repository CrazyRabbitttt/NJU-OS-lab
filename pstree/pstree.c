#include <ctype.h>
#include <dirent.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


#define MAX_PIDS 1024
#define MAX_ADD_LEN 512
#define MAX_TREE_DEPTH 25

/*===================global setting ===================*/
struct {
	int numeric_sort;
	int show_pid;
}global_setting;

typedef struct ProcInfonode{
	pid_t pid;
	char name[50];
	pid_t ppid;
	int level;
}ProcInfo;

ProcInfo procs[MAX_PIDS];



int Index;
static char *optstring = "pnv";

static const struct option long_options[] = {
    {"show_pids", no_argument, NULL, 'p'},
    {"numeric_sort", no_argument, NULL, 'n'},
    {"version", no_argument, NULL, 'v'},
    {NULL, no_argument, NULL, 0}};

void Showuse() {
	printf("useage : pstree [-p, --show-pids] [-n,--numeric-sort] [-v, --version]\n");
}

typedef struct Treenode {
	ProcInfo *procs;
	struct Treenode *left_brother;			//not two childs, many brother's and one father
	struct Treenode *right_father;
}TreeNode;




void FillProcInfo(char *filename, int *proc_index)
{
	FILE * fd;
	fd = fopen(filename, "r");

	if (fd) {
		printf("sucessfully opend the file:%s\n", filename);
		char tmp[MAX_ADD_LEN];
		char finalS[MAX_ADD_LEN];
		while (fgets(tmp, 160, fd) != NULL) {
			if (tmp[0] == 'N' && tmp[1] == 'a') {
				sscanf(tmp, "Name:   %s", finalS);
				printf("The name of proc : %s\n", finalS);
				strcpy(procs[*proc_index].name, finalS);
			}else if (tmp[0] == 'P' && tmp[1] == 'P') {
				sscanf(tmp, "PPid:   %s", finalS);
				printf("The ppid is %s\n", finalS);
				procs[*proc_index].ppid = atoi(finalS);
			}else if (tmp[0] == 'P' && tmp[1] == 'i') {
				sscanf(tmp, "Pid:    %s", finalS);
				printf("The pid is %s\n", finalS);
				procs[*proc_index].pid = atoi(finalS);
			}

		}

	}else {
		perror("failed to open the file : \n");
		exit(EXIT_FAILURE);
	}

	(*proc_index) ++;					//index ++

	Index = *proc_index;
}


int main(int argc, char **argv)
{
	//first : open the /proc dir

	DIR				*pdir;
	struct dirent	*ent;
	pdir = opendir("/proc");
	char child[512];
	while ((ent = readdir(pdir)) != NULL) {
		if (ent -> d_type == 4 && isdigit(ent -> d_name[0]))		//is a number dir
		{
			int a;
			int proc_index;				//which proc will you fill , the index of the array of the ProcInfo
			a = atoi(ent -> d_name);
			sprintf(child, "%s/%s/status", "/proc", ent -> d_name);
			printf("the dirname is : %s\n", child);
			FillProcInfo(child, &proc_index);
		}
	}
	closedir(pdir);


	/*============= PRINT THE PROCINFO=============*/


	int c;
	while (1) {
		c = getopt_long(argc, argv, optstring, long_options, 0);
		if (c == -1) break;
		switch(c) {
			case 'p':
				printf("you choosed p\n");
			break;
			case 'n':
				printf("you choosed n\n");
			break;
			case 'v':
				printf("Version : 1.0\n");
			break;
			default :
				Showuse();
		}
	}


}
