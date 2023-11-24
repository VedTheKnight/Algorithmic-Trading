#include "receiver.h"
#include "string"
#include "map.h"
#include<vector> 
#include<bits/stdc++.h>
using namespace std;
#define ALPHABET_SIZE 26

bool orderoutput(string name,int price,char option,Map M)
{
    bool flag;
    Map* query=M.find(name);
    if (query==nullptr)//insert
    {
        M.insert(name,vector<int>{price,0,0});//order is bestprice,bestbuy,bestsell
        flag= 1;return flag;
    }
    if (option=='b')
    {

        if (query->second[1]<price || query->second[1]==0)//price>best buy and is the new best order cancelling others
        {
            query->second[1]=price;
            if (query->second[1]==query->second[2])
            {
                query->second[1]=0;
                query->second[2]=0;//both trades cancel
                flag= 0;return flag;//best buy and best sell are same so output is (best price,0,0)
            }
            if(query->second[1]>query->second[0])//current trade is best buy and also better than best price 
            {
                query->second[0]=query->second[1];//its the new best price and no pending best buy
                query->second[1]=0;
                flag= 1;return flag;//best buy and best sell are same so output is (updated best price,0,bestsell/)
            }
            else//the current trade is best buy but not the better than best price
            {
                flag= 0;return flag;//output ->no change in your vector and new order just rejected
            }
        }
        else
        {
            flag= 0;return flag;
        }

    }
    if (option=='s')
    {
        if (query->second[2]>price ||query->second[2]==0)//price<best sell and is the new best order cancelling others
        {
            query->second[2]=price;
            if (query->second[1]==query->second[2])
            {
                query->second[1]=0;
                query->second[2]=0;//both trades cancel
                flag= 0;return flag;
            }
            if(query->second[2]<query->second[0])//current trade is best sell and also better than best price
            {
                query->second[0]=query->second[2];//its the new best price and no pending best buy
                query->second[2]=0;
                flag= 1;return flag;
            }
            else//the current trade is best buy but not the better than best price
            {
                flag= 0;return flag;
            }
        }
        else
        {
            flag= 0;return flag;//directly cancelled and not traded on
        }
    }

}

void process_problem1(Map M,string message)
{
    string order_name="",temp="";
    int price=0;
    int flagstr=0;

    char option;
    string pricestring="";
    for (auto i : message)
    {
        if(i==' ')
        {

            if (flagstr==0)
            order_name=temp;
            else if (flagstr==1)
            pricestring=temp;
            flagstr++;
            temp="";
            continue;
        }
        if (i=='#')
        {
            option=temp[0];
            break;
        }

        temp+=i;
    }

        for (int i=0;i<pricestring.size();i++)
    {
        price+=(pricestring[i]-48)*(pow(10,(pricestring.size()-1-i)));
    }
    bool flag=orderoutput(order_name,price,option,M);
    if (flag==1)
    {
        
        if (option=='b')
        cout <<order_name<<" "<< price<<" s"<<endl;
        else
        cout <<order_name<<" "<<price<<" b"<<endl;

    }
    else
    {
       cout<<"No trade"<<endl;
    }

}

int main() {

string message;

    string line="";
    int n;
    char option;
    vector<string> v;

    Map M;

    for ( int i=1;i!=31;i++)//388 times run
    {
        if(i%3==1)
        {
            cin>>line;
        }
        if(i%3==2)
        {
            cin>>n;
        }
        if(i%3==0)
        {
            cin>>option;
            cout<<orderoutput(line,n,option,M)<<endl;
        }
    }

return 0;
}

