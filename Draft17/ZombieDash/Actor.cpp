#include "Actor.h"
#include "StudentWorld.h"
#include "Level.h"
#include "GameConstants.h" //to use random
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
double distA(double x, double y, double otherX, double otherY);
double distA(double x, double y, double otherX, double otherY)
{
	return sqrt((pow(x - otherX, 2) + pow(y - otherY, 2)));
}
bool Actor::activateGoodie()
{
	getWorld()->increaseScore(50);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	setDead();
	return true;
}

//AGENTS IMPLEMENTATION
bool Agent::moveToTarget(double cX, double cY, double pX, double pY, double dist)
{
	{
		if (cX == pX)
		{
			if (cX < pX)
				return moveActor(cX, cY, cX + dist, cY);
			else
				return moveActor(cX, cY, cX - dist, cY);
		}
		else if (cY == pY)
		{
			if (cY < pY)
				return moveActor(cX, cY, cX, cY + dist);
			else
				return moveActor(cX, cY, cX, cY - dist);
		}
		else
		{
			int rand = randInt(0, 1);
			if (rand == 0) //Moving horizontally
			{
				if (cX < pX)
					return moveActor(cX, cY, cX + dist, cY);
				else
					return moveActor(cX, cY, cX - dist, cY);
			}
			else
				if (cY < pY) //Moving vertically
					return moveActor(cX, cY, cX, cY + dist);
				else
					return moveActor(cX, cY, cX, cY - dist);
		}
	}
}
void Agent::alternateParalysis()
{
	if (paralyzed == true) //Alternate paralysis every tick
		paralyzed = false;
	else
		paralyzed = true;
}
bool Agent::checkDeadOrParalyzed()
{
	if (isDead())
		return true;
	else if (getParalysis() == true)
		return true;
	else
		return false;
}

void Agent::doSomething()
{

}

bool Agent::moveActor(double xStart, double yStart, double xDest, double yDest)
{
	double xDiff = xDest - xStart;
	double yDiff = yDest - yStart;
	if (abs(xDiff) > abs(yDiff))
	{
		if (xDiff > 0)
			setDirection(right);
		else
			setDirection(left);
	}
	else
	{
		if (yDiff > 0)
			setDirection(up);
		else
			setDirection(down);
	}
	if (!(getWorld()->checkSurroundings(xStart, yStart, xDest, yDest))) //Check that there are no intersections
	{
		moveTo(xDest, yDest);
		return true;
	}
	return false;
}
void Human::doSomething()
{
	if (getInfectionStatus())
		infectionCount++;
}

//CITIZEN IMPLEMENTATION
Citizen::~Citizen()
{
	int rand = randInt(1, 10);
	getWorld()->decrementCitizens();
	if (free) //Citizen was freed not infected
	{
		getWorld()->playSound(SOUND_CITIZEN_SAVED);
		getWorld()->increaseScoreWrapper(500);
		return;
	}

	else if (naturalKill) //Citizen was killed by fire or pit, not by infection
	{
		getWorld()->increaseScoreWrapper(-1000);
		return;
	}
	//Played SOUND_ZOMBIE_BORN at infection
	if (rand >= 1 && rand <= 3) //30% of being smart
		getWorld()->addActor('s', getX(), getY());
	else //70% of being dumb
		getWorld()->addActor('d', getX(), getY());
	getWorld()->increaseScoreWrapper(-1000); //Got killed
}

bool Citizen::gotBurned()
{
	setDead();
	naturalKill = true;
	getWorld()->playSound(SOUND_CITIZEN_DIE);
	return true;

}

void Citizen::doSomething()
{
	if (isDead())
		return;
	Human::doSomething();
	Agent::doSomething();
	if (getParalysis() == true)
		return;
	if (getInfectionCount() >= 500)
	{
		setDead();
		getWorld()->playSound(SOUND_ZOMBIE_BORN);
		return;
	}
	double cX = getX();
	double cY = getY();
	double pDist, pX, pY; 
	double max = distA(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
	double zDist = max;
	double zX, zY;
	getWorld()->locatePlayer(getX(), getY(), pX, pY, pDist);
	getWorld()->locateZombie(cX, cY, zX, zY, zDist);
	if ((pDist <= 80) && (pDist < zDist))
		moveToTarget(cX, cY, pX, pY, 2);
	else if (zDist <= 80)//Try to move away from zombie
	{
		moveAway(cX, cY, zX, zY);
		
		double xDest, yDest;
		double zDistUp = max;
		double zDistDown = max;
		double zDistRight = max;
		double zDistLeft = max;
		//Get location furthest from zombie //Didn't finish completely
		/*
		if (!getWorld()->checkSurroundings(cX, cY, cX, cY + 2))
			getWorld()->locateZombie(cX, cY + 2, zX, zY, zDistUp);
		if (!getWorld()->checkSurroundings(cX, cY, cX, cY - 2))
			getWorld()->locateZombie(cX, cY - 2, zX, zY, zDistDown);
		if (!getWorld()->checkSurroundings(cX, cY, cX+2, cY))
			getWorld()->locateZombie(cX + 2, cY, zX, zY, zDistRight);
		if (!getWorld()->checkSurroundings(cX, cY, cX-2, cY))
			getWorld()->locateZombie(cX - 2, cY, zX, zY, zDistLeft);
		pickDest(zDist, zDistUp, zDistDown, zDistRight, zDistLeft, xDest, yDest);
		moveActor(cX, cY, xDest, yDest);
		*/
		

	}
}
void Citizen::moveAway(double x, double y, double xThreat, double yThreat)
{
	//Move horizontally if horizontal difference is less
	double horDiff = abs(x - xThreat);
	double verDiff = abs(y - yThreat);
	if (horDiff <= verDiff)
	{
		if (x < xThreat) //Move left
			moveActor(x, y, x - 2, y);
		else
			moveActor(x, y, x + 2, y);
	}
	else
	{
		if (y < yThreat) //Move down
			moveActor(x, y, x, y-2);
		else
			moveActor(x, y, x, y+2);
	}
	return;
}
bool Citizen::getInfected()
{
	getWorld()->playSound(SOUND_CITIZEN_INFECTED);
	return Human::getInfected();
}
void Citizen::pickDest(double stay,double up, double down, double right, double left, double& zX, double& zY)
{
	//Find the largest distance and set destination to that
	double zDistTemp = stay;
	double destX = zX;
	double destY = zY;
	if (up > zDistTemp)
	{
		zDistTemp = up;
		destX = zX;
		destY = zY + 2;
	}
	if (down > zDistTemp)
	{
		zDistTemp = down;
		destX = zX;
		destY = zY - 2;
	}
	if (right > zDistTemp)
	{
		zDistTemp = right;
		destX = zX+2;
		destY = zY;
	}
	if (left > zDistTemp)
	{
		zDistTemp = left;
		destX = zY-2;
		destY = zY;
	}
	zX = destX;
	zY = destY;
}


//////////////////////PENELOPE IMPLEMENTATION///////////////////////////////////
void Penelope::getExit()
{
	if (getWorld()->getCitizens() == 0)
	{
		getWorld()->setWonLevel();
		setDead();
	}
}
Penelope::~Penelope()
{

}
void Penelope::useFlamethrower()
{
	if (getNumFlameCharges() > 0)
	{
		getWorld()->playSound(SOUND_PLAYER_FIRE);
		double pX = getX();
		double pY = getY();
		Direction dir = getDirection();
		char flame = 'f';
		if (dir == up)
		{
			if (getWorld()->addActor(flame, pX, pY + SPRITE_HEIGHT, dir)) //Only add the next flame if the prev one spawned
			{
				if (getWorld()->addActor(flame, pX, pY + 2 * SPRITE_HEIGHT, dir))
					getWorld()->addActor(flame, pX, pY + 3 * SPRITE_HEIGHT, dir);
			}

		}
		else if (dir == down)
		{
			if (getWorld()->addActor(flame, pX, pY - SPRITE_HEIGHT, dir))
			{
				if (getWorld()->addActor(flame, pX, pY - 2 * SPRITE_HEIGHT, dir))
					getWorld()->addActor(flame, pX, pY - 3 * SPRITE_HEIGHT, dir);
			}

		}
		else if (dir == left)
		{
			if (getWorld()->addActor(flame, pX - SPRITE_HEIGHT, pY,dir))
			{
				if (getWorld()->addActor(flame, pX - 2 * SPRITE_HEIGHT, pY, dir))
					getWorld()->addActor(flame, pX - 3 * SPRITE_HEIGHT, pY, dir);
			}

		}
		else //dir == right
		{
			if (getWorld()->addActor(flame, pX + SPRITE_HEIGHT, pY, dir))
			{
				if (getWorld()->addActor(flame, pX + 2 * SPRITE_HEIGHT, pY, dir))
					getWorld()->addActor(flame, pX + 3 * SPRITE_HEIGHT, pY, dir);
			}
		}
		m_flames--;
	}
}
void Penelope::useVaccine()
{
	if (getNumVaccines() > 0)
	{
		setInfectionStatus(false);
		resetInfectionCount();
		m_vaccines--;
	}
}

void Penelope::doSomething() 
//Ply SOUND_PLAYER_DIED in StudentWorld
{
	if (getInfectionCount() >= 500)
	{
		setDead();
		getWorld()->setFailedLevel();
		return;
	}
	if (isDead())
	{
		return;
	}
	Human::doSomething();
	int ch;
	//use graph objects moveTo(double x, double y) to move
	if (getWorld()->getKey(ch))
	{
		double xStart = getX();
		double yStart = getY();

		switch (ch)
		{
		case KEY_PRESS_LEFT:
			moveActor(xStart, yStart, xStart - 4, yStart);
			break;
		case KEY_PRESS_RIGHT:
			moveActor(xStart, yStart, xStart + 4, yStart);
			break;
		case KEY_PRESS_UP:
			moveActor(xStart, yStart, xStart, yStart + 4);
			break;
		case KEY_PRESS_DOWN:
			moveActor(xStart, yStart, xStart, yStart - 4);
			break;
		case KEY_PRESS_SPACE:
			useFlamethrower();
			break;
		case KEY_PRESS_ENTER:
			useVaccine();
			break;
		case KEY_PRESS_TAB:
			useMine();
			break;
		}
	}
}

void Penelope::useMine()
{
	if (getNumLandmines() > 0)
	{
		m_landmines--;
		getWorld()->addActor('m', getX(), getY());
	}
}


////////////////////////////ZOMBIE IMPLEMENTATION//////////////////////////////////
DumbZombie::~DumbZombie()
{
	dropGoodie();
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	getWorld()->increaseScoreWrapper(1000);
}
SmartZombie::~SmartZombie()
{
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	getWorld()->increaseScoreWrapper(2000);
}

void Zombie::vomit()
{
	double zX = getX();
	double zY = getY();
	double cDist = distA(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
	double cX, cY;
	Direction dir = getDirection();
	int rand = randInt(1, 3);
	char ID = 'v';
	//Get distance of nearest player/citizen from vomit coords and decide if close enough
	if (dir == up)
	{
		getWorld()->locateZombieTarget(zX, zY + SPRITE_HEIGHT, cX, cY, cDist);
		if ((cDist <= 10) && (rand == 1)) //make vomit
		{
			getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			getWorld()->addActor(ID, zX, zY + SPRITE_HEIGHT);
		}
	}
	else if (dir == down)
	{
		getWorld()->locateZombieTarget(zX, zY - SPRITE_HEIGHT, cX, cY, cDist);
		if ((cDist <= 10 ) && (rand == 1)) //make vomit
		{
			getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			getWorld()->addActor(ID, zX, zY - SPRITE_HEIGHT);
		}
	}
	else if (dir == left)
	{
		getWorld()->locateZombieTarget(zX - SPRITE_WIDTH, zY, cX, cY, cDist);
		if ((cDist <= 10 ) && (rand == 1)) //make vomit
		{
			getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			getWorld()->addActor(ID, zX - SPRITE_WIDTH, zY);
		}
	}
	else // (dir == right)
	{
		getWorld()->locateZombieTarget(zX + SPRITE_WIDTH, zY, cX, cY, cDist);
		if ((cDist <= 10 ) && (rand == 1)) //make vomit
		{
			getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			getWorld()->addActor(ID, zX + SPRITE_WIDTH, zY);
		}
	}

}

void Zombie::doSomething()
{

}
bool Zombie::randomMove()
{
	double mX = getX();
	double mY = getY();
	int randDir = randInt(1, 4); //1 = up, 2 = right, 3 = down, 4 = left
	if (randDir == 1)
		return moveActor(mX, mY, mX, mY + 1);
	else if (randDir == 2)
		return moveActor(mX, mY, mX + 1, mY);
	else if (randDir == 3)
		return moveActor(mX, mY, mX, mY - 1);
	else // (randDir == 4)
		return moveActor(mX, mY, mX - 1, mY);
}

void DumbZombie::doSomething()
{
	alternateParalysis();
	if (checkDeadOrParalyzed())
		return;
	Zombie::vomit();
	if (getMovementPlan() <= 0)
		setMovementPlan(randInt(3, 10));
	bool moveSuccess = randomMove();
	if (moveSuccess)
		setMovementPlan(getMovementPlan() - 1);
	else
		setMovementPlan(0);
}
void DumbZombie::dropGoodie()
{
	int rand = randInt(1, 10);
	if (rand == 1)
	{
		double x = getX();
		double y = getY();
		int rand2 = randInt(1, 4);
		if (rand2 == 1) //up
			getWorld()->addActor('a', x, y + SPRITE_HEIGHT);
		else if (rand2 == 2) //down
			getWorld()->addActor('a', x, y - SPRITE_HEIGHT);
		else if (rand2 == 3) //right
			getWorld()->addActor('a', x+SPRITE_WIDTH, y);
		else // (rand2 == 3) //left
			getWorld()->addActor('a', x - SPRITE_WIDTH, y);
	}
}
void SmartZombie::doSomething()
{
	alternateParalysis();
	if (checkDeadOrParalyzed())
		return;
	if (getMovementPlan() <= 0)
		setMovementPlan(randInt(3, 10));
	Zombie::vomit();
	double zX = getX();
	double zY = getY();
	double cDist = distA(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
	double cX, cY;
	bool foundC = getWorld()->locateZombieTarget(zX, zY, cX, cY, cDist);
	bool moveSuccess = false;
	if ((cDist <= 80  &&  foundC)) //Check if nearest citizen is closer to nearest player
	{
			moveSuccess = moveToTarget(zX, zY, cX, cY, 1);
	}
	else //If nobody near enough found, move randomly
		moveSuccess = randomMove();

	if (moveSuccess)
		setMovementPlan(getMovementPlan() - 1);
	else
		setMovementPlan(0);
}

////////////////////////////ACTIVATINGACTORS IMPLEMENTATION/////////////////////////////////////
void ActivatingActor::incrementTick()
{
	tick++;
	if (tick > 2)
	{
		setDead();
		return;
	}
}
void Flame::overlapActivate(Actor* target)
{
	target->gotBurned();
	return;
}
void Vomit::overlapActivate(Actor * target)
{
	target->getInfected();
	return;
}

void Vomit::doSomething()
{	
	if (isDead())
		return;
	ActivatingActor::incrementTick();
	
}
void Flame::doSomething()
{
	if (isDead())
		return;
	ActivatingActor::incrementTick();
}


void Exit::overlapActivate(Actor * target)
{
	target->getExit();
}

//GOODIE IMPLEMENTATION

void VaccineGoodie::overlapActivate(Actor * target)
{
	if (target->getVaccine()) //Only returns true if picked up
		Actor::activateGoodie(); //Increments score and dies
}
void GasCanGoodie::overlapActivate(Actor * target)
{

	if (target->getGas())
		Actor::activateGoodie();
}
void LandmineGoodie::overlapActivate(Actor * target)
{
	if (target->getMine())
		Actor::activateGoodie();
}

/////////////////LANDMINE IMPLEMENTATION//////////////

void Landmine::doSomething()
{
	//Other affects managed by Agents and StudentWorld
	if (isDead())
		return;
	if (!isActive)
	{
		safetyTicks--;
		if (safetyTicks <= 0)
			isActive = true;
	}
}

void Landmine::overlapActivate(Actor * target)
{
	if (target->setOffMine())
	{
		if (isActive)
		{
			explode();
		}
	}
}


bool Landmine::gotBurned()
{
	if (isDead())
		return false;
	explode();
	return true;
}

void Landmine::explode()
{
	setDead();
	double mx = getX();
	double my = getY();
	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
	getWorld()->addActor('f', mx, my);
	getWorld()->addActor('f', mx + SPRITE_WIDTH, my); //East
	getWorld()->addActor('f', mx - SPRITE_WIDTH, my); //West
	getWorld()->addActor('f', mx, my + SPRITE_HEIGHT); //North
	getWorld()->addActor('f', mx, my - SPRITE_HEIGHT); //South
	getWorld()->addActor('f', mx + SPRITE_WIDTH, my + SPRITE_HEIGHT); //NorthEast
	getWorld()->addActor('f', mx - SPRITE_WIDTH, my - SPRITE_HEIGHT); //SouthWest
	getWorld()->addActor('f', mx + SPRITE_WIDTH, my - SPRITE_HEIGHT); //SouthEast
	getWorld()->addActor('f', mx - SPRITE_WIDTH, my + SPRITE_HEIGHT); //NorthWest
	getWorld()->addActor('h', mx, my);
}

