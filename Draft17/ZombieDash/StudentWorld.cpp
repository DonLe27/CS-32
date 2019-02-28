#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h" //Added to make Penelope and other actors
#include <string>
#include<vector>
#include "Level.h" //Used to load specified positions of actors
#include <cmath> //Used to include powers
#include <cctype> //Used for toUpper
using namespace std;



GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_lev(assetPath)
{
	n_actors = 0;
	citizensLeft = 0;
	wonLevel = false;
	failedLevel = false;
	levelReset = false;
	m_penelope = nullptr;
}

int StudentWorld::init()
{
	int filled = fillMap();
	if (filled == -1)
		return GWSTATUS_LEVEL_ERROR;
	if (filled == 1)
		return GWSTATUS_PLAYER_WON;
	else
		return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

	if (m_penelope->isDead())
	{
		if (wonLevel)
		{
			playSound(SOUND_LEVEL_FINISHED);
			setFailedLevel(); //Reset wonLevel
			levelReset = false;
			return GWSTATUS_FINISHED_LEVEL;
		}
		else
		{
			playSound(SOUND_PLAYER_DIE);
			decLives();
			levelReset = false;
			return GWSTATUS_PLAYER_DIED;
		}	
	}
	removeDeadActors();
	string stats = "Score: " + to_string(getScore()); //Two spaces after each label
	stats.append("  Level: " + to_string(getLevel()));
	stats.append("  Lives: " + to_string(getLives()));
	stats.append("  Vacc: " + to_string(m_penelope->getNumVaccines()));
	stats.append("  Flames: " + to_string(m_penelope->getNumFlameCharges()));
	stats.append("  Mines: " + to_string(m_penelope->getNumLandmines()));
	stats.append("  Infected: " + to_string(m_penelope->getInfectionCount()));
	setGameStatText(stats);
	m_penelope->doSomething(); //Penelope can currently move around
	std::vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end()) //Tell all actors to doSomething()
	{
		(*it)->doSomething();
		it++;
	}
	checkOverlap();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	if (m_penelope != nullptr)
		delete m_penelope; //Delete Penelope when done

	for (std::vector< Actor* >::iterator it = m_actors.begin(); it != m_actors.end(); ++it)
	{
		delete (*it); //*it is an Actor*. it is an Actor**
	}
	m_actors.clear();

	/*	
	while (!m_actors.empty())
	{
		delete m_actors.back(); //delete Actor*
		m_actors.pop_back();
	}
		if (!m_actors.empty())
		cerr << "m_actors not empty!" << endl;
	*/
}

int StudentWorld::fillMap()
{
	int currentLevel = getLevel();
	string levelFile = "";
	if (currentLevel <= 9)
		levelFile = "level0" + to_string(currentLevel) + ".txt"; //Use getLevel() to fill this
	else //currentLevel is in the double digits so don't add 0
		levelFile = "level" + to_string(currentLevel) + ".txt";
	Level::LoadResult result = m_lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
	{
		cerr << "Cannot find data file" << endl;
		return 1;
	}
	else if (result == Level::load_fail_bad_format)
	{
		cerr << "Your level was improperly formatted" << endl;
		return -1;
	}
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		for (int level_x = 0; level_x < LEVEL_WIDTH; level_x++)
		{
			for (int level_y = 0; level_y < LEVEL_HEIGHT; level_y++)
			{
				Level::MazeEntry ge = m_lev.getContentsOf(level_x, level_y);
				int a_x = level_x * SPRITE_WIDTH; //Multiplying correctly?
				int a_y = level_y * SPRITE_HEIGHT;
				switch (ge)
				{
					
				case Level::wall:
					m_actors.push_back(new Wall(a_x, a_y));
					n_actors++;
					break;
				case Level::player:
					m_penelope = new Penelope(a_x, a_y, this);
					break;
				case Level::exit:
					m_actors.push_back(new Exit(a_x, a_y));
					break;
				case Level::citizen:
					m_actors.push_back(new Citizen(a_x, a_y, this));
					n_actors++;
					citizensLeft++;
					break;
				case Level::smart_zombie:
					m_actors.push_back(new SmartZombie(a_x, a_y, this));
					n_actors++;
					break;
				case Level::dumb_zombie:
					m_actors.push_back(new DumbZombie(a_x, a_y, this));
					n_actors++;
					break;
				case Level::gas_can_goodie:
					m_actors.push_back(new GasCanGoodie(a_x, a_y, this));
					n_actors++;
					break;
				case Level::vaccine_goodie:
					m_actors.push_back(new VaccineGoodie(a_x, a_y, this));
					n_actors++;
					break;
				case Level::landmine_goodie:
					m_actors.push_back(new LandmineGoodie(a_x, a_y, this));
					n_actors++;
					break;
				case Level::pit:
					m_actors.push_back(new Pit(a_x, a_y, this));
					n_actors++;
					break;
				case Level::empty:
					break;
					
				default:
					break;
				}
			}
		}
	}
	return 0;
}

bool StudentWorld::checkSurroundings(double xStart, double yStart, double xDest, double yDest) const
{
	std::vector<Actor*>::const_iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		double cX = (*it)->getX();
		double cY = (*it)->getY();
		if (cX == xStart && cY == yStart) //Pass when checking the object that called checkSurroundings
		{
			it++;		
			continue;
		}
		if (cX > (xDest - (SPRITE_WIDTH-1)) && cX < (xDest + (SPRITE_WIDTH-1))
				&& cY > (yDest - (SPRITE_HEIGHT-1)) && cY < (yDest + (SPRITE_HEIGHT-1)))//True if there is an interesection
		{
			Actor *a = (*it);
			if (a->blocksMovement() && !a->isDead()) 
				return true;
			return false; //Does not block movement
		}

		it++; //Lol forgot this boi the first time
	}
	double cX = m_penelope->getX(); //checking for penelope
	double cY = m_penelope->getY();
	if (cX == xStart && cY == yStart) //Pass when checking the object that called checkSurroundings
	{
		return false;
	}
	if (cX > (xDest - (SPRITE_WIDTH - 1)) && cX < (xDest + (SPRITE_WIDTH - 1))
		&& cY >(yDest - (SPRITE_HEIGHT - 1)) && cY < (yDest + (SPRITE_HEIGHT - 1)))//True if there is an interesection
	{
		return true; //There was an interesection
	}


	return false;
}

void StudentWorld::checkOverlap()
{
	std::vector<Actor*>::const_iterator it1 = m_actors.begin();
	std::vector<Actor*>::const_iterator it2;
	for (it1 = m_actors.begin(); it1 != m_actors.end(); it1++)
	{
		for (it2 = it1 + 1; it2 != m_actors.end(); it2++)
		{
			double currX = (*it1)->getX();
			double currY = (*it1)->getY();
			double otherX = (*it2)->getX();
			double otherY = (*it2)->getY();
			double distance = pow((currX - otherX), 2) + pow((currY - otherY), 2);
			if (distance <= 100.0)
			{
				Actor* target1 = (*it1);
				Actor* target2 = (*it2);
				//Tell activated actor to affect target actor
				(*it1)->overlapActivate(target2);
				(*it2)->overlapActivate(target1); 
			}
		}
	}
	std::vector<Actor*>::const_iterator it3 = m_actors.begin(); //Have to check individually for m_penelope since she isn't in m_actors
	for (it3; it3 != m_actors.end(); it3++)
	{
		double currX = (m_penelope)->getX();
		double currY = (m_penelope)->getY();
		double otherX = (*it3)->getX();
		double otherY = (*it3)->getY();
		double distance = pow((currX - otherX), 2) + pow((currY - otherY), 2);
		if (distance <= 100.0)
		{
			Actor* pTarget = m_penelope;
			Actor* target2 = (*it3);
			//Tell activated actor to affect target actor
			(m_penelope)->overlapActivate(target2);
			(*it3)->overlapActivate(pTarget);
		}
	}
	return;
}

bool StudentWorld::canSpawn(double x, double y)
{
	std::vector<Actor*>::const_iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		double cX = (*it)->getX();
		double cY = (*it)->getY();

		double distance = pow((cX - x), 2) + pow((cY - y), 2);
		if (distance <= 100.0)
			return false; //Overlap
		it++;
	}
	double pX = m_penelope->getX(); //checking for penelope
	double pY = m_penelope->getY();
	double distance = pow((pX - x), 2) + pow((pY - y), 2);
	if (distance <= 100.0)
		return false; //Overlap
	return true;
}

double dist(double x, double y, double otherX, double otherY)
{
	return sqrt((pow(x - otherX, 2) + pow(y - otherY, 2)));
}


bool StudentWorld::locatePlayer(double x, double y, double& otherX, double& otherY, double& distance)
{
		otherX = m_penelope->getX();
		otherY = m_penelope->getY();
		distance = dist(x, y, otherX, otherY);
		return true;
}

bool StudentWorld::locateZombieTarget(double x, double y, double& otherX, double& otherY, double& distance)
{
	std::vector<Actor*>::const_iterator it = m_actors.begin();
	double targetX = 0;
	double targetY = 0;
	bool found = false;
	double tempDist = dist(0, 0, VIEW_WIDTH, VIEW_HEIGHT); //Setting distance to max possible
	while (it != m_actors.end())
	{
		double cX = (*it)->getX();
		double cY = (*it)->getY();
		if (cX == x && cY == y) //Pass when checking the object that called checkSurroundings
			{
				it++;
				continue;
			}
		if ((*it)->zombieTarget()) //See if the current actor is a zombie target
		{
			if (tempDist >= dist(x, y, cX, cY)) //Set the otherX and otherY to the closest person you found
			{
				tempDist = dist(x, y, cX, cY);
				otherX = cX;
				otherY = cY;
				found = true;
			}
		}
		it++;
	}
	//Now check for Penelope
	double pX = m_penelope->getX();
	double pY = m_penelope->getY();
	double pDist = dist(x, y, pX, pY);
	if (tempDist > pDist)
	{
		tempDist = dist(x, y, pX, pY);
		otherX = pX;
		otherY = pY;
	}
	distance = tempDist;
	return found;
}

bool StudentWorld::locateZombie(double x, double y, double& otherX, double& otherY, double& distance)
{
	std::vector<Actor*>::const_iterator it = m_actors.begin();
	double targetX = 0;
	double targetY = 0;
	bool found = false;
	double tempDist = dist(0, 0, VIEW_WIDTH, VIEW_HEIGHT); //Setting distance to max possible
	while (it != m_actors.end())
	{
		double cX = (*it)->getX();
		double cY = (*it)->getY();
		if (cX == x && cY == y) //Pass when checking the object that called checkSurroundings
		{
			it++;
			continue;
		}
		if ((*it)->citizenThreat()) //See if the current actor is a citizen threat
		{
			if (tempDist >= dist(x, y, cX, cY)) //Set the otherX and otherY to the closest person you found
			{
				tempDist = dist(x, y, cX, cY);
				otherX = cX;
				otherY = cY;
				found = true;
			}
		}
		it++;
	}
	distance = tempDist;
	return found;
}

bool StudentWorld::addActor(char ID, double a_x, double a_y, int dir)
{
	switch (ID)
	{
	case 'w':
		m_actors.push_back(new Wall(a_x, a_y));
		n_actors++;
		break;
	case 'p':
		m_penelope = new Penelope(a_x, a_y, this);
		break;
	case 'e':
		m_actors.push_back(new Exit(a_x, a_y));
		break;
	case 'c':
		m_actors.push_back(new Citizen(a_x, a_y, this));
		n_actors++;
		break;
	case 's':
		m_actors.push_back(new SmartZombie(a_x, a_y, this));
		n_actors++;
		break;
	case 'd':
		m_actors.push_back(new DumbZombie(a_x, a_y, this));
		n_actors++;
		break;
	case 'f':
		if (checkProjectile(a_x, a_y)) //Check if a Flame can be spawned there
		{
			m_actors.push_back(new Flame(a_x, a_y, this, dir));
			n_actors++;
			return true;
		}
		return false; //If one of the flame's collided with the wall, make sure the other one's don't spawn
		break;
	case 'v':
		if (checkProjectile(a_x, a_y)) //Check if a Vomit can be spawned there
		{
			m_actors.push_back(new Vomit(a_x, a_y, this));
			n_actors++;
			return true;
		}
		return false; //If one of the vomit's collided with the wall, make sure the other one's don't spawn
		break;
	case 'a':
		if (canSpawn(a_x, a_y)) //Check if a Vomit can be spawned there
		{
			m_actors.push_back(new VaccineGoodie(a_x, a_y, this));
			n_actors++;
			return true;
		}
		return false; //If one of the vomit's collided with the wall, make sure the other one's don't spawn
		break;
	case 'm':
		m_actors.push_back(new Landmine(a_x, a_y, this));
		n_actors++;
		break;
	case 'h': 
		m_actors.push_back(new Pit(a_x, a_y, this));
		n_actors++;
	case 'n': //n for empty
		break;
	default:
		break;
	}
}
void StudentWorld::dump()
{

}


bool StudentWorld::checkProjectile(double x, double y)
{
	std::vector<Actor*>::const_iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		double cX = (*it)->getX();
		double cY = (*it)->getY();
		if (cX > (x - (SPRITE_WIDTH - 1)) && cX < (x + (SPRITE_WIDTH - 1))
			&& cY >(y - (SPRITE_HEIGHT - 1)) && cY < (y + (SPRITE_HEIGHT - 1)))//True if there is an interesection
		{
			if (((*it)->blocksProjectiles()))
				return false;
		}
		it++; //Lol forgot this boi the first time
	}
	return true;
}

void StudentWorld::removeDeadActors()
{

	for (int i = 0; i < m_actors.size(); i++)
	{
		if ((m_actors[i])->isDead())
		{
			delete (m_actors[i]); //Delete the actor
			m_actors.erase(m_actors.begin()+i); //Erase from list
		}
	}
}

void StudentWorld::increaseScoreWrapper(int score)
{
	if (m_penelope->isDead()) //Don't want score to change when player died
		return;
	else
		increaseScore(score);
}