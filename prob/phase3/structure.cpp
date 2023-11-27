#include "market.h"
#include "iostream"
#include "vector"
#include "string"
#include "fstream"
#include "maxheap.h"
#include "minheap.h"
using namespace std;

struct stocks
{
    string name;
    MaxHeap B;
    MinHeap S;
};

void computeMaxHeap(string stock,int price,int time_entry, string name,int quantity,int time_exit,MaxHeap& B,MinHeap& S,int& trades,int& total,int& shares)//its a max heap implying the new order is a sell order ready to sell to someone with highest buy price
{//name is the new order wala 
//so basically abhi we have a new sell order which i will sell to the highest possible buy order if it is valid and has all other quantity and time valid
// fix the issue of infinite time
    vector<pair<string,vector<int>>> arbitrage; 
    while(quantity>0)//if quantity has become 0 we will be exiting aage hi but we also need to see if price of sell orders exceed our max buy limit price
    {

        if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
        break;
        if(B.max()->second[0]<price)//no more elements in the heap to trade with since the max element is lesser than my sell price
        break;
        if (B.max()->first==name && B.max()->second[0]!=price)//prevent arbitrage
        {
            if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            break;//default escape if its not valid 
            arbitrage.push_back({B.max()->first,B.max()->second});
            B.deleteMax();
            //S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
            //return;
        }
        if(quantity<=B.max()->second[2])
        {
             if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            break;
            int p=B.max()->second[0];//just in case calling max twice cleans the top making max inaccessible
            cout<<B.max()->first<<" purchased "<<quantity<<" share of "<<stock<<" from "<<name<<" for $"<<p<<"/share"<<endl;
            total+=p*quantity;
            trades++;
            shares+=quantity;
            B.max()->second[2]-=quantity;
            quantity=0;
            return;//if it is possible for the heap to satisfy our order it exits here
        }
        else
        {
             if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            break;
            int p=B.max()->second[0];
            cout<<B.max()->first<<" purchased "<<B.max()->second[2]<<" share of "<<stock<<" from "<<name<<" for $"<<p<<"/share"<<endl;
            trades++;
            shares+=B.max()->second[2];
            total+=B.max()->second[2]*p;
            quantity-=B.max()->second[2];
            B.max()->second[2]=0;
        }
    }
    //once it comes here we have not satisfied the entire order so we must add it to the MinHeap of S
    S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
    for(int i=0;i<arbitrage.size();i++)
    {
        B.insert(arbitrage[i]);
    }
}

void computeMinHeap(string stock,int price,int time_entry, string name,int quantity,int time_exit,MaxHeap& B,MinHeap& S,int& trades,int& total,int& shares)//its a min heap implying the new order is a buy order ready to buy from someone with lowest sell price
{
//so basically abhi we have a new buy order which i will buy from the lowest possible sell order if it is valid and has all other quantity and time valid
    vector<pair<string,vector<int>>> arbitrage;
    while(quantity>0)//if quantity has become 0 we will be exiting aage hi but we also need to see if price of sell orders exceed our max buy limit price
    {
        if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
        break;
        if(S.min()->second[0]>price)//no more elements in the heap can be traded with since lowest sell is greater than our buy price
        break;
        if (S.min()->first==name && S.min()->second[0]!=price)//prevent arbitrage
        {
            if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            break;
            arbitrage.push_back({S.min()->first,S.min()->second});
            S.deleteMin();
            //B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
            //return;
        }
        if(quantity<=S.min()->second[2])
        { 
            if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            break;
            int p=S.min()->second[0];
            cout<<name<<" purchased "<<quantity<<" share of "<<stock<<" from "<<S.min()->first<<" for $"<<p<<"/share"<<endl;
            trades++;
            shares+=quantity;
            total+=quantity*p;
            S.min()->second[2]-=quantity;
            quantity=0;
            return;//if it is possible for the heap to satisfy our order it exits here
        }
        else
        {
            if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            break;
            int p=S.min()->second[0];
            cout<<name<<" purchased "<<S.min()->second[2]<<" share of "<<stock<<" from "<<S.min()->first<<" for $"<<p<<"/share"<<endl;
            trades++;
            shares+=S.min()->second[2];
            total+=S.min()->second[2]*p;
            quantity-=S.min()->second[2];
            S.min()->second[2]=0;
        }
    }
    //once it comes here we have not satisfied the entire order so we must add it to the MaxHeap of B
    B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
    for(int i=0;i<arbitrage.size();i++)
    {
        S.insert(arbitrage[i]);
    }
}
