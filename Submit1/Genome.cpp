#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
	GenomeImpl(const string& nm, const string& sequence);
	static bool load(istream& genomeSource, vector<Genome>& genomes);
	int length() const;
	string name() const;
	bool extract(int position, int length, string& fragment) const;
private:
	string m_name;
	string m_sequence;
	//No destructor needed since I don't use pointers
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence) 
	: m_name(nm), m_sequence(sequence)
{

}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes)
{
	genomes.clear(); //Make sure original contents are gone
	//genomeSource holds our file that we can getline from
	string s;
	int index = 0;
	string name = "";
	string sequence = "";
	bool baseCheck = false; //Gets turned off by having bases after name
	//Get first genome name
	getline(genomeSource, s);
	if (s[0] != '>') //The first line didn't contain a name
		return false;
	name = s.substr(1);
	if (name.length() == 1) //First name only had > sign
		return false;
	char c;
	char pastChar = 'a';
	while (genomeSource.get(c)) //Get line from file until no more lnes
	{
		 //Detects if there are two consecutive newline character (represents empty line)
		if (c == '\n' && pastChar == '\n')
			return false;
		pastChar = c;
		if (c != '>') //Continue adding until you get to next name
		{
			if (c == 'A' || c == 'C' || c == 'T' || c == 'G' || c == 'N')
				sequence += (c);
			else if (c == 'a' || c == 'c' || c == 't' || c == 'g' || c == 'n')
				sequence += toupper(c);
			else if (c != '\n')//Wasn't a name and wasn't one of the wanted characters
				return false;
			baseCheck = false;
		}
		else //You hit a name, so add new genome and reset variables.
		{
			genomes.push_back(Genome(name, sequence));
			index++;
			getline(genomeSource, s); //Get the rest of the line as name
			if (s.size() == 0) //The name only had > sign
				return false;
			name = s;
			sequence = "";
			baseCheck = false; //Turn baseCheck back on after you get the name
		}
	}
	if (!baseCheck)
	{
		genomes.push_back(Genome(name, sequence)); //Add the last genome
	}
	if (baseCheck)
		return false;
	return true;
}

int GenomeImpl::length() const
{
	return m_sequence.length();  // This compiles, but may not be correct
}

string GenomeImpl::name() const
{
	return m_name;  // This compiles, but may not be correct
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
	if ((position + length) > m_sequence.length())
		return false;
	else
	{
		fragment = m_sequence.substr(position, length);
		return true;
	}
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes) 
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}
