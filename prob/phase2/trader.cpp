// This is your trader. Place your orders from here
#include <iostream>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream>

using namespace std;
extern mutex printMutex;
extern std::atomic<int> commonTimer;

#include <string>


int reader(int time)
{
    int index=0;
// std::lock_guard<std::mutex> lock(printMutex);
// std::cout << time << " Harry SELL AMD $1 #32 2" << std::endl;
    vector<string> order_book;
    string line;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    bool flag = true;
    while(flag){
        ifstream inputFile("output.txt");
        int ctr = 0;
        while(getline(inputFile,line)){
            if (line.compare("TL") == 0) {
                continue;
            }
            if(line.compare("!@") == 0 || line.compare("@!") == 0 || line.compare("Successfully Initiated!") == 0 ){
                flag = false;
                break;
            }

                order_book.push_back(line);
            
        }
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout<<order_book[index]<<std::endl;
        index+=2;
        if(!flag)
            break;
        //cout<<commonTimer.load()<<" VD SELL AMD $1 #32 "<<order_book.size()<<endl;
        //TESTING
        // std::lock_guard<std::mutex> lock(printMutex);
        // for(auto line:order_book){
        //     cout<<line<<endl;
        // }
        // cout<<"hi"<<order_book.size()<<endl;

        // now that we have the order book let us run phase 1 part 3 on it 
        // find the best arbitrage and update the quantities in the order book as well and update the total profit

        std::this_thread::sleep_for(std::chrono::seconds(1));
        order_book.clear();
    }
return 1;

}

int trader(std::string *message)
{

}
