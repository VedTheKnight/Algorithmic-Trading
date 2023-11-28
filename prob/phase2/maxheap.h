#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

class MaxHeap
{
public:
  // global root for the entire tree
  std::vector<pair<string,vector<int>>> store;
  unsigned sz = 0;
  int sys_time=0;//if sys_timr>exp_time for order then we cant access that order
  int swap_count = 0;
  // Constructor
  MaxHeap() {};

  // store utilities
  int  size();        // returns the size of store
  void reset();       // clears the store
  void removeLast();  // removes the last element in store
  void append(pair<string,vector<int>>); // appends key in store
  void printStore(unsigned len);

  // Maxheap Navigation
  int parent(int);
  int left(int);
  int right(int);

  // MaxHeap interface
  void swap(int i, int j);
  pair<string,vector<int>>* max();          // read max
  void insert(pair<string,vector<int>>); // insert key in Maxheap
  void Maxheapify(int i);// Maxheapify a node
  void deleteMax();   // deletes max
  void buildMaxHeap();   // builds Maxheap
  void MaxheapSort();    // sorts conentens of Maxheap and does not physically delete
                      // the content

  void print(int i=0, const string& prefix="", bool isLeft=false);
  void endround();

};

inline int MaxHeap::size() {
  return sz;
}

inline void MaxHeap::reset() {
  store.clear();
  sz = 0;
}

inline void MaxHeap::removeLast() {
  sz = sz - 1;
}

inline void MaxHeap::append(pair<string,vector<int>> k) {
  if( sz == store.size() )
    store.push_back(k); // expand if more storage is needed!
  else
    store[sz] = k;
  sz = sz + 1;
}





