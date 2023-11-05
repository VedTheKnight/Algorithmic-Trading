#include <iostream>
#include <vector>
#include <string>
#include "receiver.h"
using namespace std;

//last token doesnt include \n
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

//given a number n and k being the size of inputs upto that point, get binary representation of n
vector<int> getBinary(int n, int k){

    string binary="";

    while(n>0){
        binary = to_string(n%2) + binary;
        n = n/2;
    }


    vector<int> binary_vec(k,0);

    for(int i=0; i<binary.size(); i++){
        binary_vec[k-1-i] = binary[binary.size()-1-i] - 48;
    }
    
    return binary_vec;
}

//simple function to convert string to integer
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

// function to find token in tickers
int findTicker(vector<string> tickers, string token){
    for(int i=0;i<tickers.size(); i++){
        if(tickers[i] == token){
            return i;
        }
    }
    return -1;
}

// checks if a given combination of inputs is a zero sum combo
bool isZeroSum(vector<string> combination){

    vector<string> tickers;

    //first we filter out all the tickers
    for(auto line : combination){
        vector<string> tokens = tokenize(line);
        for(int i = 0; i<tokens.size()-2; i+=2){
            if(i%2 == 0 && findTicker(tickers,tokens[i])==-1){ //if string and it is not in tickers already we add it into tickers
                tickers.push_back(tokens[i]);
            }
        }
    }

    vector<int> corresponding_quantities(tickers.size(), 0);

    //now that we have the tickers we again go through the combinations and compute the corresponding quantities for the ticker
    for(auto line : combination){
        vector<string> tokens = tokenize(line);
        for(int i = 1; i<tokens.size()-2; i+=2){
            if(i%2 == 1){ //if string and it is not in tickers already we add it into tickers
                int it = findTicker(tickers, tokens[i-1]);
                if(tokens[tokens.size()-1][0] == 'b')
                    corresponding_quantities[it] += stringToInt(tokens[i]);
                else //in case it is a sell so we just invert it
                    corresponding_quantities[it] -= stringToInt(tokens[i]);
            }
        }
    }

    // now if all the corresponding quantities are zero then we are done
    for(auto i:corresponding_quantities){
        if(i!=0)
            return false;
    }
    return true;
}

//finds the best arbitrage given valid orders upto a point and the number of lines
vector<int> findBestArbitrage(vector<string> inputs, int k, int& total_profit){

    //code to find best arbitrage given k input lines

    vector<int> best_arbitrage(k,0);
    int max_profit = 0;

    int pow = 1;
    for(int i=1; i<=k;i++){
        pow*=2;
    } //gets 2^k which is the number of states we iterate over

    for(int arb=1; arb<pow; arb++){ //arbitrage 0 is useless so we go from 1 to 2^k-1
        //first we find the binary representation of the arbitrage "arb"
        vector<int> binary_arbitrage = getBinary(arb,k); //arb is an integer and k is the size of the vector we receive ultimately
        int profit = 0;

        // now we check if the given comibination of lines in put forms a positive sum arbitrage or not
        // if it does and sum > previous max sum we store it as the best arbitrage
        
        //get relevant lines
        vector<string> possible_arbitrage;
        for(int i=0; i<k; i++){
            if(binary_arbitrage[i]==1){
                possible_arbitrage.push_back(inputs[i]);
            }
        }

        //first we do a profit computation
        for(auto line: possible_arbitrage){
            vector<string> line_tokens = tokenize(line);
            if(line_tokens[line_tokens.size()-1][0] == 'b'){
                profit += stringToInt(line_tokens[line_tokens.size()-2]);
            }else{
                profit -= stringToInt(line_tokens[line_tokens.size()-2]);
            }
        }

        //now if this cost is greater than max_profit, and it is an arbitrage
        if(profit>max_profit && isZeroSum(possible_arbitrage)){
            max_profit = profit;    //update the max profit
            best_arbitrage = binary_arbitrage;
        }

    }

    total_profit+=max_profit;
    return best_arbitrage;
}

int main(){

    Receiver rcv;
    sleep(5);
    bool foundDollar = false;
    int iterator = 0;
    string message="";

    while(!foundDollar){
        std::string msg = rcv.readIML();
        message+=msg;
        if(msg.find('$')!= string::npos){
            rcv.terminate();
            foundDollar = true;
        }
    }

    vector<string> inputs;
    inputs.push_back("");
    int index = 0;  //index of the vector : inputs
    for(int i = 0; i<message.length(); i++){
        if(message[i]=='$'){    // edge case where we have the last line
            inputs.pop_back(); // pops the empty "" and breaks;
            break;
        }
        if(message[i]!='\r') //ignores random characters
            inputs[index].push_back(message[i]);
        if(message[i] == '#'){
            inputs.push_back("");
            index++;
        }
    }

    for(int i = 0; i<inputs.size(); i++){
        cout<<tokenize(inputs[i])[0]<<endl;
    }
    
    return 0;
}