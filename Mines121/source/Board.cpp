#include "Board.h"
using namespace std;
Fl_JPEG_Image aboutPage("graphics/aboutPage.jpg");

void newGameCallback(Fl_Widget* w, void* v) { 
	Board *board = static_cast<Board*>(v);
	//system("clear"); //Wipe the terminal window
	board->newGame(board->getMineCount());
}

void debugCallback(Fl_Widget* w, void* v) { 
	Board *board = static_cast<Board*>(v);
	board->toggleDebug();
}

void exitCallback(Fl_Widget* w, void* v) {
	exit(0); //Quit the game
}

void aboutCallback(Fl_Widget* w, void* v) {
	Fl_Window *aboutWindow = new Fl_Window(1280, 720, "About");
	Fl_Box *picBox = new Fl_Box(0,0, 1280, 720); 
	//Must create a box because window doesn't support image()
	picBox->image(aboutPage);
	picBox->redraw();
	aboutWindow->show();
}

void timerCallback(void* v) {
	Board *board = static_cast<Board*>(v);
	board->updateTimer();
	Fl::repeat_timeout(1.0, timerCallback, board);

}

void beginnerCallback(Fl_Widget* w, void* v) {
	Board *board = static_cast<Board*>(v); //Cast the old board 
	board->stopTimer(); //Stop timer before deletion; otherwise you get a seg fault
	Board *beginnerBoard = new Board(9); //Create a new board 
	beginnerBoard->seedBoard(10);
	delete board; //Remove the old board
}

void intermediateCallback(Fl_Widget* w, void* v) {
	Board *board = static_cast<Board*>(v);
	board->stopTimer();
	Board *beginnerBoard = new Board(16);
	beginnerBoard->seedBoard(40);
	delete board;
}

void expertCallback(Fl_Widget* w, void* v) {
	Board *board = static_cast<Board*>(v);
	board->stopTimer();
	Board *beginnerBoard = new Board(30);
	beginnerBoard->seedBoard(185);
	delete board;
}

Board::Board(int size) : size(size), numMines(0), gameBoard(size+2, vector<Cell*>(size+2)), 
	gameOver(false), timerOn(false) 
{
	const int cellSize = 20; //Cell size in pixels
	Fl_Double_Window *mainWindow = new Fl_Double_Window(cellSize*(size+2),cellSize*(size+4) , "Minesweeper");
	this->mainWindow = mainWindow; //Define the game window 
	
	for (int i = 0; i < size+2; i++) {//Draw each cell in the window and build the board vector
		for (int j = 0; j < size+2; j++) {
			Cell* gameCell = new Cell((cellSize * i), (cellSize * j)+20, ""); 
			gameBoard.at(i).at(j) = gameCell;
			
			gameCell->setColNum(i); //Set the cell's location in the vector
			gameCell->setRowNum(j);
			gameCell->callback(neighborCallback, this); //Set callback function for each cell
		}
	}
	
	//Create menu bar & submenus
	Fl_Menu_Bar *menu = new Fl_Menu_Bar(0,0, (size+2) * 20, 20);
	menu->box(FL_THIN_UP_BOX);
	
	menu->add("&Game/&Restart	", "^r", newGameCallback, this);
	menu->add("&Game/&New/&Beginner		", "^b", beginnerCallback, this);
	menu->add("&Game/&New/&Intermediate		", "^i", intermediateCallback, this);
	menu->add("&Game/&New/&Expert		", "^e", expertCallback, this);
	menu->add("&Game/&Debug	", "^d", debugCallback, this);
	menu->add("&Game/&Quit	", "^q", exitCallback);
	menu->add("&About	", "", aboutCallback);
	
	//Create status box & counter/timer
	Fl_Box *statusBox = new Fl_Box(0, 20, (size+2)*20, 20);
	statusBoxes.push_back(statusBox);
	
	Fl_Box *mineCountBox = new Fl_Box((size*20)-20, (size+3)*20, 60, 20);
	mineCountBox->box(FL_THIN_DOWN_BOX);
	statusBoxes.push_back(mineCountBox);
	
	Fl_Box *timerBox = new Fl_Box(0, (size+3)*20, 60, 20, "0");
	timerBox->box(FL_THIN_DOWN_BOX);
	statusBoxes.push_back(timerBox);
	
	Fl::visual(FL_DOUBLE|FL_INDEX);
	mainWindow->show();
}

Board::~Board() {
	delete mainWindow;
}

Fl_Window* Board::getMainWindow() {
	return this->mainWindow;
}

void Board::setMainWindow(Fl_Double_Window* window) {
	this->mainWindow = window;
}

void Board::startTimer() {
	Fl::add_timeout(1.0, timerCallback, this);
	timerOn = true;
}

void Board::updateTimer() {
	int timerCount = atoi(statusBoxes.at(2)->label()); //Get the current timer val
	timerCount++;
	statusBoxes.at(2)->copy_label(to_string(timerCount).c_str());
	statusBoxes.at(2)->redraw();
	
}

void Board::stopTimer() {
	Fl::remove_timeout(timerCallback);
	timerOn = false;
}

bool Board::getTimerState() {
	return timerOn;
}

void Board::seedBoard(int minesCount) {//Initialize board 
	srand(time(NULL)); 
	setNumMines(minesCount); 
	statusBoxes.at(1)->copy_label(to_string(numMines).c_str());
	cout << "Board has " << numMines << " mines." << endl;
	
	for (int i = 0; i < size+2; i++) {//Hide the left column 
		gameBoard.at(0).at(i)->hideCell();
	}
	for (int i = 0; i < size+2; i++) {//Hide the right column
		gameBoard.at(gameBoard.size()-1).at(i)->hideCell();
	}
	for (int i = 0; i < size+2; i++) {//Hide the top row
		gameBoard.at(i).at(0)->hideCell();
	}
	for (int i = 0; i < size+2; i++) {//Hide the bottom row
		gameBoard.at(i).at(gameBoard.size()-1)->hideCell();
	}
	
	int x = 0; 
	int y = 0; 
	while (minesCount > 0) {//Loop until all mines are placed
		x = rand() % (size)+1; //Account for "border cells"
		y = rand() % (size)+1;
		if (gameBoard.at(x).at(y)->getMine() == false) {
			gameBoard.at(x).at(y)->setMined(true);
			minesCount--;
		}
	}
	//Set number of neighbors for each cell (otherwise floodfill opens entire board!)
	this->setNeighbors();
}

void Board::setNumMines(int minesCount) {
	this->numMines = minesCount;
}

void Board::setDebugMode(bool debug) {
	this->DEBUGMODE = debug;
}

bool Board::getDebugMode() {
	return DEBUGMODE;
}

void Board::newGame(int minesCount) {//New game of same difficulty 
	for (unsigned int i = 1; i < gameBoard.size(); i++) {
		for (unsigned int j = 1; j < gameBoard.size(); j++) {
			gameBoard.at(i).at(j)->resetCell(); 
		}
	}
	this->seedBoard(minesCount); 
	statusBoxes.at(0)->label("");//Clear the status bar
	statusBoxes.at(2)->copy_label("0"); //Reset the timer
	this->stopTimer();
	this->gameOver = false;
}

int Board::getMineCount() {
	return numMines;
}

void Board::toggleDebug() {//Messy loop, but it seems to work  ¯\_(ツ)_/¯ 
	if (!DEBUGMODE) {
		DEBUGMODE = true;
		statusBoxes.at(0)->label("DEBUG MODE"); //We debug mode boiz
		
		for (unsigned int i = 0; i < gameBoard.size(); i++) {
			for (unsigned int j = 0; j < gameBoard.size(); j++) {
				if (gameBoard.at(i).at(j)->getMine()) {
					gameBoard.at(i).at(j)->label("X");//Mark each cell containing a mine
				}
			}
		}
	}
	else {
		DEBUGMODE = false;
		statusBoxes.at(0)->label("");
		for (unsigned int i = 0; i < gameBoard.size(); i++) {
			for  (unsigned int j = 0; j < gameBoard.size(); j++) {
				if (gameBoard.at(i).at(j)->getMine()) {
					gameBoard.at(i).at(j)->label("");//Remove cell labels
				}
			} 
		}
	}
	
}

Cell* Board::getCell(int x, int y) {
	return gameBoard.at(x).at(y);
}

void Board::floodFill(int x, int y) {
	/*Check if each cell: 
	 *	has no neighbors containing mines
	 * 	is not already open
	 * 	does not contain a mine */
	if (gameBoard.at(x).at(y)->getNumAdjacent() == 0 
		&& gameBoard.at(x).at(y)->checkOpen() == false 
		&& gameBoard.at(x).at(y)->getMine() == false
		&& gameBoard.at(x).at(y)->getFlag() == false) 
	{
		gameBoard.at(x).at(y)->openCell();
		//Recursion (called on all 8 adjacent cells)
		floodFill(x-1,y); 
		floodFill(x+1,y);
		floodFill(x,y-1);
		floodFill(x,y+1);
		floodFill(x+1,y+1);
		floodFill(x+1,y-1);
		floodFill(x-1,y+1);
		floodFill(x-1,y-1);
	}
	else if (!gameBoard.at(x).at(y)->getMine()) {//Don't open cells with mines; this F's the player over :(
		gameBoard.at(x).at(y)->openCell();
		gameBoard.at(x).at(y)->drawNumber(); //Draw border neighbor count
		return; //Stop opening if cell is adjacent to a mine
	}
}

void Board::countNeighbors(Cell* currentCell) {
	int y = currentCell->getRowNum(); //Get indices of cell
	int x = currentCell->getColNum();
	int numAdj = 0;
	
	for (int i = x-1; i <= x+1; i++) {//Loop through 3x3 region around current cell
		for (int j = y-1; j <= y+1; j++) {
			if (gameBoard.at(i).at(j)->getMine() == true) {
				numAdj++; 
			}
		}
	}
	//Set the cell's neighbor count - this does not change throughout the game
	currentCell->setNumAdjacent(numAdj);
}

int Board::countFlags(Cell* currentCell) {
	int y = currentCell->getRowNum(); //Get indices of cell
	int x = currentCell->getColNum(); 
	int numFlags = 0; 
	
	for (int i = x-1; i <= x+1; i++) {
		for (int j = y-1; j <= y+1; j++) {
			if (gameBoard.at(i).at(j)->getFlag() == true) {
				numFlags++;
			}
		}
	}
	return numFlags;
}

void Board::chordCells(Cell* currentCell) {//Opens all non-flagged adj. cells
	int y = currentCell->getRowNum(); //Get indices of cell
	int x = currentCell->getColNum(); 
	
	for (int i = x-1; i <= x+1; i++) {
		for (int j = y-1; j <= y+1; j++) {
			if (gameBoard.at(i).at(j)->getFlag() == false && 
				gameBoard.at(i).at(j)->getMine() == false) {
					floodFill(i,j); //Cascade if cell does not have mine/flag
			}
			else {//Otherwise, open w/out cascading
				gameBoard.at(i).at(j)->openCell();
			}
		}
	}
	
}

void Board::setNeighbors() {//Initialize neighbor count for each cell in the board
	for (int i = 1; i <= size; i++) {//Start at 1 to account for left border
		for (int j = 1; j <= size; j++) {
			countNeighbors(gameBoard.at(i).at(j)); 
		}
	}
}

void Board::checkLost(Cell* currentCell) {
	for (int i = 1; i <= size; i++) {
		for (int j = 1; j <= size; j++) {
			if (gameBoard.at(i).at(j)->checkLost() == true) {
				cout << "game over!" << endl;
				gameOver = true;
				this->revealBoard();
			}
		}
	}	
}

void Board::deactivateBoard() {
	for (int i = 1; i <= size; i++) {
		for (int j = 1; j <= size; j++) {
			gameBoard.at(i).at(j)->deactivate();
		}
	}
}

void Board::showMines() {//flag all mines when player wins
	for (int i = 1; i <= size; i++) {
		for (int j = 1; j <= size; j++) {
			gameBoard.at(i).at(j)->showMine();
		}
	}
}

void Board::checkWin() {
	int numCovered = 0;
	for (int i = 1; i <= size; i++) {
		for (int j = 1; j <= size; j++) {
			if (gameBoard.at(i).at(j)->checkOpen() == false) {
				numCovered++;
			}
		}
	}
	if (numCovered == numMines) {//Win condition
		statusBoxes.at(0)->label("You Won!!!");
		this->showMines();
		this->stopTimer();
		this->deactivateBoard();
	}
}

void Board::setGameOver(bool lose) {
	this->gameOver = lose;
}

bool Board::getGameState() {
	return this->gameOver;
}

void Board::revealBoard() {
	for (int i = 1; i <= size; i++) {
		for (int j = 1; j <= size; j++) {
			gameBoard.at(i).at(j)->revealCell();
			gameBoard.at(i).at(j)->deactivate();
		}
	}
	statusBoxes.at(0)->label("You lost!");
	this->stopTimer();
}

int Board::countTotalFlags() {
	int flagCount = 0;
	for (int i = 1; i <= size; i++) {
		for (int j = 1; j <= size; j++) {
			if (gameBoard.at(i).at(j)->getFlag()) {
				flagCount++;
			}
		}
	}
	return flagCount;
}

void Board::updateMineCounter(int count) {
	statusBoxes.at(1)->copy_label(to_string(count).c_str());
}

int Board::getNumMines() {
	return numMines;
}

void neighborCallback(Fl_Widget* w, void* v) {//THE IMPORTANT CALLBACK
	Cell *cell = dynamic_cast<Cell*>(w);
	Board *board = static_cast<Board*>(v);
	int y = cell->getRowNum(); //Get the location of the called cell
	int x = cell->getColNum();
	if (!board->getTimerState()) {//Start timer when first cell opened
		board->startTimer();
	}
	
	if (cell->getFlag() || cell->getMark()) {
		board->updateMineCounter(board->getNumMines() - board->countTotalFlags());
	}
	else if (cell->getMine() && cell->getFlag() == false) {//If player opens a cell containing a mine
		cout << "game over!" << endl;
		board->setGameOver(true);
		board->revealBoard();
	}
	else if ((board->countFlags(cell) == cell->getNumAdjacent()) 
		 && cell->checkOpen()) 
	{//Can only chord w/ correct flag count and if cell is open
		board->chordCells(cell);
	}
	if (!cell->getFlag() && !cell->getMark()) {
		board->floodFill(x,y);//Cascade function
		board->checkLost(cell);
		board->checkWin();
	}
	
	if(board->getDebugMode()) cout << "Adjacent Mines: " << cell->getNumAdjacent() << endl; 
}
