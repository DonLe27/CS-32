#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h" //Added to make Penelope and other actors
#include <string>
#include<vector>
#include "Level.h" //Used to load specified positions of actors
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
	n_actors = 0;
}

int StudentWorld::init()
{
	//m_penelope = new Penelope(200, 100, this); //Passed in coords and pointer to self
	//m_actors.push_back(new Wall(150, 100));
	fillMap();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	m_penelope->doSomething(); //Penelope can currently move around
	decLives();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	delete m_penelope; //Delete Penelope when done
	while (!m_actors.empty())
	{
		delete m_actors.back();
		m_actors.pop_back();
	}
	if (!m_actors.empty())
		cerr << "m_actors not empty!" << endl;
}

void StudentWorld::fillMap()
{
	Level lev(assetPath());
	string levelFile = "level01.txt"; //Use getLevel() to fill this
	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
		cerr << "Cannot find level01.txt data file" << endl;
	else if (result == Level::load_fail_bad_format)
		cerr << "Your level was improperly formatted" << endl;
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		for (int w = 0; w < LEVEL_WIDTH; w++)
		{
			for (int h = 0; h < LEVEL_HEIGHT; h++)
			{
				Level::MazeEntry ge = lev.getContentsOf(w, h);
				int a_x = w * SPRITE_WIDTH; //Multiplying correctly?
				int a_y = h * SPRITE_HEIGHT;
				switch (ge)
				{
				case Level::wall:
					m_actors.push_back(new Wall(a_x, a_y));
					n_actors++;
					break;
				case Level::player:
					m_penelope = new Penelope(a_x, a_y, this);
					break;
				case Level::empty:
					break;
				default:
					break;
				}
			}
		}
	}
}

bool StudentWorld::checkSurroundings(int xStart, int yStart, int xDest, int yDest) 
{
	int intersections = 0;
	std::vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		int cX = (*it)->getX();
		int cY = (*it)->getY();
		if (cX == xStart && cY == yStart) //Pass when checking object that decided to move
			continue;
		else if (cX > (xDest - (SPRITE_WIDTH-1)) && cX < (xDest + (SPRITE_WIDTH-1))
				&& cY > (yDest - (SPRITE_HEIGHT-1)) && cY < (yDest + (SPRITE_HEIGHT-1)))//True if there is an interesection
			return true;
		it++; //Lol forgot this boi the first time
	}
	return false;
}
