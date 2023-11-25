#include <iostream>

//u->(price,time-entry,quantity,time exit) while be the type of vectors in our store

#include "maxheap.h"
using namespace std;
bool comparemax(pair<string,vector<int>>u,pair<string,vector<int>>v)//function should return 1 when u>v;u->(price,time-entry,quantity,time exit)
{
  if (u.second[0]<v.second[0])
  return 0;
  else if(u.second[0]>v.second[0])//greater value for more price
  return 1;
  else
  {
    if (u.second[1]<v.second[1])
    return 1;//greater value for earlier entry into the order book
    else if(u.second[1]>v.second[1])
    return 0;
    else
    {
      if (u.first>v.first)
      return 0;
      else if(u.first<=v.first)
      return 1;//greater value for a name which appears first in alphabetical order
    }
  }
}

int MaxHeap::parent(int i) {
  return (i-1)/2; 
}

int MaxHeap::left(int i) {
  return 2*i+1; 
}

int MaxHeap::right(int i) {
  return 2*i+2; 
}

pair<string,vector<int>>* MaxHeap::max() //it will always return a valid 
{
  if(sz==0)
    {
       pair<string,vector <int>>* p=new  pair<string,vector <int>>{"",{0,0,0,0,0}};
       return p;
    }//we want to return an invalid vector for an empty heap;
  while(sz!=0 && sys_time>=store[0].second[3])//keep deleteing the max till the max is not one which is expired already
  deleteMax();
  while(sz!=0 && store[0].second[2]==0)//keep deleting the max till the max is not one which has quantity 0 i.e a used up order
  deleteMax();
  if(sz!=0)
  return &store[0];//returns the valid max element for the trade
  else
  {
    pair<string,vector <int>>* p= new pair<string,vector <int>> {"",{0,0,0,0,0}};
    return p;
  }//we want to return an invalid vector for an empty heap
}

void MaxHeap::swap(int i, int j) {
  pair<string,vector<int>> temp;
  temp = store[i];
  store[i] = store[j];
  store[j] = temp;
}

void MaxHeap::insert(pair<string,vector<int>>v) {
  int i = sz;
  append(v);

  while(i>0 && comparemax(store[parent(i)],store[i])==0)//while we have not reached root anf while the parent<node we keep swapping
  {
    swap(i,parent(i));
    i = parent(i);
  }
}

void MaxHeap::Maxheapify(int i) {
  int c = 0; //to store the max index between both children and the supplied node
  if(left(i)>=int(sz)&&right(i)>=int(sz))//no child 
  {
    return;
  }
  else if(right(i)>=(int)sz)//only left child
  {
    if(comparemax(store[left(i)],store[i])==1)
      c = left(i);
    else
      return;
  }
  else
  { //if both the children exist
    if((comparemax(store[i],store[left(i)])==1) && (comparemax(store[i],store[right(i)])==1)){
      return; //this means store[i] is greater than both its children hence it is a valid Maxheap
    }
    else if((comparemax(store[left(i)],store[i])==1) && (comparemax(store[left(i)],store[right(i)])==1))//left child>=node and left child>=rt child so swap with left child
    {
      c = left(i);
    }
    else if((comparemax(store[right(i)],store[i])==1) && (comparemax(store[right(i)],store[left(i)])==1))//if right child>=node and right child>=left child then swap node with right child
    {
      c = right(i);
    }
  }
  swap(i,c);
  swap_count++;
  Maxheapify(c);
}


void MaxHeap::deleteMax() {
  swap_count = 0;
  swap(0,sz-1);
  swap_count++;
  sz = sz-1; //isn't physically deleting the element!
  Maxheapify(0);
}

void MaxHeap::buildMaxHeap() {
  for(int i = sz-1; i>=0; i--){
    Maxheapify(i);
  }
}

void MaxHeap::MaxheapSort() {
  buildMaxHeap(); //assuming the original vector is just a random sequence of numbers
  while(sz>0){
    deleteMax();
  }
}

void MaxHeap::endround()
{
  sys_time++;
}

