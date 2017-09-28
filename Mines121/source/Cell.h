#pragma once 
#include <iostream>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_JPEG_Image.H>

class Cell : public Fl_Button {
	public:
		Cell(int x, int y, const char* label); //Constructor;
		int handle(int event);
		
		//Mouse click/event functions
		void openCell();
		void flagCell();
		void markCell();
		void unMarkCell();
		void hideCell();
		void resetCell();           //Resets bool all values to default (false)
		void drawNumber();          //Prints neighbor count on opened cell
		void revealCell();			//Reveal the state of the cell (when a game is lost)
		void showMine(); 			//Change mine to flag when game is won
		//Getters & setters 
		void setRowNum(int num);    //Define cell's vector indices (called in board constructor)
		void setColNum(int num);
		void setNumAdjacent(int num); //Define number of adjacent cells containing mines
		int getNumAdjacent();
		int getRowNum();
		int getColNum();
		bool checkLost();           //Returns whether or not cell lost the game
		bool getFlag();  			//Returns flagged state
		bool getMine(); 			//Returns mine state
		bool getMark();
		bool checkOpen(); 			//Check if cell is open
		void setMined(bool mine);   //Sets mine state 
		void labelMines();			//Sets text label (for debug use)
		
	private:
		const int size;  //Defined by image dimensions in constructor (should be 20x20)
		int rowNum;      //Cell's vector indices in board
		int colNum;
		int numAdjacent; //Number of neighboring mines
		
		bool hasMine;
		bool isOpen;
		bool isFlagged;
		bool isMarked;
		bool lostMine;  //If this is the cell that caused the player to lose
};
