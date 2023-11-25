#include <iostream>
#include <fstream>
#include <vector> 
#include <string>
#include <chrono>
#include <mutex>
#include <thread>

using namespace std;
extern mutex printMutex;

vector<string> order_book;

int reader(int time)
{
    ifstream inputFile ("output.txt");
    string line;

    int i=0;
    while(i++<1000){
        while(getline(inputFile,line))
            order_book.push_back(line);
        std::lock_guard<std::mutex> lock(printMutex);
        cout<<time << " hi "<<order_book.size()<<endl;
    }

    for(auto i : order_book){
        cout<<i<<endl;
    }
    // 
    // 
    // while () {

    //     if (line.compare("TL") == 0) {
    //         continue;
    //     }
        
    //     if (line.compare("!@") == 0 || line.compare("Successfully Initiated!") == 0) {
    //         break;
    //     }
    //     lines.push_back(line);
    //     // at this stage process all the orders
    //     // cout your order if any
    // }
    return 1;
}

int trader(string *message)
{
    return 1;
}
