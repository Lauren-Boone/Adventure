
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


//globals
pthread_mutex_t timeLock= PTHREAD_MUTEX_INITIALIZER;
char dirName[256];
char parentDir[256];//used for creating currenttime.txt


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
 


/******************************
function: getTime();
This function is called from the
main thread to open and read the
time from a file
*********************************/
void getTime() {

	char timeFileName[] = "currentTime.txt";
	char currentTime[200]; //string to hold time
	FILE* timeFile;//pinter to file
	//open to read time file
	
	timeFile = fopen(timeFileName, "r");
	//get the time and print it;
	while (fgets(currentTime, sizeof(currentTime), timeFile) != NULL) {
		printf("\n%s\n", currentTime);
	}
	fclose(timeFile);
	return;
}



/************************************
function writeTimefile()
This function opens and writes the
current time to a file 
**************************************/
void * writeTimeFile() {
	char timeString[300]; //time string
	DIR *d;
	//struct dirent *dir;
	time_t currTime; //for getting current current time
	FILE* timeFile; //file pointer to open time file.
	struct tm *tmp; //used example on https://linux.die.net/man/3/strftime
	
					
	//Was is rooms directory need the parent used example from stackoverflow
	//char parent[200]; //https://stackoverflow.com/questions/24050361/c-programming-how-can-i-get-parent-directory
	//snprintf(parent, sizeof(parent), "%s/..", dirName);
	d = opendir(parentDir);
	chdir(parentDir);
	memset(timeString, '\0', sizeof(timeString));

	currTime = time(NULL);

	tmp = localtime(&currTime);
	//get time in exact format 
	strftime(timeString, sizeof(timeString), "%I:%M%P, %A, %B %e, %Y", tmp);
	timeFile = fopen("currentTime.txt", "w");
	//open and write to file 
	fprintf(timeFile, "%s", timeString);
	fclose(timeFile);
	closedir(d);
	return NULL;
	
}



/*************************************
function: initializeThreads
When the time function is called
a lock is placed on a thread that
is created andtold to start writing
time to a file. The thread is joined
to make sure the main program does not continue 
untill the time has been writing. 
the mutex is unlock
**********************************************/
void initializeThreads() {
	
	
	int result;
	pthread_t currTimeThread;
	pthread_mutex_lock(&timeLock);
	result = pthread_create(&currTimeThread, NULL, writeTimeFile, NULL);
	pthread_mutex_unlock(&timeLock);
	pthread_join(currTimeThread,NULL);
	
	

}

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
	//go to the newest rooms directory
	chdir(dirName);
		dir = opendir(".");
		if (dir > 0) {
			while ((files = readdir(dir))) {
				if (strlen(files->d_name) > 2) {//make sure not to read '.' or '..'
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
	
	//loop through each index of struct and read each file line
	for (i = 0; i < NUM_ROOMS; ++i) {
		j = 0;
		memset(line, '\0', sizeof(line));
		file = fopen(roomArr[i].roomName, "r");
		while (fgets(line, sizeof(line), file) != NULL) {
			
			//data: data going into struct, type=what data member to put data in struct
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
			//clear buffer of eronious data
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
		if ((i + 1) == roomArr[index].outRoomNum) {
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
	int roomsVisited[40]; //store indexes on rooms visited
	int valid = -1, stepCount = 0, numCharsEntered = -5, j;
	size_t bufferSize = 32;
	char *lineEntered = NULL, *i;
	//printRoom(findStart(roomArr), roomArr);
	
	while (roomArr[currentRoom].rtype != 2) {//while the current room is not the end room
		//print the room
		printRoom(currentRoom, roomArr);

		//get input for the next room
		numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
		//remove trailing endline replace with /0
		i = strchr(lineEntered, '\n');
		*i = '\0';
		//validation check. if valid move to new room
		if (checkIfValid(roomArr, lineEntered, currentRoom) > -1) {
			currentRoom = getRoomIndex(roomArr, lineEntered);
			roomsVisited[stepCount] = currentRoom;
			stepCount++;
			
			
		}
		//if time command loop
		else if (strcmp(lineEntered, "time") == 0) {
			//loop until a room is entered
			while (strcmp(lineEntered, "time") == 0) {
				initializeThreads();
				getTime();
				printf("\nWHERE TO? >");
				numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
				i = strchr(lineEntered, '\n');
				*i = '\0';
				//if its a room update current room otherwise print huh
				//same as above
				if (checkIfValid(roomArr, lineEntered, currentRoom) > -1) {
					currentRoom = getRoomIndex(roomArr, lineEntered);
					roomsVisited[stepCount] = currentRoom;
					stepCount++;
				}
				else if (strcmp(lineEntered, "time") != 0) {
					printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
				}
			}
			
		}
		//not valid input
		else {
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}
		
	}
	
	free(lineEntered);
	//double check that game is over and print details
	if (roomArr[currentRoom].rtype == 2) {
		printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
		printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", stepCount);
		for (j = 0; j < stepCount; ++j) {
			printf("%s\n", roomArr[roomsVisited[j]].roomName);
		}

	}
	return;
}


void initializeGame() {
	//char dirName[256];
	struct room roomArr[NUM_ROOMS];
	int gameTime = findNewestFile(dirName);
	initializeRooms(roomArr,dirName);
	
	readFile(roomArr,dirName);
	startGame(roomArr);
	//printRoom(findStart(roomArr), roomArr);

	return;
}

int main(){
	getcwd(parentDir, sizeof(parentDir));//used for creating currentTime.txt
	initializeGame();
	//findNewestFile();
	return 0;
}

