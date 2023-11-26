#include "receiver.h"
#include "string"
#include "trie.cpp"
#include "map.h"
#include<vector> 
#include<bits/stdc++.h>
using namespace std;
#define ALPHABET_SIZE 26

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

void UpperCase(std::string& str) {
for(int i=0;i<str.length();i++)
{
    if (str[i] >= 'a' && str[i] <= 'z')   //checking for lowercase characters
	str[i] = str[i] - 32; 
}
}
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


bool orderoutput(string name,int price,char option,TrieNode* root)
{
    bool flag;
    
    TrieNode* query=search(root,name);
    if (query==NULL)//insert
    {
        insert_trie(root,name,vector<int>{price,0,0});//order is bestprice,bestbuy,bestsell
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

void process_problem1(  TrieNode* root,string message)
{
    string order_name="",temp="";
    int price=0;
    int flagstr=0;

    char option;
    string pricestring="";
    // for (auto i : message)
    // {
    //     if(i==' ')
    //     {

    //         if (flagstr==0)
    //         order_name=temp;
    //         else if (flagstr==1)
    //         pricestring=temp;
    //         flagstr++;
    //         temp="";
    //         continue;
    //     }
    //     if (i=='#')
    //     {
    //         option=temp[0];
    //         break;
    //     }

    //     temp+=i;
    // }

    //     for (int i=0;i<pricestring.size();i++)
    // {
    //     price+=(pricestring[i]-48)*(pow(10,(pricestring.size()-1-i)));
    // }
    vector<string>inputs=tokenize(message);
    // bool flag=orderoutput(order_name,price,option,root);
    removeHiddenCharacters(inputs[0]);
    bool flag=orderoutput(inputs[0],stringToInt(inputs[1]),inputs[2][0],root);
    option=inputs[2][0];
    order_name=inputs[0];
    price=stringToInt(inputs[1]);
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

void process_problem1T(Map root,string message)
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
    bool flag=orderoutput(order_name,price,option,root);
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

void process_problem1T(TrieNode* root,string message)
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
    bool flag=orderoutput(order_name,price,option,root);
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

// int main() {

// Receiver rcv;
//     sleep(5);
//     bool foundDollar = false;
//     int iterator = 0;
//     string message="";

//     while(!foundDollar){
//         std::string msg = rcv.readIML();
//         message+=msg;
//         if(msg.find('$')!= string::npos){
//             rcv.terminate();
//             foundDollar = true;
//         }
//     }

//     //first we store each line in a vector of strings called inputs 

//     vector<string> inputs;
//     inputs.push_back("");
//     int index = 0;  //index of the vector : inputs
//     for(int i = 0; i<message.length(); i++){
//         if(message[i]=='$'){    // edge case where we have the last line
//             inputs.pop_back(); // pops the empty "" and breaks;
//             break;
//         }
//         if(message[i]!='\r') //ignores random characters
//             inputs[index].push_back(message[i]);
//         if(message[i] == '#'){
//             inputs.push_back("");
//             index++;
//         }
//     }

//     // for(int i=0; i<inputs.size(); i++){
//     //     removeHiddenCharacters(inputs[i]);
//     // }

//     //Map M;
//     TrieNode* M = new TrieNode;

//     for(int i=0;i<inputs.size();i++)
//     {
//         //cout<<inputs[i]<<endl;
//         removeHiddenCharacters(inputs[i]);
//         //cout<<inputs[i]<<endl;
//         process_problem1(M,inputs[i]);
//     }


// return 0;
// }



int main() {

string message;

    string line="";
    int n;
    char option;
    vector<string> v;

    TrieNode* M=new TrieNode;

    for ( int i=1;i!=2674;i++)//388 times run
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

