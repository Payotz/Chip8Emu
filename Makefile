CXX = g++
CPPFLAGS = -c -std=c++17 -static-libstdc++
LIB_DIR = /usr/lib/
LIB_LIST = -lSDL2 -lgl3w -lSDL2_mixer
OBJ_LIST = main.o 

all: release

release: main.o
	$(CXX) -o main $(OBJ_LIST) -L $(LIB_DIR) $(LIB_LIST)

main.o:
	$(CXX) $(CPPFLAGS) main.cpp

clean:
	rm *.o
