#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Trie.h"
#include <map>
#include <unordered_map>
#include <algorithm>
using namespace std;

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
	 //REMEMBER TO MOVE BACK TO PRIVATE AFTER DONE TESTING
private:
	int minLength;
	std::unordered_map<string, Genome> m_collection;

	Trie<DNAMatch> m_trie;

	
};
bool compareFunction(const GenomeMatch &a, const GenomeMatch &b)
{
	if (a.percentMatch > b.percentMatch)
		return true;
	else if (a.percentMatch < b.percentMatch)
		return false;
	else
		return a.genomeName >= b.genomeName;
}

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
    // This compiles, but may not be correct
	minLength = minSearchLength; //Can only search for string this length or longer
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	m_collection.insert({ genome.name(),genome });
	int length = genome.length();
	for (int i = 0; i <= length - minLength; i++)
	{
		DNAMatch newSegment;
		newSegment.genomeName = genome.name();
		newSegment.length = minLength;
		newSegment.position = i; 
		//string val = "(Genome " + genome.name() + ", position " + to_string(i) + ")"; //Assumed this is what we print
		string sub;
		genome.extract(i, minLength, sub);
		m_trie.insert(sub, newSegment);
		//m_trie.insert(sub, val);
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return minLength;
}

//FINISH THIS
bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	bool result = false;
	if (minimumLength < minimumSearchLength()|| fragment.length() < minimumLength)
		return false;
	vector<DNAMatch> beginMatches = m_trie.find(fragment.substr(0, minLength), exactMatchOnly); //Contains all DNAMatches of length minimumSearchLength()
	unordered_map<string, DNAMatch> mapMatches;
	for (int i = 0; i < beginMatches.size(); i++) //Go through all beginMatches and extract characters to check
	{	
		Genome curr = m_collection.find(beginMatches[i].genomeName)->second; //Trie holds pointer to genome. This allows O(1) access time to a genome
		int matchLength = 0; //This holds the length of the potential match
		string seq;
		int pos = beginMatches[i].position; 
		int fragLen = fragment.length();
		bool success = (curr.extract(pos, fragLen, seq));  //Seq holds potential matching section of a genome
		if (!success) //Seq went past the end 
		{
			int newLen = curr.length() - pos;
			curr.extract(pos, newLen, seq);
		}
		if (exactMatchOnly)
		{
			for (int j = 0; j < fragLen; j++)
			{
				char s = seq[j];
				char f = fragment[j];
				if (seq[j] == fragment[j])
					matchLength++;
				else
					break;
			}
		}
		else
		{
			int mismatch = 0;
			if (fragment[0] != seq[0]) //The first char didn't match
				continue;
			for (int i = 0; i < fragment.length(); i++)
			{
				if (seq[i] != fragment[i])
					mismatch++;
				matchLength++;
				if (mismatch > 1)
				{
					matchLength--;
					break;
				}
			}
		}
		if (matchLength >= minimumLength)
		{
			DNAMatch newMatch;
			newMatch.genomeName = beginMatches[i].genomeName;
			newMatch.length = matchLength;
			newMatch.position = beginMatches[i].position;
			std::unordered_map<string, DNAMatch>::iterator it;
			it = mapMatches.find(newMatch.genomeName);
			if (it == mapMatches.end()) //Item not found so insert it
				mapMatches.insert({ newMatch.genomeName, newMatch });
			else //DNAMatch for this genome already in the map. See if this DNAMatch has a longer length
			{
				if (it->second.length < newMatch.length)
					it->second.length = newMatch.length;
			}
			result = true;
		}
	}
	//Insert everything in the matchMap into the vector
	unordered_map<string, DNAMatch>::iterator it; //This time complexity is at most O(F) so it will only make total complexity O(2*H*F) == O(H*F)
	for (it = mapMatches.begin(); it != mapMatches.end(); it++)
	{
		matches.push_back(it->second);
	}
	return result;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	if (fragmentMatchLength < minimumSearchLength())
		return false;
	double totalSequences = query.length() / fragmentMatchLength; //Holds number of subsequences in query
	bool result = false;
	std::unordered_map<string, double> mapMatches; //Holds number of matches
	for (int i = 0; i < (query.length() / fragmentMatchLength); i++) //For each subsequence of query
	{
		string seq;
		vector<DNAMatch> matches;
		bool success = query.extract(i * fragmentMatchLength, fragmentMatchLength, seq);
		if (!success) //couldn't extract for some reason
			continue;
		if (findGenomesWithThisDNA(seq, fragmentMatchLength, exactMatchOnly, matches)) //Found at least one match
			result = true;
		for (int i = 0; i < matches.size(); i++) 
			//For all DNA matches. Has time complexity of O(G) where G is the number of genomes in the library. This is a neglible amount as G is a relatively small number
		{
			string genomeName = matches[i].genomeName;
			std::unordered_map<string, double>::iterator it;
			it = mapMatches.find(genomeName);
			if (it == mapMatches.end()) //Item not found so insert it
				mapMatches.insert({ genomeName, 1 });
			else
			{
				it->second++; //Increment count
			}
		}
	}
	std::unordered_map<string, double>::iterator it;
	for (it = mapMatches.begin(); it != mapMatches.end(); it++) //Transfer mapMatches to results
	{
		GenomeMatch newGenomeMatch;
		newGenomeMatch.genomeName = it->first;
		double percent = 100.0 * (1.0 * it->second) / (totalSequences * 1.0);
		newGenomeMatch.percentMatch = percent;
		if (percent >= matchPercentThreshold)
			results.push_back(newGenomeMatch);
	}
	std::sort(results.begin(), results.end(), compareFunction);
	return result;
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
