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



vector<string> final_tokenize(vector<string> tokens){
    vector<string> final_tokens;
    final_tokens.push_back(tokens[0]);//pushes back the time of entry
    string name;
    int i;
    for( i=1;i<tokens.size();i++)
    {
        if(tokens[i]!="SELL" && tokens[i]!="BUY")
        {
            //name+=tokens[i]+" ";
            continue;
        }
        break;
    }
    //remove last space in the name which will be extra and we dont want it
    //name.pop_back();
    // so now our name has "XYZ -1 ABC 1"
    //IMP at this pt i is basically the i at which i becomes SELL or BUY so at the old values SO 1 to i-1 will have alternate name and coeff
    //name=
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
    while(quantity>0)//if quantity has become 0 we will be exiting aage hi but we also need to see if price of sell orders exceed our max buy limit price
    {

        if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
        break;
        if(B.max()->second[0]<price)//no more elements in the heap to trade with since the max element is lesser than my sell price
        break;
        if (B.max()->first==name && B.max()->second[0]!=price)//prevent arbitrage
        {
            //if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            //break;
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
}

void computeMinHeap(string stock,int price,int time_entry, string name,int quantity,int time_exit,MaxHeap& B,MinHeap& S,vector<accounts>& accountlist,int& trades,int& total,int& shares)//its a min heap implying the new order is a buy order ready to buy from someone with lowest sell price
{
//so basically abhi we have a new buy order which i will buy from the lowest possible sell order if it is valid and has all other quantity and time valid

    while(quantity>0)//if quantity has become 0 we will be exiting aage hi but we also need to see if price of sell orders exceed our max buy limit price
    {
        if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
        break;
        if(S.min()->second[0]>price)//no more elements in the heap can be traded with since lowest sell is greater than our buy price
        break;
        if (S.min()->first==name && S.min()->second[0]!=price)//prevent arbitrage
        {
            //if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            //break;
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
}

void neworder(int time_entry,string name,string option,string stock,int price, int quantity,int delay,vector<stocks>& stocklist,vector<accounts>& accountlist,int& trades,int& total,int& shares)
//stock is the name of the new incoming stock whereas name is the name of the trader
{
auto j=accountlist.begin();
for(j;j<accountlist.end();j++)
{
    if(j->name==name)
    break;
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
    if(i->name==stock)
    break;
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
         inputs=final_tokenize(tokenize(line));//inputs ke tokens
        neworder(stringToInt(inputs[0]),inputs[1], inputs[2],inputs[3], stringToInt(inputs[4]),stringToInt(inputs[5]),stringToInt(inputs[6]),stocklist,accountlist,trades,total,shares);
    }
    //for (int i=0;i<lines.size();i++)
    // {
    //     inputs=final_tokenize(tokenize(lines[i]));//inputs ke tokens
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
