#include <iostream>

//u->(price,time-entry,quantity,time exit) while be the type of vectors in our store

#include "minheap.h"
using namespace std;
bool comparemin(pair<string,vector<int>>u,pair<string,vector<int>>v)//function should return 1 when u>v;u->(price,time-entry,name,quantity,time exit)
{
  if (u.second[0]<v.second[0])
  return 0;
  else if(u.second[0]>v.second[0])//greater value for more price
  return 1;
  else
  {
    if (u.second[1]<v.second[1])
    return 0;//greater value for earlier entry into the order book
    else if(u.second[1]>v.second[1])
    return 1;
    else
    {
      if (u.first>=v.first)
      return 0;
      else if(u.first<=v.first)
      return 1;//greater value for a name which appears first in alphabetical order
    }
  }
}

int MinHeap::parent(int i) {
  return (i-1)/2; 
}

int MinHeap::left(int i) {
  return 2*i+1; 
}

int MinHeap::right(int i) {
  return 2*i+2; 
}

pair<string,vector<int>>* MinHeap::min() {
  if(sz==0)
    {
      pair<string,vector <int>>* p= new pair<string,vector <int>> {"",{0,0,0,0,0}};
      return p;
    }//we want to return an invalid vector for an empty heap
  while(sz!=0 && sys_time>store[0].second[3])//keep deleteing the Min till the Min is not one which is expired already
  deleteMin();
  while(sz!=0 && store[0].second[2]<0)//keep deleting the Min till the Min is not one which has quantity 0 i.e a used up order
  deleteMin();
  if(sz!=0)
  return &store[0];//returns the valid Min element for the trade
  else
  {
    pair<string,vector <int>>* p= new pair<string,vector <int>> {"",{0,0,0,0,0}};
    return p;
  }//we want to return an invalid vector for an empty heap
  
}

void MinHeap::swap(int i, int j) {
  pair<string,vector<int>> temp;
  temp = store[i];
  store[i] = store[j];
  store[j] = temp;
}

void MinHeap::insert(pair<string,vector<int>>v) {
  int i = sz;
  append(v);

  while(i>0 && comparemin(store[parent(i)],store[i])==1)//while we have not reached root anf while the parent<node we keep swapping
  {
    swap(i,parent(i));
    i = parent(i);
  }
}

void MinHeap::Minheapify(int i) {
  int c = 0; //to store the Min index between both children and the supplied node
  if(left(i)>=int(sz)&&right(i)>=int(sz))//no child 
  {
    return;
  }
  else if(right(i)>=(int)sz)//only left child
  {
    if(comparemin(store[left(i)],store[i])==0)
      c = left(i);
    else
      return;
  }
  else
  { //if both the children exist
    if((comparemin(store[i],store[left(i)])==0) && (comparemin(store[i],store[right(i)])==0)){
      return; //this means store[i] is lesser than both its children hence it is a valid Minheap
    }
    else if((comparemin(store[left(i)],store[i])==0) && (comparemin(store[left(i)],store[right(i)])==0))//left child<node and left child<rt child so swap with left child
    {
      c = left(i);
    }
    else if((comparemin(store[right(i)],store[i])==0) && (comparemin(store[right(i)],store[left(i)])==0))//if right child<node and right child<left child then swap node with right child
    {
      c = right(i);
    }
  }
  swap(i,c);
  swap_count++;
  Minheapify(c);
}


void MinHeap::deleteMin() {
  swap_count = 0;
  swap(0,sz-1);
  swap_count++;
  sz = sz-1; //isn't physically deleting the element!
  Minheapify(0);
}

void MinHeap::buildMinHeap() {
  for(int i = sz-1; i>=0; i--){
    Minheapify(i);
  }
}

void MinHeap::MinheapSort() {
  buildMinHeap(); //assuming the original vector is just a random sequence of numbers
  while(sz>0){
    deleteMin();
  }
}


