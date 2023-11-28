#include "market.h"
#include "iostream"
#include "vector"
#include "string"
#include "fstream"
#include "maxheap.h"
#include "minheap.h"
using namespace std;
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

// removes /r and /n and extra spaces at the end of the line
void removeHiddenCharacters(std::string& str) {
    std::string result;
    for (char c : str) {
        if (c != '\r' && c != '\n') {
            result += c;
        }
    }
    str = result;
    int back = str.size()-1;
    while(str[back--] == ' '){
        str.pop_back();
    }
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
bool checkOrderValidity(string order){

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
    makeCapital(name);

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

    // if(name_tok.size() == 2 && name_tok[1] == "1"){
    //     name = name_tok[0];
    // }

    //now reconstruct order
    // order = "";
    // order = order + tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + name + " " + tokens[tokens.size()-3] + " " + tokens[tokens.size()-2] + " " + tokens[tokens.size()-1];
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
    while(quantity>0)//if quantity has become 0 we will be exiting aage hi but we also need to see if price of sell orders exceed our max buy limit price
    {

        if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
        break;
        if(B.max()->second[0]<price)//no more elements in the heap to trade with since the max element is lesser than my sell price
        break;
        // if (B.max()->first==name && B.max()->second[0]!=price)//prevent arbitrage
        // {
        //     if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
        //     break;//default escape if its not valid 
        //     arbitrage.push_back({B.max()->first,B.max()->second});
        //     B.deleteMax();
        //     //S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
        //     //return;
        // }
        if(quantity<=B.max()->second[2])
        {
             if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
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
            B.max()->second[2]=0;
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
    while(quantity>0)//if quantity has become 0 we will be exiting aage hi but we also need to see if price of sell orders exceed our max buy limit price
    {
        if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
        break;
        if(S.min()->second[0]>price)//no more elements in the heap can be traded with since lowest sell is greater than our buy price
        break;
        // if (S.min()->first==name && S.min()->second[0]!=price)//prevent arbitrage
        // {
        //     if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
        //     break;
        //     arbitrage.push_back({S.min()->first,S.min()->second});
        //     S.deleteMin();
        //     //B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit}});
        //     //return;
        // }
        if(quantity<=S.min()->second[2])
        { 
            if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
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
            S.min()->second[2]=0;
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

//--------------------------------------------STOCKS.CPP--------------------------------------------------------



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
        
        removeHiddenCharacters(line);
        if(!checkOrderValidity(line))
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
