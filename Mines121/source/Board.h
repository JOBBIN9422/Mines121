#pragma once 
#include <iostream>
#include <vector>
#include <ctime>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Timer.H>
#include <FL/Fl_Text_Display.H>
#include "Cell.h"

class Board {
	public:
		Board(int size);//Constructor
		~Board();		//Destructor
		
		int handle(int event);
		void seedBoard(int minesCount); 		//initialize board contents (mines/cell state)
		void countNeighbors(Cell* currentCell); //Count neighbors for one cell
		void floodFill(int x, int y);		    //Recursive cascading algorithm
		void chordCells(Cell* currentCell);   	//Open all adj. cells if flags = adj. mines
		
		void setNeighbors(); 					//Define neighbor count for all cells
		void setNumMines(int minesCount);
		void setDebugMode(bool debug);
		void setGameOver(bool lose);
		void setMainWindow(Fl_Double_Window* window);
		
		Cell* getCell(int x, int y);			//Return pointer to a cell in the board
		bool getDebugMode();
		bool getGameState();					//Returns bool for game over
		bool getTimerState();
		int  getNumMines();
		int  getSize();
		int  getMineCount(); 					//Returns # of mines contained in board 
		int  countFlags(Cell* currentCell);		//Count flags adjacent to current cell
		int  countTotalFlags();
		
		void toggleDebug();
		void newGame(int minesCount);
		void checkLost(Cell* currentCell);		//Check if player has uncovered a mine
		void checkWin();
		void revealBoard();						//Reveals state of all cells (when game lost)
		void showMines();
		void deactivateBoard(); 
		void updateMineCounter(int count);	
		void updateTimer();
		void startTimer();
		void stopTimer();
		Fl_Window* getMainWindow();
		
	private:
		int size; //FIX: Change to two sizes (rows and cols)
		int numMines;
		int numFlags;
		std::vector<std::vector<Cell*> > gameBoard;
		std::vector<Fl_Box*> statusBoxes; 		//contains status box/timer
		bool DEBUGMODE = false; //FIX: this is a jank-ass debug method 
		bool gameOver;
		bool timerOn;
		Fl_Window* mainWindow; //The game window
};

//Button callbacks
void newGameCallback(Fl_Widget* w, void* v);
void debugCallback(Fl_Widget* w, void* v);
void beginnerCallback(Fl_Widget* w, void* v);
void intermediateCallback(Fl_Widget* w, void* v);
void expertCallback(Fl_Widget* w, void* v);
void exitCallback(Fl_Widget* w, void* v);

//Logic callbacks
void neighborCallback(Fl_Widget* w, void* v);
void timerCallback(void* v);
