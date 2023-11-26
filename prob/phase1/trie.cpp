#include "string"
#include<vector> 
using namespace std;
#define ALPHABET_SIZE 26

struct TrieNode 
{ 
    TrieNode **children = new TrieNode*[ALPHABET_SIZE];
    // isEndOfWord is true if the node 
    // represents end of a word 
    bool isEndOfWord=false; 
    vector<int> second={-1,-1,-1};
    TrieNode()
    {
    for(int i=0;i<ALPHABET_SIZE;i++)
    children[i] = NULL;
    }
}; 

void insert_trie(TrieNode* root,string key,vector<int> prices)
{
    TrieNode* travel=new TrieNode;
     travel= root;
    for (int i=0;i<key.length();i++)
    {
        int index= key[i]-'A';
        if (travel->children[index]==NULL)
        {
            TrieNode* newnode=new TrieNode; 
            travel->children[index]=newnode;
        }
        travel=travel->children[index];

    }
    travel->isEndOfWord=true;
    travel->second=prices;
}

TrieNode* search(TrieNode* root,string key) //will return the last node in the search and null if it does not exist
{
    TrieNode* travel=root;
    for (int i=0;i<key.length();i++)
    {
        int index= key[i]-'A';
        if (travel->children[index]==NULL)
        {
            return NULL;
        }
        else
        {
            travel=travel->children[index];
        }

    }
    if (travel->isEndOfWord==true)//check for the case where we have key ACT but word is ACTOR,we will traverse till end of key but that wont be a valid word
    return travel;
    else
    return NULL;
}
