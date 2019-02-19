#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld; //Added for penelope to get a pointer
class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY)
		:GraphObject(imageID, startX, startY) //What to pass in as ID?
	{

	}
	virtual void doSomething() = 0; //Pure virtual function
	virtual ~Actor() { return; }
private:

	
};

class MovingActor : public Actor
	//This is for actors that need to move and check bounding boxes
{
public:
	MovingActor(int imageID, double startX, double startY, StudentWorld * studentWorld)
		:Actor(imageID, startX, startY) 
	{
		m_studentWorld = studentWorld;
	}

	StudentWorld*getWorld() const //Need this to get location of other objects
	{
		return m_studentWorld;
	}
	void moveActor(double xStart, double yStart, double xDest, double yDest);
	virtual void doSomething() { return; } 
	virtual ~MovingActor() { return; }
private:
	StudentWorld *m_studentWorld; 

};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Wall : public Actor
{
public:
	Wall(double startX, double startY)
		:Actor(IID_WALL, startX, startY) //Direction and depth has correct default
	{}
	virtual void doSomething(){ return; }//Walls don't do anything
	virtual ~Wall() { return; }
		
};

class Penelope : public MovingActor
{
public:
	Penelope(double startX, double startY, StudentWorld * studentWorld)
		:MovingActor(IID_PLAYER, startX, startY, studentWorld)
	{
	}
	
	virtual void doSomething();
	virtual ~Penelope() { return; }
private:
	//Make sure only graph objects gets ID and x,y coords

};
#endif // ACTOR_H_
