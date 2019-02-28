#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "Level.h"
class Penelope;
class Goodie;
class StudentWorld; //Added for penelope to get a pointer

////////////////ACTORS//////////////////////////////////////////
class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld * studentworld = nullptr)
		: GraphObject(imageID, startX, startY, dir, depth, size), deadStatus(false) //What to pass in as ID?
	{
		m_world = studentworld;
	}
	virtual void doSomething()
	{
		if (deadStatus == true)
			return; //All actors return if dead. 
	}
	virtual bool zombieTarget() { return false; }
	virtual bool citizenThreat() { return false; }
	virtual bool blocksFlame() const { return true; }
	virtual bool blocksMovement() const { return false; }
	virtual void overlapActivate(Actor* target) { return; }
	virtual bool blocksProjectiles() { return false; }
	virtual ~Actor() { }
	bool isDead() const { return deadStatus; }
	void setDead() {  deadStatus = true; }
	StudentWorld* getWorld() const { return m_world; }
	virtual bool pitFall() { return false; }
	virtual bool getMine() { return false; }
	virtual bool setOffMine() { return false; }
	virtual bool getVaccine() { return false; }
	virtual bool gotBurned()
	{
		deadStatus = true;
		return true;
	}
	virtual void getExit() { return; }
	virtual bool getInfected() { return false; }
	virtual bool getGas() { return false; }
protected:
	virtual bool activateGoodie();


private:
	StudentWorld * m_world;
	bool deadStatus;
};

///////////////////INHERIT FROM ACTOR////////////////////////////////////////////////////
class Wall : public Actor
{
public:
	Wall(double startX, double startY)
		:Actor(IID_WALL, startX, startY) //Direction and depth has correct default
	{ }
	virtual void doSomething() { return; }//Walls don't do anything
	virtual bool gotBurned() { return false; }
	virtual ~Wall() { return; }
	virtual bool blocksMovement() const { return true; }
	virtual bool blocksProjectiles() { return true; }
protected:
};

class ActivatingActor : public Actor
{
public:
	ActivatingActor(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0, StudentWorld * studentworld = nullptr)
		:Actor(imageID, startX, startY, dir, depth, size, studentworld), tick(0)
	{}
	virtual void doSomething() { return; }
	virtual ~ActivatingActor() { return; }
	void incrementTick();
private:
	int tick;

};
///////////////INHERIT FROM ACTIVATINGACTOR////////////////////
class Exit : public ActivatingActor
{
public:
	Exit(double startX, double startY, StudentWorld * studentworld = nullptr)
		:ActivatingActor(IID_EXIT, startX, startY, 0, 1, 1.0, studentworld) //Depth of 1
	{}
	virtual void doSomething() { return; } 
	//Citizens and Player manages interactions
	virtual ~Exit() { return; }
	virtual void overlapActivate(Actor* target);
	virtual bool gotBurned(){ return false; } //Exit's can't get burned
protected:
	virtual bool blocksProjectiles() { return true; }

};

class Pit : public ActivatingActor
{
public:
	Pit(double startX, double startY, StudentWorld* studentworld)
		:ActivatingActor(IID_PIT, startX, startY,0, 0, 1.0, studentworld )
	{}
	virtual void doSomething() { return; }
	//Interactions managed by Agents (the actors that can fall)
	virtual void overlapActivate(Actor* target) { target->pitFall(); }
	virtual ~Pit() { return; }
	virtual bool gotBurned() { return false; }

};

class Vomit : public ActivatingActor
{
public:
	Vomit(double startX, double startY, StudentWorld * studentworld = nullptr)
		:ActivatingActor(IID_VOMIT, startX, startY, 0, 0, 1.0, studentworld)
	{}
	virtual void doSomething();
	virtual ~Vomit() { return; }
	virtual void overlapActivate(Actor* target);
	virtual bool gotBurned() { return false; }
private:

};

class Landmine : public ActivatingActor
{
public:
	Landmine(double startX, double startY, StudentWorld * studentworld)
		:ActivatingActor(IID_LANDMINE, startX, startY, 0, 1, 1.0, studentworld), safetyTicks(30), isActive(false)//Depth of 1
	{}
	virtual void doSomething();
	virtual bool gotBurned();
	virtual void overlapActivate(Actor* target);
	virtual ~Landmine() { return; }
protected:
	void explode();
private:
	int safetyTicks;
	bool isActive;
};

class Flame : public ActivatingActor
{
public:
	Flame(double startX, double startY, StudentWorld * studentworld = nullptr,int dir = 90)
		:ActivatingActor(IID_FLAME, startX, startY, dir, 0, 1.0, studentworld), tick(0)
	{}//By default, flames should point up
	virtual void doSomething();
	virtual ~Flame() { return; }
	virtual bool gotBurned() { return false; }
	virtual void overlapActivate(Actor* target);
private:
	int tick;
};

class Goodie : public ActivatingActor
{
public:
	Goodie(int imageID, double startX, double startY, StudentWorld * studentworld = nullptr)
		:ActivatingActor(imageID, startX, startY, 0, 1, 1.0, studentworld)
	{}
	virtual void doSomething() { return; }
	virtual ~Goodie() { return; }

};
class VaccineGoodie : public Goodie
{
public:
	VaccineGoodie( double startX, double startY, StudentWorld * studentworld = nullptr)
		:Goodie(IID_VACCINE_GOODIE, startX, startY, studentworld)
	{}
	virtual void doSomething() { return; }
	//All interaction managed by player and StudentWorld
	virtual ~VaccineGoodie() { return; }
	virtual void overlapActivate(Actor* target);
};
class GasCanGoodie : public Goodie
{
public:
	GasCanGoodie(double startX, double startY, StudentWorld * studentworld = nullptr)
		:Goodie(IID_GAS_CAN_GOODIE, startX, startY, studentworld)
	{}
	virtual void doSomething() { return; }
	virtual ~GasCanGoodie() { return; }
	//All interactions managed by player and StudentWorld
	virtual void overlapActivate(Actor* target);
};

class LandmineGoodie : public Goodie
{
public:
	LandmineGoodie(double startX, double startY, StudentWorld * studentworld = nullptr)
		:Goodie(IID_LANDMINE_GOODIE, startX, startY, studentworld)
	{}
	virtual void doSomething() { return; }
	virtual ~LandmineGoodie() { return; }
	virtual void overlapActivate(Actor* target);
};


////////////////Agents/////////////////////////////////////////////
class Agent : public Actor
	//This is for actors that need to move and check bounding boxes
{
public:
	Agent(int imageID, double startX, double startY, StudentWorld * studentworld)
		:Actor(imageID, startX, startY, 0, 0, 1.0, studentworld), paralyzed(false)
	{
	}
	virtual bool pitFall() { setDead(); return true; }

	virtual void doSomething();

	virtual ~Agent() { return; }
	virtual bool blocksMovement() const { return true; }
	virtual bool setOffMine() { return true; }
protected:
	bool getParalysis() { return paralyzed; }
	bool checkDeadOrParalyzed();
	void alternateParalysis();
	bool moveToTarget(double cX, double cY, double pX, double pY, double dist);
	bool moveActor(double xStart, double yStart, double xDest, double yDest);
private:
	bool paralyzed;

};
/////////////////////////Zombies//////////////////////////
class Zombie : public Agent
{
public:
	Zombie(double startX, double startY, StudentWorld * studentworld)
		:Agent(IID_ZOMBIE, startX, startY, studentworld), m_movement(0)
	{

	}
	virtual void doSomething();
	
protected:
	virtual bool citizenThreat() { return true; }
	bool randomMove();
	double getMovementPlan() const { return m_movement; }
	void setMovementPlan(double a) { m_movement = a; }
	void vomit();
private:
	double m_movement;
};

class DumbZombie : public Zombie
{
public:
	DumbZombie(double startX, double startY, StudentWorld * studentworld)
		:Zombie(startX, startY, studentworld)
	{

	}
	virtual void doSomething();
	virtual ~DumbZombie();
protected:
	void dropGoodie();
};

class SmartZombie : public Zombie
{
public:
	SmartZombie(double startX, double startY, StudentWorld * studentworld)
		:Zombie(startX, startY, studentworld)
	{

	}
	virtual void doSomething();
	virtual ~SmartZombie();

};

/////////////////Humans//////////////////////////////////////////////////////////////////
class Human : public Agent
{
public:
	Human(int imageID, double startX, double startY, StudentWorld * studentworld)
		:Agent(imageID, startX, startY, studentworld), infectionCount(0), infectionStatus(false)
	{

	}
	int getInfectionCount() const { return infectionCount; }
	virtual void doSomething();
	virtual bool getInfected() { infectionStatus = true; return true; }
protected:
	bool getInfectionStatus() const { return infectionStatus; }
	void setInfectionStatus(bool infection) { infectionStatus = infection; }
	virtual bool zombieTarget() { return true; }
	void resetInfectionCount() { infectionCount = 0; }
private:
	bool infectionStatus;
	int infectionCount;

};

class Citizen : public Human
{
public:
	Citizen(double startX, double startY, StudentWorld * studentworld)
		:Human(IID_CITIZEN, startX, startY, studentworld), free(false), naturalKill(false)
	{
		setInfectionStatus(false);
	}

	virtual void doSomething();
	virtual ~Citizen();
	virtual bool getInfected();
	virtual void getExit() { free = true; setDead(); }
	virtual bool gotBurned();
protected:
	void pickDest(double stay, double up, double down, double right, double left, double& zX, double& zY);
	void moveAway(double x, double y, double xThreat, double yThreat);
private:
	bool free;
	bool naturalKill;
};
class Penelope : public Human
{
public:
	Penelope(double startX, double startY, StudentWorld * studentWorld)
		:Human(IID_PLAYER, startX, startY, studentWorld), m_vaccines(3), m_landmines(3), m_flames(3)
	{
	}
	
	virtual void doSomething();
	virtual ~Penelope();

	// How many vaccines does the object have?
	int getNumVaccines() const { return m_vaccines; }

	// How many flame charges does the object have?
	int getNumFlameCharges() const { return m_flames; }
	// How many landmines does the object have?
	int getNumLandmines() const { return m_landmines; }
protected:
	void useFlamethrower();
	virtual void getExit();
	virtual bool getVaccine() { m_vaccines++; return true; }
	virtual bool getGas() { m_flames += 5; return true; }
	virtual bool getMine() { m_landmines++; return true; }
	void useVaccine();
	void useMine();

private:
	//Make sure only graph objects gets ID and x,y coords
	int m_vaccines;
	int m_flames;
	int m_landmines;
};
#endif // ACTOR_H_
