all: explore read_and_print

arduino:
	ano build --cppflags="-std=c++11 -DEMBED"

explore: src/Explore.cpp src/Maze.cpp src/Maze.h src/MazeIO.cpp src/MazeIO.h src/Mouse.cpp src/Mouse.h src/Solvers.cpp src/Solvers.h src/Node.cpp src/Node.h src/Direction.cpp src/Direction.h
	g++ -std=c++11 -g -o build/explore src/Explore.cpp src/Maze.cpp src/MazeIO.cpp src/Mouse.cpp src/Solvers.cpp src/Node.cpp src/Direction.cpp

read_and_print: src/ReadAndPrint.cpp src/Maze.cpp src/Maze.h src/MazeIO.cpp src/MazeIO.h src/Mouse.cpp src/Mouse.h src/Solvers.cpp src/Solvers.h src/Node.cpp src/Node.h src/Direction.cpp src/Direction.h
	g++ -std=c++11 -g -o build/read_and_print src/ReadAndPrint.cpp src/Maze.cpp src/MazeIO.cpp src/Mouse.cpp src/Solvers.cpp src/Node.cpp src/Direction.cpp

clean:
	rm build/*
	ano clean
