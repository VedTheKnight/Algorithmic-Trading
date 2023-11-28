#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <bits/stdc++.h>

using namespace std;

class MinHeap
{
public:
  // global root for the entire tree
  std::vector<pair<string,vector<int>>> store;
  unsigned sz = 0;
  int sys_time=0;//if sys_timr>exp_time for order then we cant access that order
  int swap_count = 0;
  int market;
  // Constructor
  MinHeap() {};

  // store utilities
  int  size();        // returns the size of store
  void reset();       // clears the store
  void removeLast();  // removes the last element in store
  void append(pair<string,vector<int>> k); // appends key in store
  void printStore(unsigned len);

  // Minheap Navigation
  int parent(int);
  int left(int);
  int right(int);

  // MinHeap interface
  void swap(int i, int j);
  pair<string,vector<int>>* min();          // read Min
  void insert(pair<string,vector<int>> k); // insert key in Minheap
  void Minheapify(int i);// Minheapify a node
  void deleteMin();   // deletes Min
  void buildMinHeap();   // builds Minheap
  void MinheapSort();    // sorts conentens of Minheap and does not physically delete
                      // the content

  void print(int i=0, const string& prefix="", bool isLeft=false);

};

inline int MinHeap::size() {
  return sz;
}

inline void MinHeap::reset() {
  store.clear();
  sz = 0;
}

inline void MinHeap::removeLast() {
  sz = sz - 1;
}

inline void MinHeap::append(pair<string,vector<int>> k) {
  if( sz == store.size() )
    store.push_back(k); // expand if more storage is needed!
  else
    store[sz] = k;
  sz = sz + 1;
}





