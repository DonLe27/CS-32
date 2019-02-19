#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Penelope::doSomething()
{
	int ch;
	//use graph objects moveTo(double x, double y) to move
	if (getWorld()->getKey(ch))
	{
		double xStart = getX();
		double yStart = getY();

		switch (ch)
		{
		case KEY_PRESS_LEFT:
			setDirection(left);
			moveActor(xStart, yStart, xStart - 4, yStart);
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			moveActor(xStart, yStart, xStart + 4, yStart);
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			moveActor(xStart, yStart, xStart, yStart + 4);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			moveActor(xStart, yStart, xStart, yStart - 4);
			break;
		}
	}
}


void MovingActor::moveActor(double xStart, double yStart, double xDest, double yDest)
{
	if (!(m_studentWorld->checkSurroundings(xStart, yStart, xDest, yDest))) //Check that there are no intersections
	{
		moveTo(xDest, yDest);
	}
}