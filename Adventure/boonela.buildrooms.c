#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>


#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

#define MAX_OUT 6
#define MIN_OUT 3
#define NUM_ROOMS 7




//Enum for the room names and types
//enum roomNames {one=0, two, three, four, five, six, seven, eight, nine, ten };
//enum roomTypes {start=0, mid, end};
struct room {
	char roomName[20];
	char roomType[32];
	int outRoomNum;
	char *connections[MAX_OUT];
	//enum roomNames name;
	//enum roomTypes type;
};





/**************************************
function: chooseRooms
Accepts: array of struct
returns: none
description: This function randomizes a list
of numbers 0 through 9, using durstenfeld's shuffly
the first seven are the rooms used. 
**************************************/
void chooseRooms(struct room *roomArr) {
	int i, x;

	int roomId[10] = { 0,1,2,3,4,5,6,7,8,9 }; //id room room name to randomize
	int swap = 0, temp = 0;
	
	//using the durstenfeld's shuffle
	//Randomly change the numbers in the array. 
	//randomly swap values at different locations with the last value, then decrement the range
	for (x = 9; x > 0; --x) {
		swap =  rand() % x;//random number between 0 and x(range lowers each loop)
		temp = roomId[x];
		roomId[x] = roomId[swap];
		roomId[swap] = temp;
	}

	//Now that all ten are randomized just use the first seven randomized rooms
	for ( i = 0; i < NUM_ROOMS; ++i) {
		//roomArr[i].name = roomId[i];
		memset(roomArr[i].roomName, '\0', sizeof(roomArr[i].roomName));
		switch (roomId[i]) {
		case 0: 
			strcpy(roomArr[i].roomName, "one");
			break;
		case 1:
			strcpy(roomArr[i].roomName, "two");
			break;
		case 2:
			strcpy(roomArr[i].roomName, "three");
			break;
		case 3:
			strcpy(roomArr[i].roomName, "four");
			break;
		case 4:
			strcpy(roomArr[i].roomName, "five");
			break;
		case 5: 
			strcpy(roomArr[i].roomName, "six");
			break;
		case 6:
			strcpy(roomArr[i].roomName, "seven");
			break;
		case 7:
			strcpy(roomArr[i].roomName, "eight");
			break;
		case 8:
			strcpy(roomArr[i].roomName, "nine");
			break;
		case 9:
			strcpy(roomArr[i].roomName, "ten");
			break;
		}
		
	}

	return;
}




/******************************************
function: isAlreadyConnect
arguments: an array of room structs connection, int
return: 1(true), 0(false)
description:
***********************************************/
int isAlreadyConnect(struct room *roomArr, int room, int otherRoom) {
	int i, j;
	//loop through all of one room's connection and check to see if they match the otherroom's name
	for (i = 0; i < roomArr[room].outRoomNum; ++i) {

		if (strcmp(roomArr[room].connections[i], roomArr[otherRoom].roomName) == 0) {
			return 1;
		}
	}
	return 0;
}



/************************************
function: hasMaxConnect(int)
arguments:integer (represents number of rooms)
returns: 1(true) or 0(false)
description: Determines if the number of connects
is the max
**********************************************/
int hasMaxConnect(int connectionsIn) {
	if (connectionsIn < 6) {
		return 0;
	}
	return 1;
}




/**************************************
function: connectRooms
argurments, array of struct, int, int
return: none
description: This functions adds a connection
between the rooms. Both rooms will not be
connected to each other. The manipulates the
connections array of char* in the struct
rooms.
*******************************************/
void connectRooms(struct room *roomArr, int room1, int otherRoom) {



	roomArr[room1].connections[(roomArr[room1].outRoomNum)] = malloc(20 * sizeof(char));
	roomArr[otherRoom].connections[(roomArr[otherRoom].outRoomNum)] = malloc(20 * sizeof(char));

	memset(roomArr[room1].connections[(roomArr[room1].outRoomNum)], '\0', sizeof(roomArr[otherRoom].roomName));
	memset(roomArr[otherRoom].connections[(roomArr[otherRoom].outRoomNum)], '\0', sizeof(roomArr[room1].roomName));
	strcpy(roomArr[room1].connections[(roomArr[room1].outRoomNum)], roomArr[otherRoom].roomName);
	strcpy(roomArr[otherRoom].connections[(roomArr[otherRoom].outRoomNum)], roomArr[room1].roomName);
	return;
}


/********************************************
function: connectionTest()
args: struct array
return: none
description: This function tests whether the
array value needs connects and if the connect is
valid. This function is independent so that if
there is an array value that does not have three
connections this function can be called once without
itterating through the intire array
****************************************************/
void addConnection(struct room *roomArr, int index) {
	int hasConnect = 0; //this is a bool value to make sure a connection is created.
	int randNum;
	int i = index;
	//while the connection has not been made
	while (hasConnect == 0) {
		randNum = rand() % 7;
		//check to make sure the max limit is not reached
		if ((hasMaxConnect(roomArr[i].outRoomNum) == 0) && (hasMaxConnect(roomArr[randNum].outRoomNum) == 0)) {
			if (randNum != i) { //make sure the connection is not it itself
				//check that the connection doesn't already exist (0for false)
				if (isAlreadyConnect(roomArr, i, randNum) == 0) {

					connectRooms(roomArr, i, randNum);
					roomArr[i].outRoomNum++;
					roomArr[randNum].outRoomNum++;
					hasConnect = 1;
					//								

				}
			}
		}
	}
	return;
}

/***********************************
function: checkConnectionMin()
arguments: array
return 1(true) 0(false)
description: This function checks to make
sure that the minimun number of connections
for each room is met
********************************************/
int hasConnectionMin(struct room *roomArr) {
	int i;
	for (i = 0; i < 7; ++i) {
		if (roomArr[i].outRoomNum < MIN_OUT) {
			addConnection(roomArr, i);
		}
	}
	
}




/**************************************
function: createConnections
Arguments: struct rooms array
returns: none
description:This function is separate from 
add connection so that the program can add 
a connection to a connection deficint room without
having to loop through all the rooms again.
This saves a lot a time during run time. 
******************************************/
void startConnectionsLoop(struct room *roomArr) {
	
	int i, j;
	
	
	//iterate through each room
	for ( i = 0; i < 7; ++i) {
		//printf("Rooms:%s\n", roomArr[i].roomName);
		//start with 2 connects first
			for (j = 0; j < 2; ++j) {
				addConnection(roomArr, i);
					
			}
			
	}
	hasConnectionMin(roomArr);
	return;
}









/******************************************
function: chooseRoomType
arguments: array of room struct
return: none
description: This function assigns the type
of rooms. Since the rooms are alread randomized
the first room in the struct array is the start
and the last is the end
**********************************************/
void chooseRoomType(struct room *roomArr){
	int i;
	for ( i = 0; i < 7; ++i) {

		//first room
		if (i == 0) {
			strcpy(roomArr[i].roomType, "START_ROOM");
			//roomArr[i].type = 0;
		}

		//last room
		else if (i == 6) {
			strcpy(roomArr[i].roomType, "END_ROOM");
			//roomArr[i].type = 2;
		}
		//all others are midrooms
		else {
			strcpy(roomArr[i].roomType, "MID_ROOM");
			//roomArr[i].type = 1;
		}
	}
	return;
}


/******************************************
function: initializeRooms()
arguments: none
returns: none
description: This functions creates the array
of type struct rooms and initlizes the values.
*********************************************/
void initializeRooms(struct room *roomArr) {
	
	//strcpy(roomArr[0].roomName, "Lauren");
	//printf("%s", roomArr[0].roomName);
	chooseRooms(roomArr);
	int i, j;
	for (i = 0; i < 7; ++i) {
		roomArr[i].outRoomNum = 0;
	}
	chooseRoomType(roomArr);

	startConnectionsLoop(roomArr);
	//hasConnectionMin(roomArr);
	for ( i = 0; i < 7; i++) {
	//	printf("Room: %s\nRoomType: %s\n\n", roomArr[i].roomName, roomArr[i].roomType);
		for (j = 0; j < roomArr[i].outRoomNum; ++j) {
			//printf("connect: %s\n", roomArr[i].connections[j]);
		}
	}
	return;
}

/*****************************
function: createDirectory()
args: none
return:none
description: this function
creates a directory to store
all of the rooms in. The directory
is identified using the Process Id.
*****************************/
void createDirectory() {
	char dirName[] = "./boonela.rooms.";
	char folderName[] = "boonela.rooms.";
	int pid = getpid();
	memset(dirName, '\0', sizeof(dirName));
	sprintf(dirName, "%s%d", folderName, pid);
	mkdir(dirName, 0770);
	return;

}


void addFiles( struct room *roomArr) {
	FILE *roomFiles;
	char dirName[] = "./boonela.rooms.";
	char folderName[] = "./boonela.rooms.";
	int pid = getpid();
	memset(dirName, '\0', sizeof(dirName));
	sprintf(dirName, "%s%d", folderName, pid);


	chdir(dirName);
	int i, j;
	for (i = 0; i < NUM_ROOMS; ++i) {
		roomFiles = fopen(roomArr[i].roomName, "w");

		fprintf(roomFiles, "ROOM NAME:  %s\n", roomArr[i].roomName);
		for (j = 0; j < roomArr[i].outRoomNum; ++j) {
			fprintf(roomFiles, "CONNECTION %d: %s\n", (j + 1), roomArr[i].connections[j]);
		}
		fprintf(roomFiles, "ROOM TYPE: %s\n", roomArr[i].roomType);
		fclose(roomFiles);
	}

	return;
}


void removeConnections(struct room *roomArr) {
	
	int i,j;
	for (i = 0; i < NUM_ROOMS; ++i) {
		for (j = 0; j < roomArr[i].outRoomNum; ++j) {
			free(roomArr[i].connections[j]);
		}
	}
	return;
}
int main() {

	srand((int)time(NULL));
	struct room roomArr[NUM_ROOMS];
	createDirectory();
	initializeRooms(roomArr);
	addFiles( roomArr);
	removeConnections(roomArr);

	
	

	return 0;
}