#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include "ZeoFlow_SFML.h"
#include "MD2.h"

//Compiler Directives
using namespace std;
using namespace zeoFlow;

ZeoFlow_SFML zfSFML; //declaring a variable of ZeoFlow_SFML type - help us to draw the images easier
MD2 btnLvl; //declaring a variable of MD2 type - help us to draw the buttons

sf::RenderWindow window(sf::VideoMode(800, 500), "Magical Drop 2"); //the game screen

//declaring the value for each screen that can be found in game
const int SCENE_SPLASH_SCREEN = 0;
const int SCENE_GAME_MENU_SCREEN = 1;
const int SCENE_GAME_SCREEN = 2;
const int SCENE_OPTIONS_SCREEN = 3;
const int SCENE_SELECT_LVL = 4;
const int SCENE_HOW_TO_SCREEN = 5;
const int objectSize = 35; //the ball size
int currentScreen = SCENE_SPLASH_SCREEN; //the current screen
int lvlUnlocked = 1; //variable that store the number of unlocked levels
int gameLvl; //variable that store the current level
int inGameEventType = 1; //variable storing the type of notification that appear in game
int userCharacter = 1; //here we store the character that the user choose (by default is the first character - the boy)
int finishLvlScore[6]; //array that stores each level target
int characterAnimation = 0; //the current sprite that makes the animation of the character - while playing
//the current sprite of the characters that appear in game menu
int stateNinja1 = 0, stateNinja2 = 3, stateNinja3 = 6, stateKnight1 = 0, stateKnight2 = 5, stateBoy = 0, stateGirl = 0,
	stateJack1 = 0, stateJack2 = 0;
int lvlScore = 0; //the current score
int menuSquares = 0, gameLostLines = 0; //declaring the items that helps to animate the menu/end screen while playing
int optionSelected = 0; //helps us to know which option was selected from the in game menu/end screen
int ballsInHandNo = 0, ballsInHandType = 0; //store the data about the ball(s) that are in the character hands
int levelLines, levelColumns; //the total number of lines and columns for the current level

sf::Color gameBallColors[10]; //declaring the array that contains the colors that are used to fill the balls
sf::Color gameLostColors[8], gameWonColors[8]; //declaring the arrays that contins the colors for the end screen

sf::Font font1(zfSFML.loadFont("Assets/fonts/", "big_space", "otf")); //declaring the font that is used by text

//declaring all the variables of text type that are used in the game
sf::Text scoreText, scoreTitle, inGameOptions, inGameExit, inGameResume, exitTitle, exitAfirmative, exitNegative, exitContent,
	btnPlayTxt, btnCharacterTxt, btnHowToTxt, speedIncreased;

bool showMenu = false; //help us to know if we need to draw the in game menu/end screen or not
bool gameGridGenerated = false; //generate the game grid for the first running of the game screen after a lvl was selected
bool gridGeneratedEffectShow; //help us to know if the screen was animated after the game grid was generated
bool ballsStreak; //after we drop the balls we need to know if it was a streak of at least 3 balls
bool gameLost; //help us to know when the game was lost
bool spriteToRight = true; //variable that is important for the character facing (to right or left)
bool gameEvent; //to know if it is a game notification and draw it
bool lvlTargetHit; //helps us to know if we had previously reached the level target

//ararys of sprites that contains the character images - helps us to animate them
sf::Sprite characterBoy[15], characterGirl[16], characterJack[10], characterNinja[10], menuWalkBoy[15], menuWalkGirl[20], menuRunJack[8], menuSlideJack[10], menuIdleKnight[10],
	menuGlideNinja[10];
sf::Sprite ballSprite(zfSFML.loadSpriteFromTexture("Assets/", "ball", "png")); //the ball sprite

//clock variables that makes the game more realistic
sf::Clock clockRefreshRate, clockGameMenu, inGameEvents, inGameClock;

//declaring the method with its definition only for accesing each other from within
//methods used for creating the chain reaction
void checkBallTop(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballType);
void checkBallBottom(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballType);
void checkBallLeft(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballType);
void checkBallRight(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballType);

//declaring the method with its definition only; this way it could be called by other methods that are above the method
void markBalls(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballsStreak);

//method that generates the gamegrid - the game grid is the one tht stores the values for the balls
void generateGameGrid(int gameGrid[12][17], int linesNo, int columnsNo)
{

	srand(time(NULL)); //making sure that every time are generated random numbers
	for(int i=0; i<linesNo; i++)
	{
		for(int j=0; j<columnsNo; j++)
		{
			if (i<linesNo - 8)
			{
				int randNo;
				do
				{
					randNo = rand() % (5 + gameLvl);
				} while (randNo == 0);
				gameGrid[i][j] = randNo;
			}
			else if (i<linesNo - 7)
			{
				int chance = rand() % 4;
				if (chance == 0)
				{
					gameGrid[i][j] = 0;
				}
				else
				{
					gameGrid[i][j] = rand() % (4 + gameLvl) + 1;
				}
			}
			else if (i<linesNo - 6)
			{
				int chance = rand() % 3;
				if (gameGrid[i-1][j] == 0 || chance == 0)
				{
					gameGrid[i][j] = 0;
				}
				else
				{
					gameGrid[i][j] = rand() % (4 + gameLvl) + 1;
				}
			}
			else
			{
				//adding the empty spaces
				gameGrid[i][j] = 0;
			}
		}
	}

}

//method that add a row of random balls at every X seconds
void addRow(int gameGrid[12][17], int linesNo, int columnsNo)
{

	//moving the rows with one row below
	for(int i=linesNo - 1; i>0; i--)
	{
		for(int j=0; j<columnsNo; j++)
		{
			gameGrid[i][j] = gameGrid[i - 1][j];
		}
	}
	srand(time(NULL)); //making sure that every time are generated random numbers
	//adding a new row
	for(int i=0; i<columnsNo; i++)
	{
		int randNo;
		do
		{
			randNo = rand() % (5 + gameLvl);
		} while (randNo == 0);
		gameGrid[0][i] = randNo;
	}

}

//method that generates the ball colors for each lvl
void generateGameBallColors(sf::Color ballColors[11], int colors)
{
	//red
	ballColors[0] = sf::Color(213,0,0);
	//pink
	ballColors[1] = sf::Color(245,0,87);
	//purple
	ballColors[2] = sf::Color(170,0,255);
	//indigo
	ballColors[3] = sf::Color(49,27,146);
	//blue
	ballColors[4] = sf::Color(0,229,255);
	//teal
	ballColors[5] = sf::Color(0,230,118);
	//green
	ballColors[6] = sf::Color(118,255,3);
	//yellow
	ballColors[7] = sf::Color(255,255,0);
	//orange
	ballColors[8] = sf::Color(255,160,0);
	//brown
	ballColors[9] = sf::Color(62,39,35);
	//black - not in use
	ballColors[10] = sf::Color(0, 0, 0);
}

//method that draw the character
void drawCharacter(int type, int characterX, int characterY)
{
	//drawing the different types of characters
	if(userCharacter == 1)
	{
		characterBoy[characterAnimation].setPosition(objectSize*characterY + (window.getSize().x/2 - objectSize*levelColumns/2), 40*characterX);
		if(spriteToRight)
		{
			characterBoy[characterAnimation].setOrigin(30, 430);
			characterBoy[characterAnimation].setScale(0.2, 0.2);
		}
		else
		{
			characterBoy[characterAnimation].setOrigin(235, 430);
			characterBoy[characterAnimation].setScale(-0.2, 0.2);
		}
		window.draw(characterBoy[characterAnimation]);

		if(clockRefreshRate.getElapsedTime().asMilliseconds() > 50)
		{
			characterAnimation++;
			clockRefreshRate.restart();
		}
		if(characterAnimation == 7) characterAnimation = 0;
	}
	else if(userCharacter == 2)
	{
		characterGirl[characterAnimation].setPosition(objectSize*characterY + (window.getSize().x/2 - objectSize*levelColumns/2), 40*characterX);
		if(spriteToRight)
		{
			characterGirl[characterAnimation].setOrigin(147, 400);
			characterGirl[characterAnimation].setScale(0.23, 0.23);
		}
		else
		{
			characterGirl[characterAnimation].setOrigin(325, 400);
			characterGirl[characterAnimation].setScale(-0.23, 0.23);
		}
		window.draw(characterGirl[characterAnimation]);

		if(clockRefreshRate.getElapsedTime().asMilliseconds() > 50)
		{
			characterAnimation++;
			clockRefreshRate.restart();
		}
		if(characterAnimation == 16) characterAnimation = 0;
	}
	else if(userCharacter == 3)
	{
		menuIdleKnight[characterAnimation].setPosition(objectSize*characterY + (window.getSize().x/2 - objectSize*levelColumns/2), 40*characterX);
		if(spriteToRight)
		{
			menuIdleKnight[characterAnimation].setOrigin(105, 570);
			menuIdleKnight[characterAnimation].setScale(0.15, 0.15);
		}
		else
		{
			menuIdleKnight[characterAnimation].setOrigin(375, 570);
			menuIdleKnight[characterAnimation].setScale(-0.15, 0.15);
		}
		window.draw(menuIdleKnight[characterAnimation]);

		if(clockRefreshRate.getElapsedTime().asMilliseconds() > 50)
		{
			characterAnimation++;
			clockRefreshRate.restart();
		}
		if(characterAnimation == 10) characterAnimation = 0;
	}
	else if(userCharacter == 4)
	{
		characterJack[characterAnimation].setPosition(objectSize*characterY + (window.getSize().x/2 - objectSize*levelColumns/2), 40*characterX);
		if(spriteToRight)
		{
			characterJack[characterAnimation].setOrigin(70, 650);
			characterJack[characterAnimation].setScale(0.14, 0.14);
		}
		else
		{
			characterJack[characterAnimation].setOrigin(360, 650);
			characterJack[characterAnimation].setScale(-0.14, 0.14);
		}
		window.draw(characterJack[characterAnimation]);

		if(clockRefreshRate.getElapsedTime().asMilliseconds() > 50)
		{
			characterAnimation++;
			clockRefreshRate.restart();
		}
		if(characterAnimation == 10) characterAnimation = 0;
	}
	else if(userCharacter == 5)
	{
		characterNinja[characterAnimation].setPosition(objectSize*characterY + (window.getSize().x/2 - objectSize*levelColumns/2), 40*characterX);
		if(spriteToRight)
		{
			characterNinja[characterAnimation].setOrigin(30, 380);
			characterNinja[characterAnimation].setScale(0.22, 0.22);
		}
		else
		{
			characterNinja[characterAnimation].setOrigin(221, 380);
			characterNinja[characterAnimation].setScale(-0.22, 0.22);
		}
		window.draw(characterNinja[characterAnimation]);

		if(clockRefreshRate.getElapsedTime().asMilliseconds() > 50)
		{
			characterAnimation++;
			clockRefreshRate.restart();
		}
		if(characterAnimation == 10) characterAnimation = 0;
	}
	
	//draw the blls that are in the user hands
	sf::Sprite ballSpriteInHand(zfSFML.loadSpriteFromTexture("Assets/", "ball", "png"));
	ballSpriteInHand.setScale(0.25, 0.25);
	ballSpriteInHand.setColor(gameBallColors[ballsInHandType - 1]);
	srand(time(NULL)); //making sure that every time are generated random numbers
	if(ballsInHandNo > 3) ballsInHandNo = 3;
	for(int i=ballsInHandNo - 1; i>=0; i--)
	{
		int movement;
		if(i!=0)
		{
			movement = 8 - i*2 + rand()%(i*4);
		}
		else
		{
			movement = 8;
		}
		ballSpriteInHand.setPosition(objectSize*characterY + (window.getSize().x/2 - objectSize*levelColumns/2) + movement, 40*characterX - 25 * (i + 1) + 2 * i);
		window.draw(ballSpriteInHand);
	}
}

//method that draws the pointers that help us to know: where will the ball fall/which ball to take
void drawPointers(int characterX, int characterY, int gameGrid[12][17])
{	
	sf::CircleShape circle;
	circle.setRadius(7);
	sf::RectangleShape menuSqr;
	menuSqr.setSize(sf::Vector2f(100, 100));
	menuSqr.setScale(0.14, 0.35);
	if(ballsInHandNo>0)
	{
		//in case that we have balls in hand we color the pointers with the balls color
		menuSqr.setFillColor(gameBallColors[ballsInHandType - 1]);
		circle.setFillColor(gameBallColors[ballsInHandType - 1]);
	}
	else
	{
		//otherwise we color them with a dark color
		menuSqr.setFillColor(sf::Color(43, 43, 43));
		circle.setFillColor(sf::Color(43, 43, 43));
	}
	for(int i = levelLines - 2; i>=0; i--)
	{
		if (gameGrid[i -1][characterY] != 0 && gameGrid[i][characterY] == 0 || i == 0 && gameGrid[i][characterY] == 0)
		{
			menuSqr.setScale(0.14, 0.30);
			menuSqr.setPosition(objectSize*characterY + 13 + (window.getSize().x/2 - objectSize*levelColumns/2), objectSize*i + 22);
			circle.setPosition(objectSize*characterY + 13 + (window.getSize().x/2 - objectSize*levelColumns/2), objectSize*i + 17);
			window.draw(menuSqr);
			window.draw(circle);
		}
		else if (gameGrid[i][characterY] == 0)
		{
			menuSqr.setPosition(objectSize*characterY + 13 + (window.getSize().x/2 - objectSize*levelColumns/2), objectSize*i + 15);
			window.draw(menuSqr);
		}
	}
}

//get the first row where it is a ball and it's on the same column as the character
int getBallX(int gameGrid[12][17], int characterY, int gameLines)
{
	int i = gameLines;
	bool found = false;
	while(i>=1 && !found)
	{
		if(gameGrid[i - 1][characterY] != 0)
		{
			found = true;
		}
		i--;
	}
	return i;
}

//get the number of balls that are the same and are on the same column
int getSameBalls(int gameGrid[12][17], int ballY, int ballX)
{
	int i = ballX;
	int ballsNo = 1;
	int ballType = gameGrid[i][ballY];
	bool stop = false;
	while(i>=0 && !stop)
	{
		if(gameGrid[i - 1][ballY] == ballType)
		{
			ballsNo++;
			i--;
		}
		else
		{
			stop = true;
		}
	}
	return ballsNo;
}

//droping the balls from the character hands to the game board
void throwBalls(int gameGrid[12][17], int characterY, int gameLines)
{
	int i = gameLines - 1;
	bool found = false;
	while(i>=1 && !found)
	{
		if(gameGrid[i - 1][characterY] != 0)
		{
			found = true;
		}
		else
		{
			i--;
		}
	}
	if (found || i == 0)
	{
		int startLine = i;
		while(startLine < gameLines && ballsInHandNo > 0 && ballsInHandType != 0)
		{
			gameGrid[startLine][characterY] = ballsInHandType;
			startLine++;
			ballsInHandNo--;
			if (ballsInHandNo == 0 || startLine == gameLines)
			{
				ballsInHandType = 0;
				ballsInHandNo = 0;
			}
		}
		if(ballsStreak)
		{
			ballsInHandType = 0;
			ballsInHandNo = 0;
		}
	}
}

//getting the balls from the game board
void getBalls(int gameGrid[12][17], int characterY, int gameLines)
{
	int i = gameLines - 1;
	bool exit = false;
	while(i >= 0 && !exit)
	{
		if(gameGrid[i][characterY] != 0)
		{
			if (ballsInHandType == 0)
			{
				ballsInHandType = gameGrid[i][characterY];
				gameGrid[i][characterY] = 0;
				ballsInHandNo++;
			}
			else
			{
				if(gameGrid[i][characterY] == ballsInHandType)
				{
					gameGrid[i][characterY] = 0;
					ballsInHandNo++;
				}
				else 
				{
					exit = true;
				}
			}
		}
		i--;
	}
}

//checking if the game i lost - at least a ball is on the last row
bool checkGameLost(int gameGrid[12][17], int linesNo, int columnsNo)
{
	bool lost = false;
	int i = 0;
	while(!lost && i<columnsNo) 
	{
		if (gameGrid[linesNo - 1][i] != 0) 
		{
			lost = true;
		}
		i++;
	}
	return lost;

}

//get the number of balls that are the same
int isAStreak(int column, int line, int gameGrid[12][17])
{
	int ballsNo = 1;
	int ballType = gameGrid[line][column];
	int i = line - 1;
	while(gameGrid[i][column] == ballType && i>=0) 
	{
		i--;
		ballsNo++;
	}
	i = line + 1;
	while(gameGrid[i][column] == ballType && i<levelLines) 
	{
		i++;
		ballsNo++;
	}

	return ballsNo;
}

//deleting the empty space that are within the balls
void checkEmptySpaces(int gameGrid[12][17], int linesNo, int columnsNo)
{
	for(int i = 0; i<columnsNo; i++) 
	{
		for(int j = linesNo - 1; j>=1; j--) 
		{
			if(gameGrid[j][i] != 0 && gameGrid[j - 1][i] == 0) 
			{
				gameGrid[j - 1][i] = gameGrid[j][i];
				gameGrid[j][i] = 0;
				if(gameGrid[j-1][i] != gameGrid[j+1][i]) 
				{
					int streak = isAStreak(i, j-1, gameGrid);
					if(streak >= 3) 
					{
						markBalls(gameGrid, linesNo, columnsNo, i, j-1, streak);
					}
				}
				j = linesNo - 1;
			}
		}
	}
}

//check same ball type at the top of the current ball
void checkBallTop(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballType)
{
	if(ballX > 0) 
	{
		ballX -= 1;
		int cBallType = gameGrid[ballX][ballY];
		if(cBallType == ballType) 
		{
			gameGrid[ballX][ballY] = ballType * -1;
			checkBallTop(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
			checkBallLeft(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
			checkBallRight(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
		}
	}
}

//check same ball type at the bottom of the current ball
void checkBallBottom(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballType)
{
	if(ballX < linesNo - 1) 
	{
		ballX += 1;
		int cBallType = gameGrid[ballX][ballY];
		if(cBallType == ballType) 
		{
			gameGrid[ballX][ballY] = ballType * -1;
			checkBallBottom(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
			checkBallLeft(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
			checkBallRight(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
		}
	}
}

//check same ball type at the left of the current ball
void checkBallLeft(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballType)
{
	if(ballY > 0) 
	{
		ballY -= 1;
		int cBallType = gameGrid[ballX][ballY];
		if(cBallType == ballType) 
		{
			gameGrid[ballX][ballY] = ballType * -1;
			checkBallBottom(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
			checkBallTop(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
			checkBallLeft(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
		}
	}
}

//check same ball type at the right of the current ball
void checkBallRight(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballType)
{
	if(ballY < columnsNo - 1) 
	{
		ballY += 1;
		int cBallType = gameGrid[ballX][ballY];
		if(cBallType == ballType) 
		{
			gameGrid[ballX][ballY] = ballType * -1;
			checkBallBottom(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
			checkBallTop(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
			checkBallRight(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
		}
	}
}

//remove the balls that needs to be destroyed due to the chain reaction
void removeAllBalls(int gameGrid[12][17], int linesNo, int columnsNo, int ballType)
{
	int score = 0;
	for(int i = 0; i<linesNo; i++) 
	{
		for(int j=0; j<columnsNo; j++) 
		{
			if(gameGrid[i][j] * -1 == ballType) 
			{
				gameGrid[i][j] = 0;
				score++;
			}
		}
	}
	lvlScore += score*40;
}

//detect the balls that are the same and cause chain reaction
void markBalls(int gameGrid[12][17], int linesNo, int columnsNo, int ballY, int ballX, int ballsStreak)
{
	int ballType = gameGrid[ballX][ballY];
	if(getSameBalls(gameGrid, ballY, ballX) >= 3 || ballsStreak >= 3) 
	{
		gameGrid[ballX][ballY] = ballType * -1;
		checkBallTop(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
		checkBallBottom(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
		checkBallLeft(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
		checkBallRight(gameGrid, linesNo, columnsNo, ballY, ballX, ballType);
		removeAllBalls(gameGrid, linesNo, columnsNo, ballType);
		checkEmptySpaces(gameGrid, linesNo, columnsNo);
	}

}

//method that checks if a level is unlocked
bool isLvlUnlocked(int lvl) 
{
	return lvl <= lvlUnlocked;
}

//method that draws the animated background of the game menu
void drawGameMenuBg()
{
	
	menuIdleKnight[stateKnight1].setScale(0.2, 0.2);
	menuIdleKnight[stateKnight1].setPosition(50, 300);
	window.draw(menuIdleKnight[stateKnight1]);

	menuIdleKnight[stateKnight2].setScale(0.22, 0.22);
	menuIdleKnight[stateKnight2].setPosition(10, 310);
	window.draw(menuIdleKnight[stateKnight2]);

	menuGlideNinja[stateNinja1].setScale(-0.22, 0.22);
	menuGlideNinja[stateNinja1].setPosition(400, 20);
	window.draw(menuGlideNinja[stateNinja1]);

	menuGlideNinja[stateNinja2].setScale(-0.24, 0.24);
	menuGlideNinja[stateNinja2].setPosition(340, 70);
	window.draw(menuGlideNinja[stateNinja2]);

	menuGlideNinja[stateNinja3].setScale(-0.26, 0.26);
	menuGlideNinja[stateNinja3].setPosition(440, 100);
	window.draw(menuGlideNinja[stateNinja3]);

	menuWalkGirl[stateGirl].setScale(0.26, 0.26);
	menuWalkGirl[stateGirl].setPosition(345, 325);
	window.draw(menuWalkGirl[stateGirl]);

	menuWalkBoy[stateBoy].setScale(0.28, 0.28);
	menuWalkBoy[stateBoy].setPosition(330, 310);
	window.draw(menuWalkBoy[stateBoy]);

	menuSlideJack[stateJack1].setScale(0.18, 0.18);
	menuSlideJack[stateJack1].setPosition(570, 340);
	window.draw(menuSlideJack[stateJack1]);

	ballSprite.setScale(0.4, 0.4);
	ballSprite.setColor(sf::Color(7, 250, 98));
	ballSprite.setPosition(655, 400);
	window.draw(ballSprite);

	menuRunJack[stateJack2].setScale(-0.18, 0.18);
	menuRunJack[stateJack2].setPosition(780, 310);
	window.draw(menuRunJack[stateJack2]);

	if(clockRefreshRate.getElapsedTime().asMilliseconds() > 100)
	{
		stateKnight1++;
		stateKnight2++;
		stateNinja1++;
		stateNinja2++;
		stateNinja3++;
		stateBoy++;
		stateGirl++;
		stateJack1++;
		stateJack2++;
		clockRefreshRate.restart();
	}
	
	if(stateKnight1 == 10) stateKnight1 = 0;
	if(stateKnight2 == 10) stateKnight2 = 0;
	if(stateNinja1 == 10) stateNinja1 = 0;
	if(stateNinja2 == 10) stateNinja2 = 0;
	if(stateNinja3 == 10) stateNinja3 = 0;
	if(stateBoy == 15) stateBoy = 0;
	if(stateGirl == 20) stateGirl = 0;
	if(stateJack1 == 10) stateJack1 = 0;
	if(stateJack2 == 8) stateJack2 = 0;

}

//method that draws the game menu
void drawGameMenu()
{
	btnPlayTxt.setString("PLAY");
	btnPlayTxt.setPosition(690, 40);
	btnPlayTxt.setCharacterSize(30);
	btnPlayTxt.setFillColor(sf::Color::White);
	btnPlayTxt.setOrigin(btnPlayTxt.getGlobalBounds().width/2, btnPlayTxt.getGlobalBounds().height/2);

	sf::RectangleShape btnPlayL1, btnPlayL2, btnPlayL3;

	btnPlayL1.setSize(sf::Vector2f(btnPlayTxt.getLocalBounds().width + 30, btnPlayTxt.getLocalBounds().height  + 20));
	btnPlayL2.setSize(sf::Vector2f(btnPlayTxt.getLocalBounds().width + 30, btnPlayTxt.getLocalBounds().height  + 20));
	btnPlayL3.setSize(sf::Vector2f(btnPlayTxt.getLocalBounds().width + 30, btnPlayTxt.getLocalBounds().height  + 20));

	btnPlayL1.setOrigin(btnPlayL1.getGlobalBounds().width/2, btnPlayL1.getGlobalBounds().height/2);
	btnPlayL2.setOrigin(btnPlayL2.getGlobalBounds().width/2, btnPlayL2.getGlobalBounds().height/2);
	btnPlayL3.setOrigin(btnPlayL3.getGlobalBounds().width/2, btnPlayL3.getGlobalBounds().height/2);
	
	sf::Color clrL1(63,81,181), clrL2(49,27,146);
	btnPlayL1.setFillColor(clrL1);
	btnPlayL2.setFillColor(clrL2);
	btnPlayL3.setFillColor(clrL2);
	
	btnPlayL1.setPosition(690, 50);
	sf::IntRect btnPlayRect(btnPlayL1.getPosition().x - btnPlayL1.getGlobalBounds().width / 2, btnPlayL1.getPosition().y - btnPlayL1.getGlobalBounds().height / 2,
		btnPlayL1.getGlobalBounds().width, btnPlayL1.getGlobalBounds().height);

	if (btnPlayRect.contains(sf::Mouse::getPosition(window))) 
	{
		btnPlayL2.setPosition(685, 55);
		btnPlayL3.setPosition(695, 45);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			currentScreen = SCENE_SELECT_LVL;
			clockRefreshRate.restart();
		}
	} 
	else 
	{
		btnPlayL2.setPosition(675, 60);
		btnPlayL3.setPosition(705, 40);
	}
	
	window.draw(btnPlayL2);
	window.draw(btnPlayL3);
	window.draw(btnPlayL1);

	window.draw(btnPlayTxt);

	btnCharacterTxt.setString("CHARACTER");
	btnCharacterTxt.setPosition(690, 110);
	btnCharacterTxt.setCharacterSize(30);
	btnCharacterTxt.setFillColor(sf::Color::White);
	btnCharacterTxt.setOrigin(btnCharacterTxt.getGlobalBounds().width/2, btnCharacterTxt.getGlobalBounds().height/2);
	
	sf::RectangleShape btnCharactersL1, btnCharactersL2, btnCharactersL3;

	btnCharactersL1.setSize(sf::Vector2f(btnCharacterTxt.getLocalBounds().width + 30, btnCharacterTxt.getLocalBounds().height  + 20));
	btnCharactersL2.setSize(sf::Vector2f(btnCharacterTxt.getLocalBounds().width + 30, btnCharacterTxt.getLocalBounds().height  + 20));
	btnCharactersL3.setSize(sf::Vector2f(btnCharacterTxt.getLocalBounds().width + 30, btnCharacterTxt.getLocalBounds().height  + 20));
	
	btnCharactersL1.setFillColor(clrL1);
	btnCharactersL2.setFillColor(clrL2);
	btnCharactersL3.setFillColor(clrL2);
	
	btnCharactersL1.setOrigin(btnCharactersL1.getGlobalBounds().width/2, btnCharactersL1.getGlobalBounds().height/2);
	btnCharactersL2.setOrigin(btnCharactersL2.getGlobalBounds().width/2, btnCharactersL2.getGlobalBounds().height/2);
	btnCharactersL3.setOrigin(btnCharactersL3.getGlobalBounds().width/2, btnCharactersL3.getGlobalBounds().height/2);
	
	btnCharactersL1.setPosition(690, 120);
	sf::IntRect btnCharactersRect(btnCharactersL1.getPosition().x - btnCharactersL1.getGlobalBounds().width / 2, btnCharactersL1.getPosition().y - btnCharactersL1.getGlobalBounds().height / 2,
		btnCharactersL1.getGlobalBounds().width, btnCharactersL1.getGlobalBounds().height);

	if (btnCharactersRect.contains(sf::Mouse::getPosition(window))) 
	{
		btnCharactersL2.setPosition(685, 125);
		btnCharactersL3.setPosition(695, 115);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			currentScreen = SCENE_OPTIONS_SCREEN;
			clockRefreshRate.restart();
		}
	} 
	else 
	{
		btnCharactersL2.setPosition(675, 130);
		btnCharactersL3.setPosition(705, 110);
	}

	window.draw(btnCharactersL2);
	window.draw(btnCharactersL3);
	window.draw(btnCharactersL1);

	window.draw(btnCharacterTxt);

	btnCharacterTxt.setString("HOW TO");
	btnCharacterTxt.setPosition(690, 180);
	btnCharacterTxt.setCharacterSize(30);
	btnCharacterTxt.setFillColor(sf::Color::White);
	btnCharacterTxt.setOrigin(btnCharacterTxt.getGlobalBounds().width/2, btnCharacterTxt.getGlobalBounds().height/2);

	btnCharactersL1.setSize(sf::Vector2f(btnCharacterTxt.getLocalBounds().width + 30, btnCharacterTxt.getLocalBounds().height  + 20));
	btnCharactersL2.setSize(sf::Vector2f(btnCharacterTxt.getLocalBounds().width + 30, btnCharacterTxt.getLocalBounds().height  + 20));
	btnCharactersL3.setSize(sf::Vector2f(btnCharacterTxt.getLocalBounds().width + 30, btnCharacterTxt.getLocalBounds().height  + 20));
	
	btnCharactersL1.setFillColor(clrL1);
	btnCharactersL2.setFillColor(clrL2);
	btnCharactersL3.setFillColor(clrL2);
	
	btnCharactersL1.setOrigin(btnCharactersL1.getGlobalBounds().width/2, btnCharactersL1.getGlobalBounds().height/2);
	btnCharactersL2.setOrigin(btnCharactersL2.getGlobalBounds().width/2, btnCharactersL2.getGlobalBounds().height/2);
	btnCharactersL3.setOrigin(btnCharactersL3.getGlobalBounds().width/2, btnCharactersL3.getGlobalBounds().height/2);
	
	btnCharactersL1.setPosition(690, 190);
	sf::IntRect btnC2haractersRect(btnCharactersL1.getPosition().x - btnCharactersL1.getGlobalBounds().width / 2, btnCharactersL1.getPosition().y - btnCharactersL1.getGlobalBounds().height / 2,
		btnCharactersL1.getGlobalBounds().width, btnCharactersL1.getGlobalBounds().height);

	if (btnC2haractersRect.contains(sf::Mouse::getPosition(window))) 
	{
		btnCharactersL2.setPosition(685, 195);
		btnCharactersL3.setPosition(695, 185);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			currentScreen = SCENE_HOW_TO_SCREEN;
			clockRefreshRate.restart();
		}
	} 
	else 
	{
		btnCharactersL2.setPosition(675, 200);
		btnCharactersL3.setPosition(705, 180);
	}

	window.draw(btnCharactersL2);
	window.draw(btnCharactersL3);
	window.draw(btnCharactersL1);

	window.draw(btnCharacterTxt);

}

//method that draws the in-game menu (the game menu/end screen)
void drawInGameMenu()
{
	auto mouse_pos = sf::Mouse::getPosition(window);
	auto translated_pos = window.mapPixelToCoords(mouse_pos);

	if(clockGameMenu.getElapsedTime().asMicroseconds() > 10) 
	{
		if(menuSquares<levelColumns*levelLines / 2) 
		{
			menuSquares++;
		}
		clockGameMenu.restart();
	}
	sf::RectangleShape menuSqr;
	menuSqr.setSize(sf::Vector2f(40, 40));
	menuSqr.setFillColor(sf::Color(43, 43, 43));
	
	for(int i=0; i<levelLines; i++)
	{
		for(int j=0; j<levelColumns; j++) 
		{
			int noSquares;
			if(i>0) 
			{
				noSquares = i*levelColumns + j;
			} 
			else 
			{
				noSquares = j;
			}
			if(noSquares < menuSquares) 
			{
				int x = objectSize*j + (window.getSize().x/2 - objectSize*levelColumns/2);
				int y = objectSize*i + 10;
				menuSqr.setPosition(x, y);
				window.draw(menuSqr);
				x = objectSize*(levelColumns - j - 1) + (window.getSize().x/2 - objectSize*levelColumns/2);
				y = objectSize*(levelLines - i - 1) + 10;
				menuSqr.setPosition(x, y);
				window.draw(menuSqr);
				if(j<levelLines)
				{
					x = objectSize*i + (window.getSize().x/2 - objectSize*levelColumns/2);
					y = objectSize*j + 10;
					menuSqr.setPosition(x, y);
					window.draw(menuSqr);
					x = objectSize*(levelColumns - i - 1) + (window.getSize().x/2 - objectSize*levelColumns/2);
					y = objectSize*(levelLines - j - 1) + 10;
					menuSqr.setPosition(x, y);
					window.draw(menuSqr);
				}
			}
		}
	}

	if(levelColumns*levelLines / 2 == menuSquares && optionSelected == 0) 
	{
		scoreText.setString("SCORE:");
		scoreText.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 40);
		scoreText.setCharacterSize(30);
		scoreText.setFillColor(sf::Color::White);
		scoreText.setOrigin(scoreText.getGlobalBounds().width/2, 0);
		window.draw(scoreText);

		scoreTitle.setString(to_string(lvlScore));
		scoreTitle.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 62);
		scoreTitle.setCharacterSize(46);
		scoreTitle.setFillColor(sf::Color::White);
		scoreTitle.setOrigin(scoreTitle.getGlobalBounds().width/2, 0);
		window.draw(scoreTitle);

		inGameResume.setString("RESUME");
		inGameResume.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 150);
		inGameResume.setCharacterSize(26);
		if(inGameResume.getGlobalBounds().contains(translated_pos)) 
		{
			inGameResume.setFillColor(sf::Color(255, 255, 255));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) 
			{
				showMenu = false;
			}
		}
		else
		{
			inGameResume.setFillColor(sf::Color(198, 198, 198));
		}
		inGameResume.setOrigin(inGameResume.getGlobalBounds().width/2, 0);
		window.draw(inGameResume);

		inGameOptions.setString("OPTIONS");
		inGameOptions.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 180);
		inGameOptions.setCharacterSize(26);
		if(inGameOptions.getGlobalBounds().contains(translated_pos)) 
		{
			inGameOptions.setFillColor(sf::Color(255, 255, 255));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) 
			{

			}
		} else 
		{
			inGameOptions.setFillColor(sf::Color(198, 198, 198));
		}
		inGameOptions.setOrigin(inGameOptions.getGlobalBounds().width/2, 0);
		window.draw(inGameOptions);

		inGameExit.setString("EXIT");
		inGameExit.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 210);
		inGameExit.setCharacterSize(26);
		if(inGameExit.getGlobalBounds().contains(translated_pos)) 
		{
			inGameExit.setFillColor(sf::Color(255, 255, 255));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) 
			{
				optionSelected = 3;
			}
		} 
		else 
		{
			inGameExit.setFillColor(sf::Color(198, 198, 198));
		}
		inGameExit.setOrigin(inGameExit.getGlobalBounds().width/2, 0);
		window.draw(inGameExit);
	} 
	else if (optionSelected == 3) 
	{
		//exit case
		exitTitle.setString("EXIT");
		exitTitle.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 70);
		exitTitle.setCharacterSize(40);
		exitTitle.setFillColor(sf::Color::White);
		exitTitle.setOrigin(exitTitle.getGlobalBounds().width/2, 0);
		window.draw(exitTitle);

		exitContent.setString("Are you sure you want to exit?");
		exitContent.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 120);
		exitContent.setCharacterSize(18);
		exitContent.setFillColor(sf::Color::White);
		exitContent.setOrigin(exitContent.getGlobalBounds().width/2, 0);
		window.draw(exitContent);

		exitNegative.setString("NO");
		exitNegative.setPosition(objectSize*(levelColumns/2 - 1) + (window.getSize().x/2 - objectSize*levelColumns/2), 160);
		exitNegative.setCharacterSize(18);
		if(exitNegative.getGlobalBounds().contains(translated_pos)) 
		{
			exitNegative.setFillColor(sf::Color(238, 28, 28));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) 
			{
				optionSelected = 0;
			}
		} 
		else 
		{
			exitNegative.setFillColor(sf::Color(198, 198, 198));
		}
		exitNegative.setOrigin(exitNegative.getGlobalBounds().width/2, 0);
		window.draw(exitNegative);

		exitAfirmative.setString("YES");
		exitAfirmative.setPosition(objectSize*(levelColumns/2 + 2) + (window.getSize().x/2 - objectSize*levelColumns/2), 160);
		exitAfirmative.setCharacterSize(18);
		if(exitAfirmative.getGlobalBounds().contains(translated_pos))
		{
			exitAfirmative.setFillColor(sf::Color(28, 238, 77));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) 
			{
				currentScreen = SCENE_GAME_MENU_SCREEN;
				clockRefreshRate.restart();
				gameGridGenerated = false;
				showMenu = false;
				menuSquares = 0;
			}
		} 
		else
		{
			exitAfirmative.setFillColor(sf::Color(198, 198, 198));
		}
		exitAfirmative.setOrigin(exitAfirmative.getGlobalBounds().width/2, 0);
		window.draw(exitAfirmative);
	}
}

//method that hides the in-game menu (the game menu/end screen)
void hideInGameMenu()
{
	if(clockGameMenu.getElapsedTime().asMicroseconds() > 250)
	{
		if(menuSquares>0)
		{
			if(menuSquares == 1) 
			{
				inGameClock.restart();
				optionSelected = 0;
			}
			menuSquares--;
		}
		clockGameMenu.restart();
	}
	sf::RectangleShape menuSqr;
	menuSqr.setSize(sf::Vector2f(40, 40));
	menuSqr.setFillColor(sf::Color(43, 43, 43));

	if(gridGeneratedEffectShow) 
	{
		for(int i=0; i<levelLines; i++) 
		{
			for(int j=0; j<levelColumns; j++) 
			{
				int noSquares;
				if(i>0) 
				{
					noSquares = i*levelColumns + j;
				} 
				else
				{
					noSquares = j;
				}
				if(noSquares < menuSquares)
				{
					int x = objectSize*j + (window.getSize().x/2 - objectSize*levelColumns/2);
					int y = objectSize*i + 10;
					menuSqr.setPosition(x, y);
					window.draw(menuSqr);
				}
			}
		}
		if(menuSquares == 0)
		{
			gridGeneratedEffectShow = false;
		}
	} 
	else 
	{
		for(int i=0; i<levelLines; i++) 
		{
			for(int j=0; j<levelColumns; j++) 
			{
				int noSquares;
				if(i>0) 
				{
					noSquares = i*levelColumns + j;
				}
				else 
				{
					noSquares = j;
				}
				if(noSquares < menuSquares) 
				{
					int x = objectSize*j + (window.getSize().x/2 - objectSize*levelColumns/2);
					int y = objectSize*i + 10;
					menuSqr.setPosition(x, y);
					window.draw(menuSqr);
					x = objectSize*(levelColumns - j - 1) + (window.getSize().x/2 - objectSize*levelColumns/2);
					y = objectSize*(levelLines - i - 1) + 10;
					menuSqr.setPosition(x, y);
					window.draw(menuSqr);
					if(j<levelLines)
					{
						x = objectSize*i + (window.getSize().x/2 - objectSize*levelColumns/2);
						y = objectSize*j + 10;
						menuSqr.setPosition(x, y);
						window.draw(menuSqr);
						x = objectSize*(levelColumns - i - 1) + (window.getSize().x/2 - objectSize*levelColumns/2);
						y = objectSize*(levelLines - j - 1) + 10;
						menuSqr.setPosition(x, y);
						window.draw(menuSqr);
					}
				}
			}
		}
	}
}

//method that draws the end screen
void drawGameLost()
{
	auto mouse_pos = sf::Mouse::getPosition(window);
	auto translated_pos = window.mapPixelToCoords(mouse_pos);

	if(clockGameMenu.getElapsedTime().asSeconds() > 0.5) 
	{
		if(gameLostLines<=levelLines/2)
		{
			gameLostLines++;
		}
		clockGameMenu.restart();
	}
	sf::RectangleShape menuSqr;
	menuSqr.setSize(sf::Vector2f(40, 40));
	if(lvlTargetHit && lvlScore > finishLvlScore[gameLvl - 1]) 
	{
		menuSqr.setFillColor(gameWonColors[gameLostLines]);
	} 
	else 
	{
		menuSqr.setFillColor(gameLostColors[gameLostLines]);
	}
	for(int i = 0; i<levelLines; i++) 
	{
		for(int j = 0; j<levelColumns; j++) 
		{
			if(i < gameLostLines || i >= levelLines - gameLostLines) 
			{
				menuSqr.setPosition(objectSize*j + (window.getSize().x/2 - objectSize*levelColumns/2), objectSize*i + 10);
				window.draw(menuSqr);
			}
		}
	}

	if(gameLostLines == levelLines/2 + 1 && optionSelected == 0) 
	{
		scoreText.setString("FINAL SCORE:");
		scoreText.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 140);
		scoreText.setCharacterSize(30);
		scoreText.setFillColor(sf::Color(238, 28, 28));
		scoreText.setOrigin(scoreText.getGlobalBounds().width/2, 0);
		window.draw(scoreText);

		scoreTitle.setString(to_string(lvlScore));
		scoreTitle.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 162);
		scoreTitle.setCharacterSize(46);
		scoreTitle.setFillColor(sf::Color(238, 28, 28));
		scoreTitle.setOrigin(scoreTitle.getGlobalBounds().width/2, 0);
		window.draw(scoreTitle);

		inGameExit.setString("RETURN HOME");
		inGameExit.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 240);
		inGameExit.setCharacterSize(26);
		if(inGameExit.getGlobalBounds().contains(translated_pos)) 
		{
			inGameExit.setFillColor(sf::Color(255, 255, 255));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				currentScreen = SCENE_GAME_MENU_SCREEN;
				clockRefreshRate.restart();
				gameGridGenerated = false;
				showMenu = false;
				menuSquares = 0;
			}
		} 
		else 
		{
			inGameExit.setFillColor(sf::Color(198, 198, 198));
		}
		inGameExit.setOrigin(inGameExit.getGlobalBounds().width/2, 0);
		window.draw(inGameExit);

		inGameExit.setString("RETRY");
		inGameExit.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 270);
		inGameExit.setCharacterSize(26);
		if(inGameExit.getGlobalBounds().contains(translated_pos)) 
		{
			inGameExit.setFillColor(sf::Color(255, 255, 255));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) 
			{
				clockRefreshRate.restart();
				gameGridGenerated = false;
				showMenu = false;
				menuSquares = 0;
				currentScreen = SCENE_GAME_SCREEN;
			}
		} else 
		{
			inGameExit.setFillColor(sf::Color(198, 198, 198));
		}
		inGameExit.setOrigin(inGameExit.getGlobalBounds().width/2, 0);
		window.draw(inGameExit);

		if(gameLvl<lvlUnlocked && gameLvl <=6) 
		{
			inGameExit.setString("NEXT LEVEL");
			inGameExit.setPosition(objectSize*levelColumns/2 + (window.getSize().x/2 - objectSize*levelColumns/2), 300);
			inGameExit.setCharacterSize(26);
			if(inGameExit.getGlobalBounds().contains(translated_pos)) 
			{
				inGameExit.setFillColor(sf::Color(255, 255, 255));
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) 
				{
					clockRefreshRate.restart();
					gameGridGenerated = false;
					showMenu = false;
					menuSquares = 0;
					gameLvl++;
					currentScreen = SCENE_GAME_SCREEN;
				}
			} 
			else 
			{
				inGameExit.setFillColor(sf::Color(198, 198, 198));
			}
			inGameExit.setOrigin(inGameExit.getGlobalBounds().width/2, 0);
			window.draw(inGameExit);
		}
	}
}

//method that draws the select lvl screen
void drawSelectLvl()
{

	sf::RectangleShape selectLvlBg;
	selectLvlBg.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	selectLvlBg.setFillColor(sf::Color(43, 43, 43));
	window.draw(selectLvlBg);

	inGameExit.setString("BACK");
	inGameExit.setPosition(window.getSize().x/2, 40);
	inGameExit.setCharacterSize(26);
	sf::IntRect btnCharactersRect(inGameExit.getPosition().x - inGameExit.getGlobalBounds().width / 2,
		inGameExit.getPosition().y, inGameExit.getGlobalBounds().width, inGameExit.getGlobalBounds().height * 2);
	if (btnCharactersRect.contains(sf::Mouse::getPosition(window)))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			currentScreen = SCENE_GAME_MENU_SCREEN;
		}
		inGameExit.setFillColor(sf::Color(255, 255, 255));
	}
	else
	{
		inGameExit.setFillColor(sf::Color(198, 198, 198));
	}
	inGameExit.setOrigin(inGameExit.getGlobalBounds().width/2, 0);
	window.draw(inGameExit);
	
	btnLvl.setLocation(150, 200);
	btnLvl.setUnlocked(isLvlUnlocked(1));
	btnLvl.drawBtn(window, "LVL 1", 30, font1);
	if(btnLvl.btnClicked(window)) 
	{
		gameLvl = 1;
		currentScreen = SCENE_GAME_SCREEN;
	}

	btnLvl.setLocation(250, 300);
	btnLvl.setUnlocked(isLvlUnlocked(2));
	btnLvl.drawBtn(window, "LVL 2", 30, font1);
	if(btnLvl.btnClicked(window))
	{
		gameLvl = 2;
		currentScreen = SCENE_GAME_SCREEN;
	}

	btnLvl.setLocation(350, 200);
	btnLvl.setUnlocked(isLvlUnlocked(3));
	btnLvl.drawBtn(window, "LVL 3", 30, font1);
	if(btnLvl.btnClicked(window))
	{
		gameLvl = 3;
		currentScreen = SCENE_GAME_SCREEN;
	}

	btnLvl.setLocation(450, 300);
	btnLvl.setUnlocked(isLvlUnlocked(4));
	btnLvl.drawBtn(window, "LVL 4", 30, font1);
	if(btnLvl.btnClicked(window))
	{
		gameLvl = 4;
		currentScreen = SCENE_GAME_SCREEN;
	}

	btnLvl.setLocation(550, 200);
	btnLvl.setUnlocked(isLvlUnlocked(5));
	btnLvl.drawBtn(window, "LVL 5", 30, font1);
	if(btnLvl.btnClicked(window))
	{
		gameLvl = 5;
		currentScreen = SCENE_GAME_SCREEN;
	}

	btnLvl.setLocation(650, 300);
	btnLvl.setUnlocked(isLvlUnlocked(6));
	btnLvl.drawBtn(window, "LVL 6", 30, font1);
	if(btnLvl.btnClicked(window))
	{
		gameLvl = 6;
		currentScreen = SCENE_GAME_SCREEN;
	}
	
}

//method that shows in game notifications
void drawGameEvent()
{
	sf::RectangleShape gameEventsHolder;
	float sec = inGameEvents.getElapsedTime().asSeconds();
	if (sec < 3.0)
	{
		if(inGameEventType == 1)
		{
			speedIncreased.setString("Speed Increased!");
			speedIncreased.setFillColor(sf::Color::White);
		} 
		else if(inGameEventType == 2) 
		{
			speedIncreased.setString("Level target reached!");
			speedIncreased.setFillColor(sf::Color::Black);
		}
		speedIncreased.setPosition(window.getSize().x / 2, 35);
		speedIncreased.setCharacterSize(25);
		speedIncreased.setOrigin(speedIncreased.getGlobalBounds().width/2 - 2, speedIncreased.getGlobalBounds().height/2);
		
		gameEventsHolder.setOutlineColor(sf::Color(43, 43, 43, 200));
		gameEventsHolder.setOutlineThickness(2);
		gameEventsHolder.setSize(sf::Vector2f(speedIncreased.getLocalBounds().width + 30, speedIncreased.getLocalBounds().height  + 14));
		if(inGameEventType == 1) 
		{
			gameEventsHolder.setFillColor(sf::Color(255,143,0, 200));
		}
		else if(inGameEventType == 2) 
		{
			gameEventsHolder.setFillColor(sf::Color(139,195,74, 220));
		}
		gameEventsHolder.setPosition(window.getSize().x / 2, 40);
		gameEventsHolder.setOrigin(gameEventsHolder.getGlobalBounds().width/2 - 5, gameEventsHolder.getGlobalBounds().height/2);

		window.draw(gameEventsHolder);
		window.draw(speedIncreased);
	} 
	else
	{
		gameEvent = false;
	}
}

//method that draws the how to play screen
void drawHowToScreen()
{

	sf::RectangleShape selectLvlBg;

	selectLvlBg.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	selectLvlBg.setFillColor(sf::Color(43, 43, 43));
	window.draw(selectLvlBg);

	inGameExit.setString("BACK");
	inGameExit.setPosition(window.getSize().x/2, 20);
	inGameExit.setCharacterSize(26);
	sf::IntRect btnCharactersRect(inGameExit.getPosition().x - inGameExit.getGlobalBounds().width / 2,
		inGameExit.getPosition().y, inGameExit.getGlobalBounds().width, inGameExit.getGlobalBounds().height * 2);
	if (btnCharactersRect.contains(sf::Mouse::getPosition(window)))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			currentScreen = SCENE_GAME_MENU_SCREEN;
		}
		inGameExit.setFillColor(sf::Color(255, 255, 255));
	} 
	else
	{
		inGameExit.setFillColor(sf::Color(198, 198, 198));
	}
	inGameExit.setOrigin(inGameExit.getGlobalBounds().width/2, 0);
	window.draw(inGameExit);

	ifstream howTo("howTo.txt");
	string content((istreambuf_iterator<char>(howTo)), (istreambuf_iterator<char>()));
	inGameExit.setString(content);
	inGameExit.setPosition(50, 70);
	inGameExit.setCharacterSize(16);
	inGameExit.setOutlineColor(sf::Color::Black);
	inGameExit.setOutlineThickness(1);
	inGameExit.setFillColor(sf::Color(198, 198, 198));
	window.draw(inGameExit);
	inGameExit.setOutlineThickness(0);

}

//method that draws the option screen - the one where you can choose your character
void drawOptionsScreen()
{

	sf::RectangleShape selectLvlBg, selectedCharacter;
	selectedCharacter.setSize(sf::Vector2f(70, 10));
	selectedCharacter.setFillColor(sf::Color(117,117,117));

	selectLvlBg.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	selectLvlBg.setFillColor(sf::Color(43, 43, 43));
	window.draw(selectLvlBg);

	inGameExit.setString("BACK");
	inGameExit.setPosition(window.getSize().x/2, 40);
	inGameExit.setCharacterSize(26);
	sf::IntRect btnCharactersRect(inGameExit.getPosition().x - inGameExit.getGlobalBounds().width / 2,
		inGameExit.getPosition().y, inGameExit.getGlobalBounds().width, inGameExit.getGlobalBounds().height * 2);
	if (btnCharactersRect.contains(sf::Mouse::getPosition(window))) 
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) 
		{
			currentScreen = SCENE_GAME_MENU_SCREEN;
		}
		inGameExit.setFillColor(sf::Color(255, 255, 255));
	}
	else 
	{
		inGameExit.setFillColor(sf::Color(198, 198, 198));
	}
	inGameExit.setOrigin(inGameExit.getGlobalBounds().width/2, 0);
	window.draw(inGameExit);
	
	MD2 character;
	for(int i=1; i<=5; i++) 
	{
		int x = i*130 - 20;
		int y = window.getSize().y / 2 - 100 + 100 * (i%2);

		if(i == 1)
		{

			characterBoy[0].setPosition(x + 25, y);
			int originX = characterBoy[0].getGlobalBounds().width / 2;
			int originY = characterBoy[0].getGlobalBounds().height / 2;
			characterBoy[0].setOrigin(originX, originY);
			characterBoy[0].setScale(0.3, 0.3);
			window.draw(characterBoy[0]);

			if(character.imgCLicked(window, characterBoy[0])) 
			{
				userCharacter = i;
			}
			
			if(i == userCharacter) 
			{
				selectedCharacter.setPosition(x + 17, y + 130);
				selectedCharacter.setSize(sf::Vector2f(70, 10));
				window.draw(selectedCharacter);
			}

		} else if(i == 2)
		{

			characterGirl[0].setPosition(x, y + 4);
			int originX = characterGirl[0].getGlobalBounds().width / 2;
			int originY = characterGirl[0].getGlobalBounds().height / 2;
			characterGirl[0].setOrigin(originX, originY);
			characterGirl[0].setScale(0.31, 0.31);
			window.draw(characterGirl[0]);

			if(character.imgCLicked(window, characterGirl[0])) 
			{
				userCharacter = i;
			}
			
			if(i == userCharacter)
			{
				selectedCharacter.setPosition(x + 18, y + 130);
				selectedCharacter.setSize(sf::Vector2f(70, 10));
				window.draw(selectedCharacter);
			}

		}
		else if(i == 3) 
		{

			menuIdleKnight[0].setPosition(x, y - 13);
			int originX = menuIdleKnight[0].getGlobalBounds().width / 2;
			int originY = menuIdleKnight[0].getGlobalBounds().height / 2;
			menuIdleKnight[0].setOrigin(originX, originY);
			menuIdleKnight[0].setScale(0.24, 0.24);
			window.draw(menuIdleKnight[0]);

			if(character.imgCLicked(window, menuIdleKnight[0])) 
			{
				userCharacter = i;
			}
			
			if(i == userCharacter) 
			{
				selectedCharacter.setPosition(x - 10, y + 130);
				selectedCharacter.setSize(sf::Vector2f(96, 10));
				window.draw(selectedCharacter);
			}

		}
		else if(i == 4) 
		{

			characterJack[0].setPosition(x, y - 13);
			int originX = characterJack[0].getGlobalBounds().width / 2;
			int originY = characterJack[0].getGlobalBounds().height / 2;
			characterJack[0].setOrigin(originX, originY);
			characterJack[0].setScale(0.21, 0.21);
			window.draw(characterJack[0]);

			if(character.imgCLicked(window, characterJack[0])) 
			{
				userCharacter = i;
			}
			
			if(i == userCharacter) 
			{
				selectedCharacter.setPosition(x - 15, y + 130);
				selectedCharacter.setSize(sf::Vector2f(95, 10));
				window.draw(selectedCharacter);
			}

		} else if(i == 5)
		{

			characterNinja[0].setPosition(x, y + 15);
			int originX = characterNinja[0].getGlobalBounds().width / 2;
			int originY = characterNinja[0].getGlobalBounds().height / 2;
			characterNinja[0].setOrigin(originX, originY);
			characterNinja[0].setScale(0.3, 0.3);
			window.draw(characterNinja[0]);

			if(character.imgCLicked(window, characterNinja[0])) 
			{
				userCharacter = i;
			}

			if(i == userCharacter)
			{
				selectedCharacter.setPosition(x - 16, y + 130);
				selectedCharacter.setSize(sf::Vector2f(90, 10));
				window.draw(selectedCharacter);
			}

		}
	}

}

int main()
{
	//setting the window frame limit to 30 (usually it is 60)
	window.setFramerateLimit(30);

	//setting the game window icon
	sf::Image icon;
	icon.loadFromFile("Assets/magical_drop_2_icon.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//initialise the array that contains each level target
	for(int i=0; i<6; i++)
	{
		finishLvlScore[i] = 3500 + i*750;
	}

	//initialise the array that contains the colors for the end screen when you are loosing - red colors
	gameLostColors[0] = sf::Color(238, 63, 28);
	gameLostColors[1] = sf::Color(238, 63, 28);
	gameLostColors[2] = sf::Color(214, 51, 18);
	gameLostColors[3] = sf::Color(186, 41, 12);
	gameLostColors[4] = sf::Color(153, 31, 7);
	gameLostColors[5] = sf::Color(114, 22, 3);
	gameLostColors[6] = sf::Color(66, 13, 2);
	gameLostColors[7] = sf::Color(43, 43, 43);

	//initialise the array that contains the colors for the end screen when you are winning - green colors
	gameWonColors[0] = sf::Color(139, 195, 74);
	gameWonColors[1] = sf::Color(112, 169, 46);
	gameWonColors[2] = sf::Color(87, 142, 24);
	gameWonColors[3] = sf::Color(67, 106, 21);
	gameWonColors[4] = sf::Color(56, 95, 10);
	gameWonColors[5] = sf::Color(47, 74, 15);
	gameWonColors[6] = sf::Color(50, 70, 27);
	gameWonColors[7] = sf::Color(43, 43, 43);
	
	//set the font for each variable of text type
	speedIncreased.setFont(font1);
	scoreText.setFont(font1);
	scoreTitle.setFont(font1);
	inGameResume.setFont(font1);
	inGameExit.setFont(font1);
	inGameOptions.setFont(font1);
	exitAfirmative.setFont(font1);
	exitNegative.setFont(font1);
	exitContent.setFont(font1);
	exitTitle.setFont(font1);
	btnCharacterTxt.setFont(font1);
	btnPlayTxt.setFont(font1);
	btnHowToTxt.setFont(font1);

	//declare the game board
	//the board where will appear the balls
	sf::RectangleShape gameHolder;
	gameHolder.setOutlineThickness(10);

	//set the ball scale - 40x40 px
	ballSprite.setScale(0.4, 0.4);

	//load the sprites using the ZeoFlow Library
	sf::Sprite bgMenu(zfSFML.loadSpriteFromTexture("Assets/", "bg3", "png"));
	sf::Sprite zeoFlowSprite(zfSFML.loadSpriteFromTexture("Assets/", "zeoflow_logo", "png")); //the zeoflow logo - for splash screen
	sf::Sprite portal(zfSFML.loadSpriteFromTexture("Assets/", "portal", "png")); //the portal that is used at the boundaries of the game board
	sf::Sprite backgroundSprite(zfSFML.loadSpriteFromTexture("Assets/", "background", "png")); //in game background
	backgroundSprite = zfSFML.formatSpriteForBackground(backgroundSprite); //foramting the bg 

	//initialise the arrays that contains the images of different characters
	//helps us to animate them
	string boyCharacter = "idle_";
	for(int i=0; i<15; i++)
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/boy/", boyCharacter + to_string(i + 1), "png"));
		characterBoy[i] = characterIdle;
	}
	string girlCharacter = "idle_";
	for(int i=0; i<16; i++)
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/girl/", girlCharacter + to_string(i + 1), "png"));
		characterGirl[i] = characterIdle;
	}
	string jackCharacter = "idle_";
	for(int i=0; i<10; i++)
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/jack/", jackCharacter + to_string(i + 1), "png"));
		characterJack[i] = characterIdle;
	}
	string ninjaCharacter = "idle_";
	for(int i=0; i<10; i++)
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/ninja/", ninjaCharacter + to_string(i + 1), "png"));
		characterNinja[i] = characterIdle;
	}
	string menuWalkBoyStr = "walk_";
	for(int i=0; i<15; i++) 
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/boy/", menuWalkBoyStr + to_string(i + 1), "png"));
		menuWalkBoy[i] = characterIdle;
	}
	string menuWalkGirlStr = "walk_";
	for(int i=0; i<20; i++)
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/girl/", menuWalkGirlStr + to_string(i + 1), "png"));
		menuWalkGirl[i] = characterIdle;
	}
	string menuRunJackStr = "run_";
	for(int i=0; i<8; i++) 
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/jack/", menuRunJackStr + to_string(i + 1), "png"));
		menuRunJack[i] = characterIdle;
	}
	string menuSlideJackStr = "slide_";
	for(int i=0; i<10; i++) 
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/jack/", menuSlideJackStr + to_string(i + 1), "png"));
		menuSlideJack[i] = characterIdle;
	}
	string menuIdleKnightStr = "idle_";
	for(int i=0; i<10; i++) 
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/knight/", menuIdleKnightStr + to_string(i + 1), "png"));
		menuIdleKnight[i] = characterIdle;
	}
	string menuGlideNinjaStr = "glide_";
	for(int i=0; i<10; i++) 
	{
		sf::Sprite characterIdle(zfSFML.loadSpriteFromTexture("Assets/characters/ninja/", menuGlideNinjaStr + to_string(i + 1), "png"));
		menuGlideNinja[i] = characterIdle;
	}

	int gameGrid[12][17]; //the array that store the game board
	int clockState = 0; //helps us to reset the in-game clock at the first launch
	int characterX, characterY; //the x and y coordinates for the character

	int rangeSec; //the X seconds when we need to add a new row
	bool rowGenerated = false; //helps us to add a new row

	//the background for the splash screen
	sf::RectangleShape splashScreenBg;
	splashScreenBg.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	sf::Color color(16, 16, 16);
	splashScreenBg.setFillColor(color);
	
	//the loop that draws the game
    while (window.isOpen())
    {
        sf::Event event;
		//the loop that checks for different inputs
        while (window.pollEvent(event))
        {
			if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
			{
                window.close();
			} 
			else if(event.type == sf::Event::KeyPressed)
			{
				if(currentScreen == SCENE_GAME_MENU_SCREEN) 
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::P))
					{
						currentScreen = SCENE_SELECT_LVL;
						clockRefreshRate.restart();
					} 
					else if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) 
					{
						currentScreen = SCENE_OPTIONS_SCREEN;
						clockRefreshRate.restart();
					}
				} 
				else if(currentScreen == SCENE_GAME_SCREEN  && !gameLost) 
				{	
					if (event.key.code == 71)
					{
						//left
						spriteToRight = false;
						if (characterY > 0)
						{
							characterY--;
						} 
						else
						{
							characterY = levelColumns - 1;
						}
					}
					else if (event.key.code == 72) 
					{
						//right
						spriteToRight = true;
						if (characterY < levelColumns - 1) 
						{
							characterY++;
						}
						else
						{
							characterY = 0;
						}
					} 
					else if (event.key.code == 73 && !showMenu && menuSquares == 0)
					{
						//up
						int noBalls = ballsInHandNo;
						ballsStreak = ballsInHandNo >=3;
						throwBalls(gameGrid, characterY, levelLines);
						int ballX = getBallX(gameGrid, characterY, levelLines);
						markBalls(gameGrid, levelLines, levelColumns, characterY, ballX, ballsStreak);
					}
					else if (event.key.code == 74 && !showMenu && menuSquares == 0)
					{
						//down
						getBalls(gameGrid, characterY, levelLines);
					}
					else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)) 
					{
						if(menuSquares == 0)
						{
							menuSquares = 0;
							showMenu = true;
						}
					}
				}
			}
		}

		//clear the previous frame window and draw the current frame window
        window.clear();

		if (clockState == 0)
		{
			//first game launch - reset the clock
			inGameClock.restart();
			clockState++;
		}
		if (currentScreen == SCENE_SPLASH_SCREEN)
		{
			//show the splash screen
			float sec = inGameClock.getElapsedTime().asSeconds();
			if (sec < 2.0)
			{
				window.draw(splashScreenBg);
				zeoFlowSprite.setScale(0.15, 0.15);
				zeoFlowSprite = zfSFML.spriteInMiddle(zeoFlowSprite, window.getSize().x, window.getSize().y);
				window.draw(zeoFlowSprite);
			} 
			else
			{
				//after 2 seconds make the splash screen to dissapear
				gameGridGenerated = false;
				currentScreen = SCENE_GAME_MENU_SCREEN;
				clockRefreshRate.restart();
			}
		}
		else if(currentScreen == SCENE_GAME_SCREEN) 
		{

			window.draw(backgroundSprite);
			if (!gameGridGenerated)
			{

				//the game lvl was just created so we need to initiate all the things that helps us to draw the game lvl
				gameGridGenerated = true;
				levelColumns = 5 + gameLvl * 2;
				levelLines = 12;
				generateGameGrid(gameGrid, levelLines, levelColumns);
				generateGameBallColors(gameBallColors, 10);
				gameLost = false;
				lvlScore = 0;
				ballsInHandNo = 0;
				ballsInHandType = 0;
				characterX = levelLines - 1;
				characterY = levelColumns/2;
				inGameClock.restart();
				menuSquares = levelColumns * levelLines;
				gridGeneratedEffectShow = true;
				if(lvlUnlocked == gameLvl) 
				{
					lvlTargetHit = false;
				} 
				else 
				{
					lvlTargetHit = true;
				}

			}
			else 
			{

				//set the 'x' seconds - the seconds that represents wehn to add a new row
				int sec = (int) inGameClock.getElapsedTime().asSeconds();
				if(lvlScore < 1000)
				{
					rangeSec = 7 + gameLvl;
				} 
				else if(lvlScore < 2500)
				{
					if(rangeSec > 6 + gameLvl)
					{
						inGameEvents.restart();
						gameEvent = true;
						inGameEventType = 1;
					}
					rangeSec = 6 + gameLvl;
				}
				else if(lvlScore < 6250)
				{
					if(rangeSec > 5 + gameLvl)
					{
						inGameEvents.restart();
						gameEvent = true;
						inGameEventType = 1;
					}
					rangeSec = 5 + gameLvl;
				} 
				else if(lvlScore < 9000) 
				{
					if(rangeSec > 4 + gameLvl) 
					{
						inGameEvents.restart();
						gameEvent = true;
						inGameEventType = 1;
					}
					rangeSec = 4 + gameLvl;
				} 
				else 
				{
					if(rangeSec > 3 + gameLvl)
					{
						inGameEvents.restart();
						gameEvent = true;
						inGameEventType = 1;
					}
					rangeSec = 3 + gameLvl;
				}

				//add a new row
				if(sec % rangeSec == 0 && !rowGenerated) 
				{
					if(!gameLost && menuSquares == 0)
					{
						addRow(gameGrid, levelLines, levelColumns);
					}
					rowGenerated = true;
				} 
				else if (sec % rangeSec == rangeSec - 1)
				{
					inGameClock.restart();
					rowGenerated = false;
				}

				//check if the game is lost and change the color of the game holder
				if(gameLost)
				{
					if(lvlTargetHit && lvlScore > finishLvlScore[gameLvl - 1])
					{
						gameHolder.setOutlineColor(gameWonColors[gameLostLines]);
					}
					else
					{
						gameHolder.setOutlineColor(gameLostColors[gameLostLines]);
					}
					gameHolder.setFillColor(sf::Color(236, 35, 35, 70));
				} 
				else
				{
					gameHolder.setOutlineColor(sf::Color(43, 43, 43, 255));
					gameHolder.setFillColor(sf::Color(135, 43, 240, 70));
				}
				gameHolder.setSize(sf::Vector2f(objectSize * levelColumns + 3, objectSize * levelLines + 3));
				gameHolder.setPosition((window.getSize().x/2 - objectSize*levelColumns/2) + 1, 11);
				window.draw(gameHolder); //draw the game holder

				//draw game pieces
				for(int i=0; i<levelLines; i++) 
				{
					for(int j=0; j<levelColumns; j++)
					{
						if(gameGrid[i][j] != 0)
						{
							int ballType = gameGrid[i][j]; //get the ball type
							ballSprite.setColor(gameBallColors[ballType - 1]); //set the ball color
							ballSprite.setPosition(objectSize*j + (window.getSize().x/2 - objectSize*levelColumns/2), objectSize*i + 10); //set ball position
							window.draw(ballSprite); //draw the ball
						}
					}
				}

				//draw the pointers
				if(!gameLost && menuSquares == 0) drawPointers(characterX, characterY, gameGrid);

				//draw different screens in game
				if(gameLost)
				{
					drawGameLost();
				} 
				else
				{
					if(showMenu) 
					{
						drawInGameMenu();
					}
					else 
					{
						hideInGameMenu();
					}
				}

				//check if at least one ball is on the last row - game ends
				if(checkGameLost(gameGrid, levelLines, levelColumns) && !gameLost) 
				{
					if(lvlScore >= finishLvlScore[gameLvl - 1] && lvlUnlocked<=gameLvl) 
					{
						lvlUnlocked = gameLvl + 1;
					}
					gameLostLines = 0;
					clockGameMenu.restart();
					gameLost = true;
				}
				if(lvlScore >= finishLvlScore[gameLvl - 1] && !lvlTargetHit) 
				{
					lvlTargetHit = true;
					inGameEvents.restart();
					gameEvent = true;
					inGameEventType = 2;
				}

				//draw the suer character
				drawCharacter(0, characterX, characterY);

				//draw the portals from the game board boundaries
				portal.setScale(0.2, 0.2);
				portal.setPosition(objectSize*-2 + (window.getSize().x/2 - objectSize*levelColumns/2), objectSize*(characterX - 1));
				if(!gameLost) window.draw(portal);
				portal.setScale(0.2, 0.2);
				portal.setPosition(objectSize*levelColumns + (window.getSize().x/2 - objectSize*levelColumns/2) + 13, objectSize*(characterX - 1));
				if(!gameLost) window.draw(portal);

				//show the score if there is no in-game menu
				if(menuSquares == 0 && !gameLost)
				{
					scoreText.setString("Score:");
					scoreText.setPosition(objectSize*levelColumns + 50 + (window.getSize().x/2 - objectSize*levelColumns/2), 10);
					scoreText.setCharacterSize(20);
					scoreText.setFillColor(sf::Color::Black);
					scoreText.setOrigin(scoreText.getGlobalBounds().width/2, 0);
					window.draw(scoreText);
					scoreTitle.setString(to_string(lvlScore));
					scoreTitle.setPosition(objectSize*levelColumns + 50 + (window.getSize().x/2 - objectSize*levelColumns/2), 32);
					scoreTitle.setCharacterSize(22);
					scoreTitle.setFillColor(sf::Color::Black);
					scoreTitle.setOrigin(scoreTitle.getGlobalBounds().width/2, 0);
					window.draw(scoreTitle);
				}

				//draw the game event (in-game notification)
				if(gameEvent)
				{
					drawGameEvent();
				}

			}
		}
		else if(currentScreen == SCENE_GAME_MENU_SCREEN) 
		{
			
			//draw the game menu screen
			window.draw(bgMenu);
			drawGameMenuBg();
			drawGameMenu();

		} 
		else if(currentScreen == SCENE_OPTIONS_SCREEN) 
		{

			//draw the screen that contains all the characters
			drawOptionsScreen();

		} 
		else if(currentScreen == SCENE_SELECT_LVL) 
		{

			//draw the screen that contains all the levels
			//from here we can choose which level we want to play
			drawSelectLvl();

		} 
		else if(currentScreen == SCENE_HOW_TO_SCREEN) 
		{

			//draw the screen that contains the 'how to' about the game
			drawHowToScreen();

		}

		//display the current frame
        window.display();
		
	}

	//for closing the debug window when the game window is closed
	//getchar();

	return 0;
}