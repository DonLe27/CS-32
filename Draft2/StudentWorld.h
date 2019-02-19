#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include <string>
#include<vector> //Let's make a vector to hold pointers to all our actors
class Penelope; //Added to make Penelope pointer
class Actor; //Added to make vector<Actor*>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	~StudentWorld()
	{
		cleanUp();
	}
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	void fillMap();
	bool checkSurroundings(int xStart, int yStart, int xDest, int yDest);
private:
	Penelope *m_penelope; //Added Penelope pointer
	std::vector<Actor*> m_actors;
	int n_actors;
};

#endif // STUDENTWORLD_H_
