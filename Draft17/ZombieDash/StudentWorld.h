#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include "Level.h"
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
	int fillMap();
	bool checkSurroundings(double xStart, double yStart, double xDest, double yDest) const;
	void checkOverlap();
	bool locatePlayer(double x, double y, double& otherX, double& otherY, double& distance);
	bool locateZombieTarget(double x, double y, double& otherX, double& otherY, double& distance);
	bool locateZombie(double x, double y, double& otherX, double& otherY, double& distance);
	void dump(); //Want to test Actors calling functions
	bool addActor(char ID, double a_x, double a_y, int dir=0);
	void removeDeadActors();
	void decrementCitizens() { citizensLeft--; }
	int getCitizens() const { return citizensLeft; }
	void setWonLevel() { wonLevel = true; }
	void setFailedLevel() { wonLevel = false; }
	bool getWonLevel() { return wonLevel; }
	bool getFailedLevel() { return failedLevel; }
	void setLevelReset() { levelReset = true; }
	bool checkProjectile(double x, double y); //Check if a flame can be spawned at that location
	bool canSpawn(double x, double y);
	void increaseScoreWrapper(int score);
private:
	Penelope *m_penelope; //Added Penelope pointer
	std::vector<Actor*> m_actors;
	int n_actors;
	Level m_lev;
	bool failedLevel;
	int citizensLeft;
	bool wonLevel;
	bool levelReset;
};

#endif // STUDENTWORLD_H_
