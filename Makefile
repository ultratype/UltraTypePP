SRC=$(wildcard src/*.cc src/*.cpp src/*.c)
OUT=./utpp
OPTS=-std=c++14 -luWS -lz -lssl -llz4 -lpthread -pthread -USE_LIBUV -ggdb
# Use clang, change to use gcc instead
CC=clang++

default:
	$(CC) $(SRC) $(OPTS) -o $(OUT)
run:
	$(CC) $(SRC) $(OPTS) -o $(OUT)
	./$(OUT)
gdb:
	$(CC) $(SRC) $(OPTS) -o $(OUT)
	echo "Starting GDB..\n";
	gdb ./$(OUT)