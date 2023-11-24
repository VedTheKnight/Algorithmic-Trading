#include "map.h"
#include <iostream>
using namespace std;

int main()
{
    Map M;
    M.insert("ABC",vector <int> {1,2,3});
    M.insert("DEF",vector <int> {4,5,6});
    M.insert("GHI",vector <int> {7,8,9});
    M.insert("JKL",vector <int> {10,11,12});
    M.insert("JKLF",vector <int> {10,11,12});
    cout<<M["GHI"][1]<<endl;
    cout<<M.count("JKL")<<endl;
    cout<<M.count("A")<<endl;
    M.update("JKL",vector <int> {1,1,1});
    cout<<(M.find("JKL")->second)[2];

}