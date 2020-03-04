# Adventure
This program is simple game akin to old text adventure games like Adventure.
See Adventure File:
There are two programs, one program to build the rooms and their connections and another to emulate the game.
The game starts by showing you the room and possible connecting rooms. You must choose a rooms from the connections list to enter. There are 
a total of 10 rooms. One room is the start room and a another is the end room. 
During the game, the player can also enter a command "time" that returns the current time - this functionality utilizes mutexes and multithreading.
The goal is to find the end room taking as few steps as possible!

To compile the game in a bash shell (see makefile):
1. Compile: Run commands "make build" and "make adventure" without quotations
2. Build rooms and connections: ./build 
3. Start/Play game: ./adventure (The most recent build is used for the game using time stamps and Process IDs)

