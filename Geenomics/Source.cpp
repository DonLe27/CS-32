#include "Trie.h"
#include <iostream>
#include <cassert>
#include "provided.h"
#include <vector>
#include <istream>
#include <string>
#include <fstream>
using namespace std;



int main()
{
	//Testing Trie
	Trie<int> test;
	//Testing Trie's insert


	test.insert("a", 1);
	test.insert("ab", 2);
	test.insert("ab", 3);
	test.insert("abcd", 5);
	test.insert("abcd", 6);
	test.insert("abed", 7);
	test.insert("abrd", 8);
	test.insert("abcg", 9);
	test.insert("lbcd", 10); //Should not be added. Mismatch at the beginning
	test.printTrie(test.getRoot());	
	vector<int> v1 = (test.find("ab", true));
	cout << "findExact" << endl;
	for (std::vector<int>::const_iterator i = v1.begin(); i != v1.end(); ++i)
		std::cout << *i;
	cout << endl << "findSNIP" << endl;
	vector<int> v2 = (test.find("abcd", false));
	for (std::vector<int>::const_iterator i = v2.begin(); i != v2.end(); ++i)
		std::cout << *i;
		
	
	//Testing Genome

	string	filename = "C:\\Users\\donle\\Desktop\\fordon\\data\\test.txt";

	ifstream strm(filename);
	if (!strm)
	{
		cout << "Cannot	open " << filename << endl;
		return 0;
	}
	vector<Genome>	vg;
	bool success = Genome::load(strm, vg);	
	if (success)
	{
		cout << "Loaded	" << vg.size() << " genomes successfully:" << endl;
		for (int k = 0; k != vg.size(); k++)
		{
			string s;
			vg[k].extract(0, vg[k].length(), s);
			cout << s << endl;
			cout << vg[k].name() << endl;
			cout << vg[k].length();
		}

	}
	else
		cout << "Error	loading	genome	data" << endl;

	//Testing GenomeMatcher
	cout << "Testing Genome Matcher" << endl;
	GenomeMatcher m_matcher(3);
	m_matcher.addGenome(vg[0]);
	m_matcher.addGenome(vg[1]);
	m_matcher.minimumSearchLength();



	return 0;
}


