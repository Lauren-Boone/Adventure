#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#define NUM_ROOMS 7
#define MAX_OUT 6

struct room {
	char roomName[20];
	char roomType[32];
	int outRoomNum;
	char *connections[MAX_OUT];

};

char* getMostRecentDirectory() {
	int newestDirTime = -1;
	char targetDirName[32] = "boonela.rooms.";
	char newestDir[256];
	memset(newestDir, '\0', sizeof(newestDir));

	DIR* dirtocheck; //starting dir
	struct dirent *fileInDir; //current subdir 
	struct stat *dirStats; //hold info about dir

	dirtocheck = opendir(".");

	if (dirtocheck > 0) {
		while ((fileInDir == readdir(dirtocheck)) != NULL) {
			if (strstr(fileInDir->d_name, targetDirName) != NULL) {
				printf("Found file: %s\n", fileInDir->d_name);
				stat(fileInDir->d_name, &dirStats);

				if ((int)dirStats.st_mtime > newestDirTime) {
					newestDirTime = (int)dirStats->st_mtime;
					memset(newestDir, '\0', sizeof(newestDir));
					strcpy(newestDir, fileInDir->d_name);
					printf("Newwer dir: %s,  time: %d\n", fileInDir->d_name, newestDirTime);
				}
			}
		}
	}
	closedir(dirtocheck);


}