// Listening to a given port no 8888 and printing the incoming messages
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include "minheap.h"
#include "maxheap.h"
using namespace std;
//--------------------------------------------------------------------------GLOBAL STUFF---------------------------------------------------------------------
const int BUFFER_SIZE = 1024;
int NUM_THREADS = 2;
struct stocks
{
    string name;
    vector<MaxHeap> BMarkets;
    vector<MinHeap> SMarkets;
};
vector<stocks> stocklist;
//-------------------------------------------------------------------------GlOBAL STUFF-----------------------------------------------------------------------
//-------------------------------------------------------------------STRING PROCESSING----------------------------------------------------------------------
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


//--------------------------------------------STRINGPROCESS-----------------------------------------------------

//--------------------------------------------STOCKS.CPP--------------------------------------------------------


//u->(price,time-entry,quantity,time exit)
void computeMaxHeap(string stock,int price,int time_entry, string name,int quantity,int time_exit,MaxHeap& B,MinHeap& S,int market)//its a max heap implying the new order is a sell order ready to sell to someone with highest buy price
{//name is the new order wala 
//so basically abhi we have a new sell order which i will sell to the highest possible buy order if it is valid and has all other quantity and time valid
// fix the issue of infinite time
    // vector<pair<string,vector<int>>> arbitrage; 
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
            //cout<<B.max()->first<<" purchased "<<quantity<<" share of "<<stock<<" from "<<name<<" for $"<<p<<"/share"<<endl;
            B.max()->second[2]-=quantity;
            quantity=0;
            return;//if it is possible for the heap to satisfy our order it exits here
        }
        else
        {
             if(B.max()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            break;
            int p=B.max()->second[0];
            //cout<<B.max()->first<<" purchased "<<B.max()->second[2]<<" share of "<<stock<<" from "<<name<<" for $"<<p<<"/share"<<endl;
            quantity-=B.max()->second[2];
            B.max()->second[2]=0;
        }
    }
    //once it comes here we have not satisfied the entire order so we must add it to the MinHeap of S
     S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit,market}});
    // for(int i=0;i<arbitrage.size();i++)
    // {
    //     B.insert(arbitrage[i]);
    // }
}

void computeMinHeap(string stock,int price,int time_entry, string name,int quantity,int time_exit,MaxHeap& B,MinHeap& S,int market)//its a min heap implying the new order is a buy order ready to buy from someone with lowest sell price
{
//so basically abhi we have a new buy order which i will buy from the lowest possible sell order if it is valid and has all other quantity and time valid
    // vector<pair<string,vector<int>>> arbitrage;
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
            //cout<<name<<" purchased "<<quantity<<" share of "<<stock<<" from "<<S.min()->first<<" for $"<<p<<"/share"<<endl;
            S.min()->second[2]-=quantity;
            quantity=0;
            return;//if it is possible for the heap to satisfy our order it exits here
        }
        else
        {
            if(S.min()->second[0]==0)//price=0 entry no one would put so basically our heap is empty
            break;
            int p=S.min()->second[0];
            //cout<<name<<" purchased "<<S.min()->second[2]<<" share of "<<stock<<" from "<<S.min()->first<<" for $"<<p<<"/share"<<endl;
            quantity-=S.min()->second[2];
            S.min()->second[2]=0;
        }
    }
    //once it comes here we have not satisfied the entire order so we must add it to the MaxHeap of B
     B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_exit,market}});
    // for(int i=0;i<arbitrage.size();i++)
    // {
    //     S.insert(arbitrage[i]);
    // }
}

void neworder(int time_entry,string name,string option,string stock,int price, int quantity,int delay,vector<stocks>& stocklist,int market)
//stock is the name of the new incoming stock whereas name is the name of the trader
{

if(stocklist.size()==0)
{
    MaxHeap B;
    B.market=market;
    MinHeap S;
    S.market=market;
    vector<MinHeap>SMarket;
    vector<MaxHeap>BMarket;
    SMarket.push_back(S);
    BMarket.push_back(B);
stocklist.push_back({stock,BMarket,SMarket});
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
    B.market=market;
    MinHeap S;
    S.market=market;
    vector<MinHeap>SMarket;
    vector<MaxHeap>BMarket;
    SMarket.push_back(S);
    BMarket.push_back(B);


    S.sys_time=time_entry;
    B.sys_time=time_entry;   
    
    if (option=="SELL")
    {
        if(delay==-1)
        S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+9999999,market}});
        else
        S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+delay,market}});
    }
    if( option=="BUY")
    {
        if(delay==-1)
        B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+9999999,market}});
        else
        B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+delay,market}});
    }
    stocklist.push_back({stock,BMarket,SMarket});

}
else
{
    int j;
    for(j=0;j<i->SMarkets.size();j++)
    if(i->SMarkets[j].market==market)
    break;
    if(j==i->SMarkets.size())//basically no market matched so its a new market
{
    MaxHeap B;
    B.market=market;
    MinHeap S;
    S.market=market;
    i->SMarkets.push_back(S);
    i->BMarkets.push_back(B);


    S.sys_time=time_entry;
    B.sys_time=time_entry;   
    
    if (option=="SELL")
    {
        if(delay==-1)
        S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+9999999,market}});
        else
        S.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+delay,market}});
    }
    if( option=="BUY")
    {
        if(delay==-1)
        B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+9999999,market}});
        else
        B.insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+delay,market}});
    }
}
else
{
    
    i->BMarkets[j].sys_time=time_entry;
    i->SMarkets[j].sys_time=time_entry; 
     if (option=="SELL")
    {
        if(delay==-1)
        i->SMarkets[j].insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+9999999,market}});
        else
        i->SMarkets[j].insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+delay,market}});
    }
    if( option=="BUY")
    {
        if(delay==-1)
        i->BMarkets[j].insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+9999999,market}});
        else
        i->BMarkets[j].insert(pair<string,vector <int>>{name,{price,time_entry,quantity,time_entry+delay,market}});
    }   
}
}
//now compute the stocks se self arbitrage
{
    int j;
     for(j=0;j<i->BMarkets.size();j++)
     {
         //i->SMarkets[j].sys_time=time_entry;
         //i->BMarkets[j].sys_time=time_entry;
     }
    // for(j=0;j<i->BMarkets.size();j++)
    // for(int k=j+1;k<i->BMarkets.size();k++)
    // {
    //     while(i->SMarkets[j].min()->second[0]<i->BMarkets[k].max()->second[0])
    //     {
    //         //go to markets and print
    //         cout<<"hi";
    //         i->SMarkets[j].deleteMin();
    //         i->BMarkets[k].deleteMax();
    //     }
    //     while(i->SMarkets[k].min()->second[0]<i->BMarkets[j].max()->second[0])
    //     {
    //         //go to markets and print
    //         cout<<"hi";
    //         i->SMarkets[k].deleteMin();
    //         i->BMarkets[j].deleteMax();
    //     }
    // }
}
}
//---------------------------------------------------------------------STOCK.CPP-------------------------------------------------------------------------------
// Structure to store client socket and its details
struct ClientInfo {
    int socket;
    struct sockaddr_in address;
    ClientInfo(int socket, struct sockaddr_in& address) : socket(socket), address(address) {}
    ClientInfo() {};
};

// Function to handle a client connection
void *handleClient(void *arg) {
    ClientInfo *clientInfo = static_cast<ClientInfo *>(arg);
    char buffer[BUFFER_SIZE];

    std::cout << "Connected to client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;

    while (true) {
        // Receive data from the client
        ssize_t bytesRead = recv(clientInfo->socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            // Error or connection closed
            if (bytesRead == 0) {
                std::cout << "Connection closed by client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;
            } else {
                perror("Recv error");
            }
            break;
        } else {
            // Print the received message
            buffer[bytesRead] = '\0';
            std::cout << "Received message from client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << ": " << buffer << std::endl;
            vector<string> inputs;
             inputs=final_tokenize(tokenize(buffer));
             int market=ntohs(clientInfo->address.sin_port);
             neworder(stringToInt(inputs[0]),inputs[1], inputs[2],inputs[3], stringToInt(inputs[4]),stringToInt(inputs[5]),stringToInt(inputs[6]),stocklist,market);
        }
    }

    // Close the client socket
    close(clientInfo->socket);
    delete clientInfo;
    pthread_exit(NULL);
}

int main()
{
    for(int i=0;i<14;i++)
    {
        string line;
        int market;
        cin>>market;
        cin>>line;
        vector<string> inputs;
        inputs=final_tokenize(tokenize(line));
        neworder(stringToInt(inputs[0]),inputs[1], inputs[2],inputs[3], stringToInt(inputs[4]),stringToInt(inputs[5]),stringToInt(inputs[6]),stocklist,market);
    }
}

// int main() {
//     int serverSocket;
//     struct sockaddr_in serverAddr, clientAddr;
//     socklen_t clientAddrLen = sizeof(clientAddr);

//     // Create server socket
//     if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//         perror("Socket creation error");
//         exit(EXIT_FAILURE);
//     }

//     // Initialize server address struct
//     memset(&serverAddr, 0, sizeof(serverAddr));
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(10000);  // Port number
//     serverAddr.sin_addr.s_addr = INADDR_ANY;

//     // Bind server socket to the specified address and port
//     if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
//         perror("Bind error");
//         exit(EXIT_FAILURE);
//     }

//     // Listen for incoming connections
//     if (listen(serverSocket, 5) == -1) {  // Maximum 5 pending connections
//         perror("Listen error");
//         exit(EXIT_FAILURE);
//     }

//     std::cout << "Trader is listening on port 8888..." << std::endl;

//     std::vector<pthread_t> clientThreads;

//     for(int i = 0; i < NUM_THREADS; i++) {
//         // Accept incoming connections
//         int clientSocket;
//         if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1) {
//             perror("Accept error");
//             continue;  // Continue listening for other connections
//         }

//         // Create a thread to handle this client
//         ClientInfo *clientInfo = new ClientInfo(clientSocket, clientAddr);
//         pthread_t clientThread;
//         if (pthread_create(&clientThread, NULL, handleClient, clientInfo) != 0) {
//             perror("Thread creation error");
//             delete clientInfo;
//             continue;  // Continue listening for other connections
//         }

//         // Store the thread ID for later joining
//         clientThreads.push_back(clientThread);
//     }

//     // Join all client threads (clean up)
//     for (auto &thread : clientThreads) {
//         pthread_join(thread, NULL);
//     }

//     // Close the server socket (never reached in this example)
//     close(serverSocket);

//     return 0;
// }
