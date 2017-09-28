all: mines

mines: Board.o Cell.o mines.o
	g++ -std=c++11 -lfltk -lfltk_images Board.o Cell.o Source.o -o mines
	
mines.o: Source.cpp Cell.h Board.h
	g++ -std=c++11 -c -lfltk -lfltk_images Source.cpp
	
Board.o: Board.cpp Board.h Cell.h
	g++ -std=c++11 -c -lfltk -lfltk_images Board.cpp
	
Cell.o: Cell.cpp Cell.h Board.h
	g++ -std=c++11 -c -lfltk -lfltk_images Cell.cpp
	
clean:
	rm *.o mines
	
	
