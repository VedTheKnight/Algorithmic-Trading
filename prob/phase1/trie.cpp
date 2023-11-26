
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

// bool orderoutput(TrieNode* root ,string name,int price,char option)
// {
//     bool flag;
//     TrieNode* query=search(root,name);
//     if (query==NULL)//insert
//     {
//         insert_trie(root,name,price);
//         flag=1;
//         return flag;
//     }
//     if (option=='b')
//     {
//         if (query->second<price)
//         {
//             flag=true;
//             query->second=price;
//         }
//         else flag=false;
//     }
//     if (option=='s')
//     {
//         if (query->second>price)
//         {
//             flag =true;
//             query->second=price;
//         }
//         else flag=false;
//     }
//     return flag;
    
// }

// void process_problem1(TrieNode* root,string message)
// {
//     string order_name="",temp="";
//     int price=0;
//     int flagstr=0;

//     char option;
//     string pricestring="";
//     for (auto i : message)
//     {
//         if(i==' ')
//         {

//             if (flagstr==0)
//             order_name=temp;
//             else if (flagstr==1)
//             pricestring=temp;
//             flagstr++;
//             temp="";
//             continue;
//         }
//         if (i=='#')
//         {
//             option=temp[0];
//             break;
//         }

//         temp+=i;
//     }

//         for (int i=0;i<pricestring.size();i++)
//     {
//         price+=(pricestring[i]-48)*(pow(10,(pricestring.size()-1-i)));
//     }
//     bool flag=orderoutput(root,order_name,price,option);
//     if (flag==1)
//     {
        
//         if (option=='b')
//         cout <<order_name<<" "<< price<<" s"<<endl;
//         else
//         cout <<order_name<<" "<<price<<" b"<<endl;

//     }
//     else
//     {
//        cout<<"No trade"<<endl;
//     }

// }