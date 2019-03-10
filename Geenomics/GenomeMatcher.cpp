#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Trie.h"
using namespace std;

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
	Trie<string> m_trie; //REMEMBER TO MOVE BACK TO PRIVATE AFTER DONE TESTING
private:
	int minLength;
	std::vector<Genome> m_collection;

};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
    // This compiles, but may not be correct
	minLength = minSearchLength; //Can only search for string this length or longer
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
    // This compiles, but may not be correct
	m_collection.push_back(genome);
	int length = genome.length();
	for (int i = 0; i <= length - minLength; i++)
	{
		string val = "(Genome " + genome.name() + ", position " + to_string(i) + ")"; //Assumed this is what we print
		string sub;
		genome.extract(i, minLength, sub);
		m_trie.insert(sub, val);
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
    return minLength;  // This compiles, but may not be correct
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return false;  // This compiles, but may not be correct
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return false;  // This compiles, but may not be correct
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
	m_impl->m_trie.printTrie(m_impl->m_trie.getRoot()); //REMOVE AFTER DONE TESTING
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
