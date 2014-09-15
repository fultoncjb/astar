# astar makefile

CC = g++
LINK_OPTION = -lz
COMPILE_OPTION = -g
CXXFLAGS = -I /home/cameron/Documents/astar_gui -L /home/cameron/Documents/jsoncpp/build/lib
LIBS = jsoncpp

all: astar

clean:
	rm -f *.o *.out simple

astar: astar.cpp astar.h jsoncpp.cpp menuwindow.cpp menuwindow.h main.cpp 
	${CC} ${COMPILE_OPTION} astar.cpp jsoncpp.cpp menuwindow.cpp main.cpp `pkg-config gtkmm-3.0 --cflags --libs` ${CXXFLAGS} -l ${LIBS} -o astar



