#Declare Makefile Variables
CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I"./libs"
SRC_FILES = ./src/*.cpp ./src/Game/*.cpp ./src/Logger/*.cpp ./src/ECS/*.cpp ./src/AssetStore/*.cpp ./libs/imgui/*.cpp
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua-5.4
OBJ_NAME = GameEngine
DEBUGGER_FLAGS = -g -Wall -Wextra -pedantic -0g

#Makefile Rules
build:
	${CC} ${COMPILER_FLAGS} ${LANG_STD} ${INCLUDE_PATH} ${SRC_FILES} ${LINKER_FLAGS} -o ${OBJ_NAME};

run:
	./${OBJ_NAME}

clean:
	rm ${OBJ_NAME}

debug: 
	${CC} ${DEBUGGER_FLAGS} ${LANG_STD} ${INCLUDE_PATH} ${SRC_FILES} ${LINKER_FLAGS} -o ${OBJ_NAME}
