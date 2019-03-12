#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <iostream>
using namespace std;

template<typename ValueType>
class Trie
{
	struct Node;
public:
    Trie();
    ~Trie();
    void reset();
	void insert(const std::string& key, const ValueType& value)
	{
		int strSize = key.size();
		Node* currNode = root;
		//Check first node
		if (strSize <= 0) //Can we have an empty key?
			return;
		for (int i = 0; i < strSize; i++)
		{
			char ch = key[i];
			bool foundLabel = false;
			//Check children for label defined by char
			for (int j = 0; j < currNode->children.size(); j++)
			{
				if (currNode->children[j]->label == ch)
				{
					currNode = currNode->children[j];
					foundLabel = true;
					break;
				}
			}
			if (!foundLabel) //In the currNode, none of the children have the value you want
			{
				for (i; i < strSize; i++) //Continue iteration at character we couldn't find
				{
					ch = key[i];
					Node* tempNode = addNode(currNode, ch); //Add a new node holding the character 
					currNode = tempNode;
				}
				break; //Once done, currNode will point to the last node you added (the one that should receive the value)
			}
		}
		//currNode now holds Node you want to add the value to
		//if ((std::find(currNode->values.begin(), currNode->values.end(), value) == currNode->values.end())) //Duplicates are allowed!
		currNode->values.push_back(value);
	}
	std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;

      // C++11 syntax for preventing copying and assignment
   Trie(const Trie&) = delete;
   Trie& operator=(const Trie&) = delete;
   Node* getRoot() { return root; } //REMEMBER TO REMOVE AFTER TESTING AND DON'T USE IN IMPLEMENTATION
   void printTrie(Trie<ValueType>::Node* root) //Print label and values
   {
	   if (root == nullptr)
		   return;
	   cout << root->label << endl;
	   for (int i = 0; i < root->values.size(); i++)
	   {
		   cout << root->values[i] << endl;
	   }
	   for (int j = 0; j < root->children.size(); j++)
		   printTrie(root->children[j]);
   }
private:
	struct Node
	{
		char label;
		std::vector<ValueType> values;
		std::vector<Node*> children;
		//Vector or list? Iteration is same for both.
		//Vector can access elements quickly
		//List can add items quickly. 
	};
	Node* root;
	void deleteTree(Node* root)
	{
		if (root == nullptr)
			return;
		int size = (root->children).size();
		for (int i = 0; i < size; i++)
		{
			deleteTree(root->children[i]);
		}
		delete root;
	}
	Node*addNode(Node*parent, char label)
	{
		Node* newNode = new Node;
		newNode->label = label;
		parent->children.push_back(newNode);
		return newNode;
	}
	std::vector<ValueType> findExact(const std::string& key, Node* root) const;
	std::vector<ValueType> findSNIP(const std::string& key, Node* root, int mismatch) const;
};

template<typename ValueType>
Trie<ValueType>::Trie()
{
	root = new Node;
	root->label = NULL;
}


template<typename ValueType>
void Trie<ValueType>::reset()
{
	deleteTree(root);
	root = new Node;
}

template<typename ValueType>
Trie<ValueType>::~Trie()
{
	deleteTree(root);
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string& key, bool exactMatchOnly) const
{ 
	if (exactMatchOnly)
		return findExact(key, root);
	else
		return findSNIP(key, root, 0);
	
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::findExact(const std::string& key, Node* curr) const
{
	std::vector<ValueType> empty; //empty vector
	if ((root->children.size() == 0)) //If root node has no children, return
		return empty;
	if (key.size() == 0) //If key is empty, we found it
		return curr->values;
	//See which of the children match
	for (int i = 0; i < curr->children.size(); i++)
	{
		if (curr->children[i]->label == key[0]) //Found the child that matched
		{
			return findExact(key.substr(1), curr->children[i]);
		}
	}
	return empty;
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::findSNIP(const std::string& key, Node* curr, int mismatch) const
{
	std::vector<ValueType> empty; //empty vector
	std::vector<ValueType> result;
	if (mismatch > 1) //To many mismatches, this path won't yield a SNIP
		return empty;
	if ((root->children.size() == 0)) //If root has no children, no SNIP's
		return empty;
	if (key.size() == 0) //Got to the end, found the values we want
		return curr->values;
	//See which of the children match next character
	for (int i = 0; i < curr->children.size(); i++)
	{
		int tempMis = mismatch;
		if (curr->children[i]->label != key[0])
			tempMis++;
		if (curr == root && tempMis != 0) //Don't allow there to be missmatches at the beginning
			continue;
		std::vector<ValueType> temp = findSNIP(key.substr(1), curr->children[i], tempMis);
		result.insert(result.end(), temp.begin(), temp.end());
	}
	return result;

}



#endif // TRIE_INCLUDED
