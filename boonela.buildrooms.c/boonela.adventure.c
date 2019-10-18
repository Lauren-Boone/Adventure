
#define _GNU_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#define NUM_ROOMS 7
#define MAX_OUT 6
#define STRING_SIZE 40

enum roomType {
	START_ROOM = 0, MID_ROOM, END_ROOM
};

struct room {
	char roomName[20];
	char roomType[32];
	int outRoomNum;
	char connections[MAX_OUT][STRING_SIZE];
	enum roomType rtype;

};
 

/************************************
function: findNewestFile()
Description: THis file finds the directory
with the newest time stamp from build rooms
*****************************************/
int findNewestFile(char *newestDir) {
	int newestDirTime = -1; //for time stamp
	char targetDirName[32] = "boonela.rooms.";
	//char newestDir[356];
	memset(newestDir, '\0', sizeof(newestDir));

	DIR *dirtocheck; //starting dir(current)
	struct dirent *fileInDir; //current subdir 
	struct stat dirStats; //hold info about dir

	dirtocheck = opendir(".");//open the current directory

	if (dirtocheck > 0) { //make sure its open
		while ((fileInDir = readdir(dirtocheck))) {//read list of files/directorys
			if (strstr(fileInDir->d_name, targetDirName) != NULL) {//check if found match
				//printf("Found file: %s\n", fileInDir->d_name);
				stat(fileInDir->d_name, &dirStats); //get time

				if ((int)dirStats.st_mtime > newestDirTime) { //check if the this is newest
					newestDirTime = (int)dirStats.st_mtime;
					memset(newestDir, '\0', sizeof(newestDir));
					strcpy(newestDir, fileInDir->d_name);
					//printf("Newwer dir: %s,  time: %d\n", fileInDir->d_name, newestDirTime);
				}
			}
		}
	}
	closedir(dirtocheck);
	//printf("Newwer dir: %s,  time: %d\n", newestDir, newestDir);
	return newestDirTime;
}


/**********************************************
function intializeRooms()
This functions initilizes the struct to null/0
values and sets the names to the file names.
This make it easier to open the files
************************************************/
void initializeRooms(struct room *roomArr, char* dirName) {
	DIR *dir;
	struct dirent *files;
	char rName[40];
	int i, j = 0;
	//first make sure everything is set to null/0 so no wierd values
	for (i = 0; i < NUM_ROOMS; ++i) {
		memset(roomArr[i].roomName, '\0', sizeof(roomArr[i].roomName));
		roomArr[i].outRoomNum = 0;
	}
	chdir(dirName);
		dir = opendir(".");
		if (dir > 0) {
			while ((files = readdir(dir))) {
				if (strlen(files->d_name) > 2) {
					memset(roomArr[j].roomName, '\0', sizeof(roomArr[i].roomName));
					strcpy(roomArr[j].roomName, files->d_name);
					//printf(" %s", files->d_name);
					j++;
				}
			}
		}
		closedir(dir);
		return;

	}
	



/****************************************
function: readFile
This functions reads the data from the file
and puts it in the room struct
********************************************/
void readFile(struct room *roomArr, char* dirName) {
	//initializeRooms(roomArr, dirName);//name rooms based on files
	chdir(dirName);
	char type[30];
	char punc[15];
	char data[30];
	char line[200];
	FILE *file;
	int i, j = 0;
	
	for (i = 0; i < NUM_ROOMS; ++i) {
		j = 0;
		memset(line, '\0', sizeof(line));
		file = fopen(roomArr[i].roomName, "r");
		while (fgets(line, sizeof(line), file) != NULL) {
			sscanf(line, "%s %s %s", type,punc, data);

			if(strcmp(type, "CONNECTION")==0){
				memset(roomArr[i].connections[j], '\0', sizeof(roomArr[i].connections[j]));
				strcpy(roomArr[i].connections[j], data);
				roomArr[i].outRoomNum++;
				//printf("outnum:%d", roomArr[i].outRoomNum);
				j++;
				//printf("connection: %s", data);
			}
			else if ((strcmp(type, "ROOM") == 0) && (strcmp(punc, "TYPE:")==0)) {
				//memset(line, '\0', sizeof(line));
				//sscanf(line, "%s %s %* %s", type, punc, data);
				memset(roomArr[i].roomType, '\0', sizeof(roomArr[i].roomType));
				strcpy(roomArr[i].roomType, data);
				//printf("type: %s", data);
				if (strcmp(data, "START_ROOM")==0) {
					roomArr[i].rtype = 0;
				}
				else if (strcmp(data, "MID_ROOM")==0) {
					roomArr[i].rtype = 1;
				}
				else if (strcmp(data, "END_ROOM")==0) {
					roomArr[i].rtype = 2;
				}
				
			}
			memset(line, '\0', sizeof(line));
			memset(type, '\0', sizeof(type));
			memset(punc, '\0', sizeof(punc));
			memset(data, '\0', sizeof(data));

		}
		fclose(file);

	}
}


/************************************************
function printRoom()
This functino is a helper function that prints the 
current room
************************************************/
void printRoom(int index, struct room *roomArr) {
	int i;
	printf("\n\nCURRENT LOCATION: %s\n", roomArr[index].roomName);
	printf("POSSIBLE CONNECTIONS: ");
	for (i = 0; i < roomArr[index].outRoomNum; ++i) {
		if ((i - 1) == roomArr[index].outRoomNum) {
			printf("%s.", roomArr[index].connections[i]);
		}
		else {
			printf("%s, ", roomArr[index].connections[i]);
		}
	}
	printf("\nWHERE TO? >");
	return;
}

/*****************************************
findStart()
THis function is a helper function that
finds the starting room
******************************************/
int findStart(struct room* roomArr) {
	int i;
	for (i = 0; i < NUM_ROOMS; ++i) {
		//printf("%s\n", roomArr[i].roomType);
		if ((strcmp(roomArr[i].roomType, "START_ROOM")==0)) {
			return i;
		}
	}
	return -1;
}

/******************************************
function: checkIfValid(){
This function checks that the use input
a valid room name
********************************************/
int checkIfValid(struct room *roomArr, char *lineEntered, int index) {
	int i;
	//printf("%d", roomArr[index].outRoomNum);
	for (i = 0; i < roomArr[index].outRoomNum; ++i) {
	//	printf("room: %s connects: %s", roomArr[index].roomName, roomArr[index].connections[i]);
		if (strcmp(roomArr[index].connections[i], lineEntered) == 0) {
			return 0;
		}
	}
	return -1;
}



/*************************************************
function: getRoomIndex
This functions uses strcmp to idenfy the index
of the room in the array
**************************************************/
int getRoomIndex(struct room *roomArr, char *input) {
	int i;
	for (i = 0; i < NUM_ROOMS; ++i) {
		if (strcmp(roomArr[i].roomName, input) == 0) {
			return i;
		}
	}
	return -1;
}

/*****************************************
function: startGame
This function starts the game and holds the 
logic for the game
******************************************/
void startGame(struct room *roomArr) {
	int currentRoom = findStart(roomArr);
	int roomsVisited[20];
	int valid = -1, stepCount = 0, numCharsEntered = -5;
	size_t bufferSize = 32;
	char *lineEntered = NULL, *i;
	//printRoom(findStart(roomArr), roomArr);
	while (roomArr[currentRoom].rtype != 2) {
		printRoom(currentRoom, roomArr);
		numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
		i = strchr(lineEntered, '\n');
		*i = '\0';
		if (checkIfValid(roomArr, lineEntered, currentRoom) > -1) {
			currentRoom = getRoomIndex(roomArr, lineEntered);
			stepCount++;
		}

		else {
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}
		
	}
	free(lineEntered);
	if (roomArr[currentRoom].rtype == 2) {
		printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
		printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", stepCount);

	}
	return;
}


void initializeGame() {
	char dirName[256];
	struct room roomArr[NUM_ROOMS];
	int gameTime = findNewestFile(dirName);
	initializeRooms(roomArr,dirName);
	
	readFile(roomArr,dirName);
	startGame(roomArr);
	//printRoom(findStart(roomArr), roomArr);

	return;
}

int main(){
	initializeGame();
	//findNewestFile();
	return 0;
}

