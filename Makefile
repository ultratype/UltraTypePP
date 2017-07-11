SRC=$(wildcard src/*.cc src/*.cpp src/*.c)
OUT=./utpp
OPTS=-std=c++14 -luWS -lz -lssl -lpthread -lcrypto -pthread -lpthread -USE_LIBUV -D CPPHTTPLIB_OPENSSL_SUPPORT -ggdb
# I'm using Clang, change to "g++" to use GCC
CC=clang++

# Windows stuff
WINCC=i686-w64-mingw32-g++
WINOUT=./UltraType++

default:
	$(CC) $(OPTS) $(SRC) -o $(OUT)
run:
	clear
	$(CC) $(SRC) $(OPTS) -o $(OUT)
	./$(OUT)
gdb:
	clear
	$(CC) $(SRC) $(OPTS) -o $(OUT)
	echo "Starting GDB..\n";
	gdb ./$(OUT)
windows:
	$(WINCC) -D _MSC_VER=2000 $(OPTS) $(SRC) -o $(WINOUT)