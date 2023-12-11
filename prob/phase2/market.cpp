#include "market.h"
#include "iostream"
#include "vector"
#include "string"
#include "fstream"

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

class MinHeap
{
public:
  // global root for the entire tree
  std::vector<pair<string,vector<int>>> store;
  unsigned sz = 0;
  int sys_time=0;//if sys_timr>exp_time for order then we cant access that order
  int swap_count = 0;
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
      return 1;//greater value for a name which appears ly in alphabetical order
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
  while(sz!=0 && sys_time>store[0].second[3])//keep deleteing the max till the max is not one which is expired already
  deleteMax();
  while(sz!=0 && store[0].second[2]<0)//keep deleting the max till the max is not one which has quantity 0 i.e a used up order
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
      if (u.first>v.first)
      return 1;
      else if(u.first<=v.first)
      return 0;//greater value for a name which appears first in alphabetical order
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

//--------------------------------------------STRINGPROCESS-----------------------------------------------------
vector<string> tokenize(string input){

    vector<string> tokens;

    size_t start = 0;
    size_t end = input.find(' ');

    while (end != string::npos) {
        // Extract the token from the input string
        string token = input.substr(start, end - start);

        // Add the token to the vector
        tokens.push_back(token);

        // Update the start and end positions for the next token
        start = end + 1;
        end = input.find(' ', start);
    }

    // Add the last token after the loop
    tokens.push_back(input.substr(start));

    return tokens;
}

string rename(vector<string>tokens,int i)//1 to i ko sort
{

}

vector<string> final_tokenize(vector<string> tokens){
    vector<string> final_tokens;
    final_tokens.push_back(tokens[0]);//pushes back the time of entry
    final_tokens.push_back(tokens[1]);//pushes back the name
    final_tokens.push_back(tokens[2]);//pushes back the BUY or SELL
    string name;
    int i;
    for( i=3;i<tokens.size();i++)
    {
        if(tokens[i][0]!='$')
        {
            name+=tokens[i]+" ";
            continue;
        }
        break;
    }
    //remove last space in the name which will be extra and we dont want it
    name.pop_back();
    // so now our name has "XYZ -1 ABC 1"
    //IMP at this pt i is basically the i at which i becomes SELL or BUY so at the old values SO 1 to i-1 will have alternate name and coeff
    //name=rename(tokens,i)
    final_tokens.push_back(name);
    for (i;i<tokens.size();i++)
    {
        if(tokens[i][0]=='$')
        final_tokens.push_back(tokens[i].substr(1,tokens[i].size()-1));
        else if (tokens[i][0]=='#')
        {
            final_tokens.push_back(tokens[i].substr(1,tokens[i].size()-1));
        }
        else 
        {final_tokens.push_back(tokens[i]);}
    }
    return final_tokens;
}
bool checkEquality(string old_order, string name){

    bool flag = false;
   
    vector<string> tokens = tokenize(old_order);
    vector<string> name_tokens = tokenize(name);

    int counter = 0;
    if(tokens.size()==1 && name_tokens.size()==1)
    {
        if(name==old_order)
        {
            return 1;
        }
        else
        return 0;
        
    }
    
    if(tokens.size()!= name_tokens.size())
    {
        return 0;
        
    }

    for(int j = 0; j<tokens.size(); j++){
        for(int k = 0; k<name_tokens.size();k+=2){
            if(tokens[j] == name_tokens[k] && tokens[j+1] == name_tokens[k+1])
                counter++;
        }
    }

    if(counter == name_tokens.size()/2 && counter == (tokens.size())/2){
        flag = true;
    }

    return flag;
}

int stringToInt(const std::string& str) {
    int result = 0;
    for (char c : str) {
        if (c >= '0' && c <= '9') {
            // Convert the character to an integer and add it to the result
            result = result * 10 + (c - '0');
        } 
    }
    if(str[0]=='-')
        return -1*result;
    else    
        return result;
}

// AMZN 1 -> AMZN automatically when checkValidity is called. 
// check validity autocapitalizes BUY and SELL in an order

// returns the stock bundle name
string getStockName(vector<string> tokens){
    string stock_name="";
    for(int i = 3; i<tokens.size()-3; i++){
        stock_name+= tokens[i];
        stock_name+=" ";
    }
    stock_name.pop_back();
    return stock_name;
}

//checks if the input string is a valid ticker
bool isAlphabetical(string s){
    for(char c : s){
        if(!(static_cast<int>('a') <= tolower(c) && static_cast<int>('z') >= tolower(c))){
            return false;
        }
    }
    return true;
}

//checks if given string is a number both positive and negative included
bool isNumber(string n){

    for(int i =0 ; i<n.size(); i++){
        if(n[i] == '-' && i == 0)
            continue;
        else if(!(48 <= n[i] && n[i]<=57)){
            return false;
        }
    }

    return true;
}

//checks if given string is a positive number
bool isPositiveNumber(string n){

    for(int i =0 ; i<n.size(); i++){
        if(!(48 <= n[i] && n[i]<=57)){
            return false;
        }
    }

    return true;
}

//pass by reference makes the passed string into capitals
void makeCapital(string& s){
    for(auto &c : s){
        c = toupper(c);
    }
}

// checks order validity
// also converts AMZN 1 type orders into AMZN
bool checkOrderValidity(string& order){

    vector<string> tokens = tokenize(order);
    if(tokens.size()<6){
        return false;
    }

    //check first and last token entry time and exit time
    if(!isNumber(tokens[0]) || !isNumber(tokens[tokens.size()-1]))
        return false;
    
    //quantity check
    if(!(tokens[tokens.size()-2][0] == '#' && isPositiveNumber(tokens[tokens.size()-2].substr(1)))){
        return false;
    }

    //price check
    if(!(tokens[tokens.size()-3][0] == '$' && isPositiveNumber(tokens[tokens.size()-3].substr(1)))){
        return false;
    }

    makeCapital(tokens[2]);
    if(!(tokens[2] == "BUY" || tokens[2] == "SELL")){
        return false;
    }

    string name = getStockName(tokens);

    vector<string> name_tok = tokenize(name);

    if(name_tok.size() == 1){
        if(!isAlphabetical(name_tok[0])){
            return false;
        }
    }
    else{
        if(name_tok.size()%2 == 1){
            return false;
        }
        else{
            for(int i = 0; i<name_tok.size(); i++){
                if(i%2 == 0 && !isAlphabetical(name_tok[i])){
                    return false;
                }
                if(i%2 == 1 && !isNumber(name_tok[i])){
                    return false;
                }
            }
        }
    }

    if(name_tok.size() == 2 && name_tok[1] == "1"){
        name = name_tok[0];
    }

    //now reconstruct order
    order = "";
    order = order + tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + name + " " + tokens[tokens.size()-3] + " " + tokens[tokens.size()-2] + " " + tokens[tokens.size()-1];
    return true;
}

//_________________________________________________________________Update to checkMatching___________________________________________________________

// checkMatching("AMZN 1", "AMZN") is true although redundant


// function to check for matching stock combo : imported from phase 2 part 1
bool checkMatching(string old_order, string name){

    bool flag = false;
   
    vector<string> tokens = tokenize(old_order);
    vector<string> name_tokens = tokenize(name);

    if((tokens.size() == 2 && tokens[1] == "1" && name_tokens.size() == 1 && name_tokens[0] == tokens[0]) || (name_tokens.size() == 2 && name_tokens[1] == "1" && tokens.size() == 1 && name_tokens[0] == tokens[0])){
        return 1;
    }
    int counter = 0;
    if(tokens.size()==1 && name_tokens.size()==1)
    {
        if(name==old_order)
        {
            return 1;
        }
        else
        return 0;
        
    }
    
    if(tokens.size()!= name_tokens.size())
    {
        return 0;
    }

    for(int j = 0; j<tokens.size(); j++){
        for(int k = 0; k<name_tokens.size();k+=2){
            if(tokens[j] == name_tokens[k] && tokens[j+1] == name_tokens[k+1])
                counter++;
        }
    }

    if(counter == name_tokens.size()/2 && counter == (tokens.size())/2){
        flag = true;
    }

    return flag;
}
//--------------------------------------------STRINGPROCESS-----------------------------------------------------

//--------------------------------------------STOCKS.CPP--------------------------------------------------------
struct accounts
{
    string name;
    int buy_no=0;
    int sell_no=0;
    int net=0;
};
struct stocks
{
    string name;
    MaxHeap B;
    MinHeap S;
};
//u->(price,time-entry,quantity,time exit)
void computeMaxHeap(string stock,int price,int time_entry, string name,int quantity,int time_exit,MaxHeap& B,MinHeap& S,vector<accounts>& accountlist,int& trades,int& total,int& shares)//its a max heap implying the new order is a sell order ready to sell to someone with highest buy price
{//name is the new order wala 
//so basically abhi we have a new sell order which i will sell to the highest possible buy order if it is valid and has all other quantity and time valid
// fix the issue of infinite time
    vector<pair<string,vector<int>>> arbitrage; 
    while(quantity>=0)//if quantity has become 0 we will be exiting aage hi but we also need to see if price of sell orders exceed our max buy limit price
    {

        if(B.max()->first=="")//price=0 entry no one would put so basically our heap is empty
        break;
        if(B.max()->second[0]<price)//no more elements in the heap to trade with since the max element is lesser than my sell price
        break;
        // if (B.max()->first==name && B.max()->second[0]!=price)//prevent arbitrage
        // {
        //     if(B.max()->first=="")//price=0 entry no one would put so basically our heap is empty
        //     break;//default escape if its not valid 
        //     arbitrage.push_back({B.max()->first,B.max()->second});
        //     B.deleteMax();
        //     //S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
        //     //return;
        // }
        if(quantity<=B.max()->second[2])
        {
             if(B.max()->first=="")//price=0 entry no one would put so basically our heap is empty
            break;
            int p=B.max()->second[0];//just in case calling max twice cleans the top making max inaccessible
            cout<<B.max()->first<<" purchased "<<quantity<<" share of "<<stock<<" from "<<name<<" for $"<<p<<"/share"<<endl;
            total+=p*quantity;
            trades++;
            shares+=quantity;
            for (auto i=accountlist.begin();i<accountlist.end();i++)
            {
                if(i->name==name)
                {
                    i->sell_no+=quantity;
                    i->net+=quantity*p;
                }
                if(i->name==B.max()->first)
                {
                    i->buy_no+=quantity;
                    i->net-=quantity*p;
                }
            }
            B.max()->second[2]-=quantity;
            if(B.max()->second[2]==0)
            B.max()->second[2]=-1;
            quantity=-1;
            return;//if it is possible for the heap to satisfy our order it exits here
        }
        else
        {
             if(B.max()->first=="")//price=0 entry no one would put so basically our heap is empty
            break;
            int p=B.max()->second[0];
            cout<<B.max()->first<<" purchased "<<B.max()->second[2]<<" share of "<<stock<<" from "<<name<<" for $"<<p<<"/share"<<endl;
            trades++;
            shares+=B.max()->second[2];
            total+=B.max()->second[2]*p;
            for (auto i=accountlist.begin();i<accountlist.end();i++)
            {
                if(i->name==name)
                {
                    i->sell_no+=B.max()->second[2];
                    i->net+=B.max()->second[2]*p;
                }
                if(i->name==B.max()->first)

                {
                    i->buy_no+=B.max()->second[2];
                    i->net-=B.max()->second[2]*p;
                }
            }
            quantity-=B.max()->second[2];
            B.max()->second[2]=-1;
        }
    }
    //once it comes here we have not satisfied the entire order so we must add it to the MinHeap of S
    S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
    // for(int i=0;i<arbitrage.size();i++)
    // {
    //     B.insert(arbitrage[i]);
    // }
}

void computeMinHeap(string stock,int price,int time_entry, string name,int quantity,int time_exit,MaxHeap& B,MinHeap& S,vector<accounts>& accountlist,int& trades,int& total,int& shares)//its a min heap implying the new order is a buy order ready to buy from someone with lowest sell price
{
//so basically abhi we have a new buy order which i will buy from the lowest possible sell order if it is valid and has all other quantity and time valid
    vector<pair<string,vector<int>>> arbitrage;
    while(quantity>=0)//if quantity has become 0 we will be exiting aage hi but we also need to see if price of sell orders exceed our max buy limit price
    {
        if(S.min()->first=="")//price=0 entry no one would put so basically our heap is empty
        break;
        if(S.min()->second[0]>price)//no more elements in the heap can be traded with since lowest sell is greater than our buy price
        break;
        // if (S.min()->first==name && S.min()->second[0]!=price)//prevent arbitrage
        // {
        //     if(S.min()->first=="")//price=0 entry no one would put so basically our heap is empty
        //     break;
        //     arbitrage.push_back({S.min()->first,S.min()->second});
        //     S.deleteMin();
        //     //B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
        //     //return;
        // }
        if(quantity<=S.min()->second[2])
        { 
            if(S.min()->first=="")//price=0 entry no one would put so basically our heap is empty
            break;
            int p=S.min()->second[0];
            cout<<name<<" purchased "<<quantity<<" share of "<<stock<<" from "<<S.min()->first<<" for $"<<p<<"/share"<<endl;
            trades++;
            shares+=quantity;
            total+=quantity*p;
            for (auto i=accountlist.begin();i<accountlist.end();i++)
            {
                if(i->name==name)
                {
                    i->buy_no+=quantity;
                    i->net-=quantity*p;
                }
                if(i->name==S.min()->first)
                {
                    i->sell_no+=quantity;  
                    i->net+=quantity*p;
                }
            }
            S.min()->second[2]-=quantity;
            if(S.min()->second[2]==0)
            S.min()->second[2]=-1;
            quantity=-1;
            return;//if it is possible for the heap to satisfy our order it exits here
        }
        else
        {
            if(S.min()->first=="")//price=0 entry no one would put so basically our heap is empty
            break;
            int p=S.min()->second[0];
            cout<<name<<" purchased "<<S.min()->second[2]<<" share of "<<stock<<" from "<<S.min()->first<<" for $"<<p<<"/share"<<endl;
            trades++;
            shares+=S.min()->second[2];
            total+=S.min()->second[2]*p;
            for (auto i=accountlist.begin();i<accountlist.end();i++)
            {
                if(i->name==name)
                {
                    i->buy_no+=S.min()->second[2];
                    i->net-=S.min()->second[2]*p;
                }
                if(i->name==S.min()->first)
                {
                    i->sell_no+=S.min()->second[2];
                    i->net+=S.min()->second[2]*p;
                }
            }
            quantity-=S.min()->second[2];
            S.min()->second[2]=-1;
        }
    }
    //once it comes here we have not satisfied the entire order so we must add it to the MaxHeap of B
    B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
    // for(int i=0;i<arbitrage.size();i++)
    // {
    //     S.insert(arbitrage[i]);
    // }
}

void neworder(int time_entry,string name,string option,string stock,int price, int quantity,int delay,vector<stocks>& stocklist,vector<accounts>& accountlist,int& trades,int& total,int& shares)
//stock is the name of the new incoming stock whereas name is the name of the trader
{

auto j=accountlist.begin();
for(j;j<accountlist.end();j++)
{
    if(checkEquality(j->name,name)==1)
    {

        break;
    }
}
if(j==accountlist.end())//basically we do not have the new guys name in the list of valid accountholders so we will add his acc now so at every computing step for orders each memeber has his account already established
{
    accountlist.push_back({name,0,0,0});
}
if(stocklist.size()==0) 
{
    MaxHeap B;
    MinHeap S;
stocklist.push_back({stock,B,S});
}
auto i=stocklist.begin();
for(i;i<stocklist.end();i++)
{
    if(checkEquality(i->name,stock)==1)
    {
        stock=i->name;
        break;
    }
}
if(i==stocklist.end())
{
    MaxHeap B;
    MinHeap S;
  
    S.sys_time=time_entry;
    B.sys_time=time_entry;   
    
    if (option=="SELL")
    {
        if(delay==-1)
        S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+9999999}});
        else
        S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+delay}});
    }
    if( option=="BUY")
    {
        if(delay==-1)
        B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+9999999}});
        else
        B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+delay}});
    }
    stocklist.push_back({stock,B,S}); 

}
else
{
    i->S.sys_time=time_entry;
    i->B.sys_time=time_entry;   
    
    if (option=="SELL")
    {
        if(delay==-1)
        computeMaxHeap(stock,price,time_entry,name,quantity,time_entry+9999999,i->B,i->S,accountlist,trades,total,shares);
        else
        computeMaxHeap(stock,price,time_entry,name,quantity,time_entry+delay,i->B,i->S,accountlist,trades,total,shares);
    }
    if( option=="BUY")
    {
        if(delay==-1)
        computeMinHeap(stock,price,time_entry,name,quantity,time_entry+9999999,i->B,i->S,accountlist,trades,total,shares);
        else
        computeMinHeap(stock,price,time_entry,name,quantity,time_entry+delay,i->B,i->S,accountlist,trades,total,shares);
    }
}
}
// int main()
// {
//     int n;
//     vector<stocks> stocklist;
//     vector<accounts> accountlist;
//     int trades=0;
//     int total=0;
//     int shares=0;
//     string input;
//     MaxHeap B;
//     MinHeap S;
//     std::cin>>n;//number of entries
//     for (int i=0;i<n;i++)
//     {
//         int time_entry;
//         string name;
//         string option;
//         string stock;
//         int price; 
//         int quantity;
//         int delay;
//         cin>>time_entry>>name>>option>>stock>>price>>quantity>>delay;
//         neworder(time_entry,name, option,stock, price,quantity,delay,stocklist,accountlist,trades,total,shares);
//     }
//     cout<<endl<<"---End of Day---"<<endl;
//     cout<<"Total Amount of Money Transferred: $"<<total<<endl;
//     cout<<"Number of Completed Trades: "<<trades<<endl; 
//     cout<<"Number of Shares Traded: "<<shares<<endl;
//     for(int i=0;i<accountlist.size();i++)
//     {
//         cout<<accountlist[i].name<<" bought "<<accountlist[i].buy_no<<" and sold "<<accountlist[i].sell_no<<" for a net transfer of $"<<accountlist[i].net<<endl;
//     }
//     //computeMaxHeap("AM",100,0,"Kargocorp",30,10,B,S);
//     //computeMinHeap("AM",130,1,"KargoKrab",12,1,B,S);
//     //computeMinHeap("AM",110,1,"KargoKrab",20,1,B,S);
    
    
    
// }

//--------------------------------------------STOCKS.CPP-------------------------------------------------------



market::market(int argc, char** argv)
{
	
}

void market::start()
{
    vector<stocks> stocklist;
    vector<accounts> accountlist; 
     int trades=0;
     int total=0;
     int shares=0;
    vector<string> lines;
    vector<string> inputs;
	std::ifstream inputFile ("output.txt");
    std::string line;
    while (std::getline(inputFile,line)) {
        if (line.compare("TL") == 0) {
            continue;
        }
        if (line.compare("!@") == 0 ) {
            break;
        }
        if(checkOrderValidity(line)==0)
        continue;
        else
        inputs=final_tokenize(tokenize(line));//inputs ke tokens
        neworder(stringToInt(inputs[0]),inputs[1], inputs[2],inputs[3], stringToInt(inputs[4]),stringToInt(inputs[5]),stringToInt(inputs[6]),stocklist,accountlist,trades,total,shares);
    }
    //for (int i=0;i<lines.size();i++)
    // {
    //     inputs=final_tokenize(tokenize(lines[i]));//inputs ke token
    //     neworder(stringToInt(inputs[0]),inputs[1], inputs[2],inputs[3], stringToInt(inputs[4]),stringToInt(inputs[5]),stringToInt(inputs[6]),stocklist,accountlist,trades,total,shares);
    // }
    cout<<endl<<"---End of Day---"<<endl;
    cout<<"Total Amount of Money Transferred: $"<<total<<endl;
    cout<<"Number of Completed Trades: "<<trades<<endl;
    cout<<"Number of Shares Traded: "<<shares<<endl;
    for(int i=0;i<accountlist.size();i++)
    {
        cout<<accountlist[i].name<<" bought "<<accountlist[i].buy_no<<" and sold "<<accountlist[i].sell_no<<" for a net transfer of $"<<accountlist[i].net<<endl;
    }
}
