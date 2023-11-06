#include <iostream>
#include <vector>
#include <string>

using namespace std;


//______________________________________Utility Functions___________________________________________________________________________________________________

//simple function to convert string to integer both negative and positive
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

//______________________________________Useful Functions for Part 1________________________________________________________________________________________


//______________________________________Useful Functions for Part 2________________________________________________________________________________________


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

//function to find token in tickers
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

bool checkEquality(string old_order, string name){

    bool flag = false;
   
    vector<string> tokens = tokenize(old_order);
    vector<string> name_tokens = tokenize(name);

    int counter = 0;

    for(int j = 0; j<tokens.size()-2; j++){
        for(int k = 0; k<name_tokens.size();k+=2){
            if(tokens[j] == name_tokens[k] && tokens[j+1] == name_tokens[k+1])
                counter++;
        }
    }

    if(counter == name_tokens.size()/2 && counter == (tokens.size()-2)/2){
        flag = true;
    }

    return flag;
}

void updateOrderBook(vector<string>& order_book, string input){
    vector<string> tokens = tokenize(input); //converts new orders into tokens 
    int price = stringToInt(tokens[tokens.size()-2]); //gets the price of order

    cerr << price<<endl;

    string name=""; // to store the name of the bundle
    char mode= tokens[tokens.size()-1][0];

    for(int i=0; i<tokens.size()-2;i++){
        name+=tokens[i];
        name+=" "; //adding space after every token
    }

    cerr<<name<<endl;

    //gets atmost two matches with buy in first index and sell in second index
    int bestbuy = -1;
    vector<string> bbtokens;
    int bestsell = -1;
    vector<string> bstokens;

    for(int i=0; i<order_book.size(); i++){
        if(checkEquality(order_book[i],name)){ //if it matches we have possible cancellation
            vector<string> line_toks = tokenize(order_book[i]);
            if(line_toks[line_toks.size()-1][0] == 'b'){
                bestbuy = i;
                bbtokens = line_toks;
            }
            if(line_toks[line_toks.size()-1][0] == 's'){
                bestsell = i;
                bstokens = line_toks;
            }
        }
    }
    
    cerr<<bestbuy<<endl;
    cerr<<bestsell<<endl;
    
    if(bestbuy == -1 && bestsell == -1){ //in this case we do not bother and simply add the line
        order_book.push_back(input);
    }

    else if(mode == 'b'){
        //first we compare with its own category
        if(bestbuy == -1){
            //this is the only buy so we check if there is a matching sell which exists otherwise we would not be here
            if(price == stringToInt(bstokens[bstokens.size()-2])){
                //we must cancel the sell and not push in the new order
                order_book.erase(order_book.begin()+bestsell);
            }
            else{ //if their prices do not match we can simply add the new order in
                order_book.push_back(input);
            }
        }
        if(bestbuy != -1){ //in which case we must first check with best buy prices and then best sell
            if(price <= stringToInt(bbtokens[bbtokens.size()-2])){ //if the new order has lower or equal price to the old best buy
                // we straight up dont care about the new order
            }
            else{
                // if the prices is more than the old best buy, the old best buy gets cancelled
                order_book.erase(order_book.begin()+bestbuy);

                // now we add a check if the new order cancels with any previous sell
                if(bestsell == -1){
                    // no cancellation simply add the new order
                    order_book.push_back(input);
                }
                else{
                    if(price == stringToInt(bstokens[bstokens.size()-2])){
                        //we must cancel the sell and not push in the new order
                        order_book.erase(order_book.begin()+bestsell);
                    }
                    else{ //if their prices do not match we can simply add the new order in
                        order_book.push_back(input);
                    }
                }
            }
        }
    }
    else if(mode == 's'){
        //first we compare with its own category
        if(bestsell == -1){
            //this is the only sell so we check if there is a matching buy which exists otherwise we would not be here
            if(price == stringToInt(bbtokens[bbtokens.size()-2])){
                //we must cancel the buy and not push in the new sell order
                order_book.erase(order_book.begin()+bestbuy);
            }
            else{ //if their prices do not match we can simply add the new order in
                order_book.push_back(input);
            }
        }
        if(bestsell != -1){ //in which case we must first check with best sell price and then best buy
            if(price >= stringToInt(bstokens[bstokens.size()-2])){ //if the new order has higher or equal price to the old best sell
                // we straight up dont care about the new order
            }
            else{
                // if the price is less than the old best sell, the old best sell gets cancelled
                order_book.erase(order_book.begin()+bestsell);

                // now we add a check if the new order cancels with any previous sell
                if(bestbuy == -1){
                    // no cancellation simply add the new order
                    order_book.push_back(input);
                }
                else{
                    if(price == stringToInt(bbtokens[bbtokens.size()-2])){
                        //we must cancel the buy and not push in the new order
                        order_book.erase(order_book.begin()+bestbuy);
                    }
                    else{ //if their prices do not match we can simply add the new order in
                        order_book.push_back(input);
                    }
                }
            }
        }
    }
    
    for(int i = 0; i<order_book.size(); i++){
        cerr<<order_book[i];
    }
    cerr<<"\n";
}

int main(){

    vector<string> inputs;
    inputs.push_back("B 1 A 1 10 s#");
    inputs.push_back("A 1 B 1 5 s#");
    inputs.push_back("A 1 B 1 10 b#");
    inputs.push_back("B 1 A 1 C 6 10 s#");
    inputs.push_back("A 1 B 1 5 s#");
    inputs.push_back("A 1 B 1 10 b#");
    inputs.push_back("B 1 C 6 A 1 10 b#");
    inputs.push_back("B 1 A 1 C 6 10 b#");
    inputs.push_back("B 1 10 b#");
    inputs.push_back("B 1 C 6 A 1 10 b#");
    inputs.push_back("A 1 B 1 5 b#");
    // inputs.push_back("B 1 10 b#");
    // inputs.push_back("A 2 B 1 10 s#");

    // Here we start the logic
    // If we have k lines in our input, we will do 2^k checks for arbitrage
    // In each check, we will go through all our tokens in the active strings and add it to a 
    // map: ticker->amnt assuming all as b orders. We check for cost and add it into a vector.
    
    vector<string> order_book;

    int total_profit = 0;

    for(int i=0; i<inputs.size(); i++){
        //NEED TO INSERT CODE FOR PROCESSING HERE FIRST -- WAIT FOR BRIAN LOGIC SIMILAR TO PHASE 1
        updateOrderBook(order_book, inputs[i]);

        //find the best arbitrage
        vector<int> best_arbitrage = findBestArbitrage(order_book, order_book.size(),total_profit); //given the number of lines

        bool flag = true;
        for(int j=0; j<best_arbitrage.size(); j++){
            if(best_arbitrage[j]==1){
                flag = false;
                break;
            }
        }

        if(flag){
            cout<<"No Trade\n";
            continue;
        }

        
        // if there exists an arbitrage
        // we display the arbitrage in reverse order
        for(int iterator = order_book.size()-1; iterator>=0; iterator--){
            if(best_arbitrage[iterator] == 1){ // this is a part of our arbitrage so we display it
                order_book[iterator][order_book[iterator].size()-2] = (order_book[iterator][order_book[iterator].size()-2]=='b')? 's':'b'; // assuming new line character is there
                cout<<order_book[iterator]<<"\n";
            }
        }

        // we delete the lines from the order book in reverse order
        for(int j=order_book.size()-1; j>=0; j--){
            if(best_arbitrage[j]==1){
                order_book.erase(order_book.begin()+j);
            }
        }
    }

    cout<<total_profit;
    return 0;
}

