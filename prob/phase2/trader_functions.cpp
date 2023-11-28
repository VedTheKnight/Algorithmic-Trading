#include <iostream>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream>
#include <algorithm>
using namespace std;

//___________________________________________________Utility Functions_____________________________________________________________________________________________________________________________________________________________________________________________________________________________________________

// removes /r and /n and extra spaces at the end of the line
void removeHC(std::string& str) {
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

//locates a line in order book returns index if found, returns -1 if not present
int find(vector<string> order_book, string line){
    if(order_book.size() == 0)
        return -1;

    for(int i = 0; i<order_book.size(); i++){
        if(order_book[i] == line){
            return i;
        }
    }
    return -1;
}

//simple function to convert string to integer both negative and positive
int s2I(const std::string& str) {
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
vector<string> tokZ(string input){

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

//function to find token in tickers
int findTicker(vector<string> tickers, string token){
    for(int i=0;i<tickers.size(); i++){
        if(tickers[i] == token){
            return i;
        }
    }
    return -1;
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

// prints the order to terminal
void printOrder(vector<string> line_toks){
    string order="";
    order = order + to_string(-69420) + " ";
    order = order + "VD ";
    order += (line_toks[2] == "BUY")? "SELL":"BUY";
    order += " ";

    for(int i=3; i<line_toks.size();i++){
        if(line_toks[i][0] == '#'){
            break;
        }
        order+= line_toks[i];
        order+= " ";
    }
    
    //now concatenate the quantity
    //order += "#";
    order += line_toks[line_toks.size()-2];
    order += " ";

    order += line_toks[line_toks.size()-1];

    //std::lock_guard<std::mutex> lock(printMutex);
    std::cout<<order<<" \n";
}


//______________________________________Arbitrage Functions__________________________________________________________________________________________________________________________________________________________________________________________________________________

// function to check for matching stock combo : imported from phase 2 part 1
bool checkMatching(string old_order, string name){

    bool flag = false;
   
    vector<string> tokens = tokZ(old_order);
    vector<string> name_tokens = tokZ(name);

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

// checks if any stock name is an exact match in the combination with one buy and one sell, in which case the arbitirage will never exist
// works for single line as well, can be used in median trading
bool detectError(vector<string> combination){

    // for single size combination
    if(combination.size() == 1){
        vector<string> tokens = tokZ(combination[0]);
        if(tokens[tokens.size()-1] == "-1")
            return 0;
        if(-69420 > s2I(tokens[0]) + s2I(tokens[tokens.size()-1])){ //expired order
            return 1;
        }
    }

    //checks for invalid time
    for(int i = 0; i < combination.size(); i++){
        vector<string> tokens = tokZ(combination[i]);
        if(tokens[tokens.size()-1] == "-1")
            continue;
        if(-69420 > s2I(tokens[0]) + s2I(tokens[tokens.size()-1])){
            return 1;
        }
    }

    for(int i=0; i<combination.size(); i++){
        vector<string> tok1 = tokZ(combination[i]);
        string s1;
        //compile the bundle 1
        int it = 3;
        while(tok1[it][0]!='$'){
            s1+=tok1[it];
            s1+=" ";
            it++;
        }
        s1.pop_back();
        for(int j = i+1; j<combination.size(); j++){
            vector<string> tok2 = tokZ(combination[j]);

            if(tok1[2] != tok2[2] && tok1[1] != tok2[1]){ // we have a possible cancellation if BUY SELL combination not by the same person
                //the stock bundle is the same then we have a match anyways                
                string s2;
                it = 3;
                while(tok2[it][0]!='$'){
                    s2+=tok2[it];
                    s2+=" ";
                    it++;
                }
                s2.pop_back();
                if(checkMatching(s1,s2))
                    return 1;
            }
        }
    }
    return 0;
}

// recursive function to generate all possible quantities
void generateQuantities(vector<int>& inputArray, vector<int>& currentArray, int index, vector<vector<int>>& result) {
    if(index == inputArray.size()){
        result.push_back(currentArray);
        return;
    }

    for(int i = 1; i <= inputArray[index]; i++){
        currentArray[index] = i;
        generateQuantities(inputArray, currentArray, index + 1, result);
    }
}

// checks if a given combination of inputs could lead to a possible arbitrage and outputs null vector if no 
// and returns a vector of optimal quantities if yes. 
vector<int> checkArbitrage(vector<string> combination){

    vector<string> tickers;
    vector<int> quantities;
    vector<int> trunc_quantities;

    if(detectError(combination))
        return vector<int>(combination.size(),0);
    
    //first we filter out all the tickers and all the quantities 
    for(auto line : combination){
        vector<string> tokens = tokZ(line);
        trunc_quantities.push_back(min(5,s2I(tokens[tokens.size()-2].substr(1,tokens[tokens.size()-2].size()-1))));
        quantities.push_back(s2I(tokens[tokens.size()-2].substr(1,tokens[tokens.size()-2].size()-1)));
        for(int i = 3; i<tokens.size()-3; i+=1){
            if(isAlphabetical(tokens[i]) && findTicker(tickers,tokens[i])==-1){ //if string and it is not in tickers already we add it into tickers
                tickers.push_back(tokens[i]);
            }
        }
    }

    vector<vector<int>> quantity_combinations;

    // we need to fill in the code to get all the possible quantity combinations
    vector<int> currentArray(quantities.size(), 0);
    generateQuantities(trunc_quantities, currentArray, 0, quantity_combinations);

    //now that we have the tickers we again go through the combinations and compute the corresponding quantities for the ticker
    for(auto quantity_combo : quantity_combinations){
        vector<int> corresponding_quantities(tickers.size(), 0);

        for(int j=0; j<combination.size(); j++){ //we go through all the lines so the correspondiing quantity is quantity_combo[j]
            string line = combination[j];
            vector<string> tokens = tokZ(line);
            for(int i = 3; i<tokens.size()-3; i+=1){
                if(isAlphabetical(tokens[i])){ // update the quantities based on the bundle quantity and the quantity of the stock in the bundle itself
                    int it = findTicker(tickers, tokens[i]);
                    if(isAlphabetical(tokens[i+1]) || tokens[i+1][0] == '$'){ //quantity of 1
                        if(tokens[2] == "BUY")
                            corresponding_quantities[it] += quantity_combo[j];
                        else //in case it is a sell so we just invert it
                            corresponding_quantities[it] -= quantity_combo[j];
                    }
                    else{ //normal case where the next is integral
                        if(tokens[2] == "BUY")
                            corresponding_quantities[it] += quantity_combo[j]*s2I(tokens[i+1]);
                        else //in case it is a sell so we just invert it
                            corresponding_quantities[it] -= quantity_combo[j]*s2I(tokens[i+1]);
                    }
                    
                }
            }
        }

        // now if all the corresponding quantities are zero then we are done
        int ctr = 0;
        for(auto i:corresponding_quantities){
            if(i==0)
                ctr++;
        }

        if(ctr == corresponding_quantities.size()){ //i.e there exists some combination that makes all the tickers have 0 quantity
            // we must output the max possible multiple of quantity combo present in quantity combinations
            bool flag = true;
            int x = 1;
            while(flag){
                x++;
                for(int i=0; i<quantity_combo.size(); i++){
                    if(quantity_combo[i]*x>quantities[i]){
                        flag = false;
                    }
                }
            }

            for(int i=0; i<quantity_combo.size(); i++){
                quantity_combo[i] *= (x-1);
            }

            return quantity_combo;

        }
        else{
            continue; // check other combinations
        }
    }
    
    return vector<int>(combination.size(),0);
}

// finds the best arbitrage given valid orders upto a point and the number of lines
// returns the relevant orders and their corresponding quantities
vector<pair<int,int>> findBestArbitrage(vector<string> inputs, int k){

    //code to find best arbitrage given k input lines

    vector<int> best_arbitrage(k,0);
    int max_profit = 0;
    vector<int> best_quantities(k,0);

    int pow = 1;
    for(int i=1; i<=k;i++){
        pow*=2;
    } //gets 2^k which is the number of states we iterate over

    for(int arb=1; arb<pow; arb++){ //arbitrage 0 is useless so we go from 1 to 2^k-1
        //first we find the binary representation of the arbitrage "arb"
        vector<int> binary_arbitrage = getBinary(arb,k); //arb is an integer and k is the size of the vector we receive ultimately
        int profit = 0;

        // get relevant lines
        vector<string> possible_arbitrage;
        for(int i=0; i<k; i++){
            if(binary_arbitrage[i]==1){
                possible_arbitrage.push_back(inputs[i]);
            }
        }

        //we first compute the possible linear combinations which satisfy are zero sum. 
        //calling this function gets us the optimal maximum quantities for the given combination corresponding to the
        //possible arbitrage vector

        vector<int> quantities = checkArbitrage(possible_arbitrage);

        int zcount = 0;
        for(int i = 0; i<quantities.size(); i++){
            if(quantities[i]==0)
                zcount++;
        }

        if(zcount == quantities.size()) //all zeros which means no arbitrage
            continue;
        
        //if we reach here it means we have a possible arbitrage on our hands
        //first we do a profit computation
        int ctr = 0;
        for(auto line: possible_arbitrage){
            vector<string> line_tokens = tokZ(line);
            if(line_tokens[2] == "BUY"){
                profit += quantities[ctr]*s2I(line_tokens[line_tokens.size()-3].substr(1,line_tokens[line_tokens.size()-3].size()-1)); //price is at minus 3, quantity at minus 2
            }else{
                profit -= quantities[ctr]*s2I(line_tokens[line_tokens.size()-3].substr(1,line_tokens[line_tokens.size()-3].size()-1));
            }
            ctr++;
        }

        //now if this cost is greater than max_profit, and it is an arbitrage
        if(profit>max_profit && zcount != quantities.size()){
            max_profit = profit;    //update the max profit
            best_arbitrage = binary_arbitrage;
            int x = 0;
            for(int i=0; i<best_arbitrage.size(); i++){
                best_quantities[i] = (best_arbitrage[i] == 1)? quantities[x++]:0; //copies in the quantities vector
            }
        }
    }

    vector<pair<int,int>> best;
    for(int i=0; i<best_arbitrage.size(); i++){
        best.push_back(make_pair(best_arbitrage[i], best_quantities[i]));
    }

    return best;
}

//runs our trading algorithm
void trade(vector<string> window, vector<pair<string,vector<int>>> stockline){

    vector<pair<int,int>> best_arbitrage = findBestArbitrage(window, window.size()); //given the number of lines

    //to check if valid arbitrage not present in which case output no trade and continue
    bool flag = true;
    for(int j=0; j<best_arbitrage.size(); j++){
        if(best_arbitrage[j].first==1){
            flag = false;
            break;
        }
    }

    if(flag){ //this is the case where there is no arbitrage, so we must employ a median trading strategy

        for(auto line : window){
            if(detectError(vector<string>{line}))
                continue; //no point of executing any trades there

            //now if the line is tradable, we try and trade with it, in a way as to maximize our profits
            vector<string> tokens = tokZ(line);
            string name = getStockName(tokens);
            string type = tokens[2];
            int price = s2I(tokens[tokens.size()-3]);
            
            // fetch median
            double median_price = 0;
            for(auto stock:stockline){
                if(checkMatching(stock.first,name)){
                    median_price = (stock.second.size()%2==0)? ((stock.second[stock.second.size()/2]+stock.second[stock.second.size()/2-1])/2.0):stock.second[stock.second.size()/2];
                    break;
                }
            }

            if(type == "BUY"){
                // we want to sell above the median price
                if(price >= median_price){
                    printOrder(tokens);
                }
            }
            else{
                if(price <= median_price){
                    printOrder(tokens);
                }
            }


        }
        return; //once we go through the lines we return since no arbitrage anyway 
    }
    
    // if there exists an arbitrage
    // we display the arbitrage in reverse order
    // as we display we update the quantities in the order book and delete the ones where quantities become zero
    int min_time_to_expiry = 2147483647;
    for(int iterator = 0; iterator<window.size(); iterator++){
        if(best_arbitrage[iterator].first == 1 && s2I(tokZ(window[iterator])[tokZ(window[iterator]).size()-1])!= -1)
            min_time_to_expiry = min(min_time_to_expiry, s2I(tokZ(window[iterator])[tokZ(window[iterator]).size()-1]));
    }
    if(min_time_to_expiry == 2147483647)
        min_time_to_expiry = -1;
    for(int iterator = window.size()-1; iterator>=0; iterator--){
        if(best_arbitrage[iterator].first == 1){ // this is a part of our arbitrage so we display it

            vector<string> line_toks = tokZ(window[iterator]);
            string order="";
            order = order + to_string(-69420) + " ";
            order = order + "VD ";
            order += (line_toks[2] == "BUY")? "SELL":"BUY";
            order += " ";

            for(int i=3; i<line_toks.size();i++){
                if(line_toks[i][0] == '#'){
                    break;
                }
                order+= line_toks[i];
                order+= " ";
            }
            
            //now concatenate the quantity
            order += "#";
            order += to_string(best_arbitrage[iterator].second);
            order += " ";

            order += to_string(min_time_to_expiry);

            //std::lock_guard<std::mutex> lock(printMutex);
            std::cout<<order<<" \n";
        }
    }
}

//_______________________________________________Moving Median Functions______________________________________________________________________________________________________________________________________________________________________________________________________

void CollectInfo(vector<pair<string,vector<int>>>& stockline, vector<string> window){
    for(auto order : window){
        vector<string> tokens = tokZ(order);
        string name = getStockName(tokens);
        int price = s2I(tokens[tokens.size()-3].substr(1,tokens[tokens.size()-3].size()-1));
        int quantity = s2I(tokens[tokens.size()-2].substr(1,tokens[tokens.size()-2].size()-1));

        bool flag = 0;
        for(auto& line : stockline){
            if(checkMatching(line.first,name)){
                if(line.second.size() == 0){ //the case where the first occurence has 0 size
                    for(int j = 1; j <= quantity; j++){
                        line.second.push_back(price);
                    }
                    flag = 1;
                    break;
                }
                bool check = false;
                for(int i=0; i<line.second.size(); i++){
                    if(line.second[i] >= price){
                        check = true;
                        for(int j = 1; j <= quantity; j++){
                            line.second.insert(line.second.begin() + i, price);
                        }
                        break;
                    }
                }
                //the case where we end up with max value as the newest
                if(!check){
                    for(int j = 1; j <= quantity; j++){
                        line.second.push_back(price);
                    }
                }
                flag = 1;
                break;
            }
        }

        if(flag == 0)
            stockline.push_back(make_pair(name, vector<int>(quantity,price)));
    }
}


//_____________________________________________________test______________________________________________________________________________________________________________________________________________

// Function to generate a random integer in a given range [min, max]
int getRandomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Function to generate a random stock ticker
std::string getRandomStock() {
    std::vector<std::string> stocks = {"AAPL", "GOOGL", "AMZN", "NFLX", "MSFT", "TSLA"};
    int index = getRandomInt(0, stocks.size() - 1);
    return stocks[index];
}

// Function to generate a random order type (BUY or SELL)
std::string getRandomOrderType() {
    return (getRandomInt(0, 1) == 0) ? "BUY" : "SELL";
}

// Function to generate a random stock order
std::string generateRandomStockOrder() {
    std::string order;

    // Time of entry
    order += std::to_string(getRandomInt(1, 100)) + " ";

    // Extra time (set to -1 for active forever)
    order += "-1 ";

    // Corporation (randomize)
    order += "OT" + std::to_string(getRandomInt(1, 10)) + " ";

    // Order type (BUY or SELL)
    order += getRandomOrderType() + " ";

    // Stock bundle (randomize quantity and stock tickers)
    std::vector<std::string> stockTickers;
    for (int i = 0; i < getRandomInt(2, 5); ++i) {
        std::string stock = getRandomStock();
        // Introduce a chance to reuse stock tickers in different bundles
        if (getRandomInt(0, 1) == 0 && !stockTickers.empty()) {
            stock = stockTickers[getRandomInt(0, stockTickers.size() - 1)];
        }
        stockTickers.push_back(stock);

        int coefficient = getRandomInt(-10, 10);
        order += stock + " " + std::to_string(coefficient) + " ";
    }
    order += "-1 ";

    // Cost (randomize)
    order += "$" + std::to_string(getRandomInt(100, 1000)) + " ";

    // Quantity
    order += "#" + std::to_string(getRandomInt(1, 20));

    return order;
}

void processWindow(vector<string>& window){
    for(int i = 0; i<window.size(); i++){
        vector<string> tokens = tokZ(window[i]);
        for(int j = 0; j < i; j++){
            if(window[j] == "X")
                continue;
            vector<string> tokensj = tokZ(window[j]);
            if(checkMatching(getStockName(tokens),getStockName(tokensj))){ //possible cancellations
                if(tokens[2] == "BUY"){
                    if(tokensj[2] == "SELL"){
                        if(s2I(tokensj[tokensj.size()-3].substr(1)) < s2I(tokens[tokens.size()-3].substr(1))){ //previous buy higher than current ka sell
                            //update the quantities accordingly 
                            if(s2I(tokensj[tokensj.size()-2].substr(1)) < s2I(tokens[tokens.size()-2].substr(1))){ // more quantity of the buy
                                window[j] = "X";
                                //update window[i] ka quantity
                                tokens[tokens.size() - 2] = "#" + to_string(s2I(tokens[tokens.size()-2].substr(1)) - s2I(tokensj[tokensj.size()-2].substr(1)));
                                //now recombine
                                window[i] = "";
                                for(auto tok : tokens){
                                    window[i] = window[i] + tok + " ";
                                }
                                window[i].pop_back();

                                continue;
                            }
                            else if(s2I(tokensj[tokensj.size()-2].substr(1)) > s2I(tokens[tokens.size()-2].substr(1))){ // less quantity of the previous buy
                                window[i] = "X";

                                //update window[j] ka quantity
                                tokensj[tokensj.size() - 2] = "#" + to_string(s2I(tokensj[tokensj.size()-2].substr(1)) - s2I(tokens[tokens.size()-2].substr(1)));
                                //now recombine
                                window[j] = "";
                                for(auto tok : tokensj){
                                    window[j] = window[j] + tok + " ";
                                }
                                window[j].pop_back();
                                break;
                            } 
                            else{ //same quantity dono khach khach
                                window[j] = "X";
                                window[i] = "X";
                                break;
                            }
                        }
                        //if less than or equal to the current sell we are good since no trade executed
                    }
                }
                else{ // if tokens[2] = SELL 
                    if(tokensj[2] == "BUY"){
                        if(s2I(tokensj[tokensj.size()-3].substr(1)) > s2I(tokens[tokens.size()-3].substr(1))){ //previous sell lower than incoming buy -- self arbitrage
                            //update the quantities accordingly 
                            if(s2I(tokensj[tokensj.size()-2].substr(1)) < s2I(tokens[tokens.size()-2].substr(1))){ // more quantity of the sell
                                window[j] = "X";
                                //update window[i] ka quantity
                                tokens[tokens.size() - 2] = "#" + to_string(s2I(tokens[tokens.size()-2].substr(1)) - s2I(tokensj[tokensj.size()-2].substr(1)));
                                //now recombine
                                window[i] = "";
                                for(auto tok : tokens){
                                    window[i] = window[i] + tok + " ";
                                }
                                window[i].pop_back();

                                continue;
                            }
                            else if(s2I(tokensj[tokensj.size()-2].substr(1)) > s2I(tokens[tokens.size()-2].substr(1))){ // less quantity of the sell
                                window[i] = "X";

                                //update window[j] ka quantity
                                tokensj[tokensj.size() - 2] = "#" + to_string(s2I(tokensj[tokensj.size()-2].substr(1)) - s2I(tokens[tokens.size()-2].substr(1)));
                                //now recombine
                                window[j] = "";
                                for(auto tok : tokensj){
                                    window[j] = window[j] + tok + " ";
                                }
                                window[j].pop_back();
                                break;
                            } 
                            else{ //same quantity dono khach khach
                                window[j] = "X";
                                window[i] = "X";
                                break;
                            }
                        }
                        //if less than or equal to the old sell we are good since no trade executed
                    }
                }
            }
            //otherwise we are good for now
        }
    }
    for(int i =0; i<window.size(); i++){
        if(window[i] == "X"){
            window.erase(window.begin()+i);
            i--;
        }
    }
}

//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________

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

//checks order validity
bool checkOrderValidity(string& order){

    vector<string> tokens = tokZ(order);
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

    vector<string> name_tok = tokZ(name);

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

    if(name_tok.size() == 2 && name_tok[1] == "1"){
        name = name_tok[0];
    }

    //now reconstruct order
    order = "";
    order = order + tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + name + " " + tokens[tokens.size()-3] + " " + tokens[tokens.size()-2] + " " + tokens[tokens.size()-1];
    return true;
}

//__________________________________________________Main Code______________________________________________________________

int main(){

    // cout<<isPositiveNumber("-23423");
    // cout<<isPositiveNumber("1351");
    // cout<<isPositiveNumber("-2139581-123");
    // string s = "Ba2NaNaaaw";
    // vector<string> name = tokZ(s);

    // for(auto l : name){
    //     cout<< l;
    // }
    //cout<<checkMatching("BANANA 1 AMZN 1", "AMZN 1 BANANA 1")<<endl;

    string s1 = "0 KarGoCorp SelL AMZN 2 $100 #30 10";
    string s2 = "1 KarGoKrab BuY AMZN 2 ams sdv $130 ##12 0";
    
    
    cout<<checkOrderValidity(s1)<<endl;
    cout<<s1<<endl;
    //cout<<checkOrderValidity(s2)<<endl;
    // makeCapital(s);
    // cout<<s;
    // vector<pair<string,vector<int>>> stockline;
    // vector<string> order_book = 
    // vector<string>{
    //     "0 KarGoCorp SELL AMZN $100 #30 10",
    // //     "0 KarGoExpress SELL AMD $120 #32 1",
    //     "1 KarGoKrab BUY AMZN $130 #12 0",
    // //     "1 KarGoCorp BUY GE $200 #20 8",
    //     "1 KarGoCorp BUY AMZN $50 #30 5",
    // //     "2 KarGoTravels BUY AMD $100 #50 10",
    //     "2 KarGoCorp SELL AMZN $50 #5 0",
    // //     "3 KarGoKrab BUY AMD $130 #10 7",
    // //     "3 KarGoTravels BUY AMD $150 #25 0",
    // //     "4 KarGoExpress SELL GE $150 #50 6",
    // //     "5 KarGoExpress SELL AMD $80 #100 -1",
    // //     "5 KarGoExpress BUY NFLX $80 #15 6",
    // //     "6 KarGoCorp BUY AMD $120 #10 1",
    //     "6 KarGoTravels SELL AMZN $50 #22 -1",
    // //     "7 KarGoKrab BUY GE $110 #10 3",
    // //     "7 KarGoCorp SELL GE $50 #15 -1",
    // //     "7 AKG SELL NFLX $290 #7 -1",
    // //     "8 AKG SELL AMD $23 #3 15",
    // //     "9 SM BUY NFLX $496 #7 5",
    // //     "9 SM BUY GE $107 #2 11",
    // //     "10 AKG BUY NFLX $143 #2 12",
    // //     "10 AKG SELL GE $43 #5 7",
    // //     "11 SV BUY GE $486 #12 7",
    // //     "11 SV SELL GE $230 #9 17",
    // //     "12 GS SELL GE $138 #16 -1",
    // //     "12 SV SELL GE $300 #14 -1",
    // //     "13 SM SELL GE $87 #5 7",
    // //     "13 SV BUY AMD $200 #1 3",
    // //     "14 OT BUY AMD $144 #9 2",
    // //     "14 GS BUY NFLX $74 #9 13",
    // //     "15 AKG BUY NFLX $93 #14 0",
    //     "15 KG SELL AMZN $433 #8 -1",
    //     "16 AKG SELL AMZN $421 #1 19",
    // //     "16 KA BUY AMD $139 #12 16",
    // //     "17 KA BUY NFLX $459 #12 19",
    // //     "17 SV SELL GE $85 #14 0",
    // //     "18 AA SELL NFLX $94 #9 16",
    // //     "18 KG SELL AAPL $456 #0 0",
    // //     "19 AKG SELL GE $211 #12 4",
    // //     "19 AKG BUY GE $260 #15 9" ,
    //     "19 SV SELL AMZN $134 #10 18",
    //     "20 AA BUY AMZN $116 #0 17",
    // //     "21 SV SELL AMD $72 #1 11",
    // //     "21 OT BUY GE $290 #10 4",
    // //     "22 SM SELL AAPL $204 #2 12",
    // //     "22 KG SELL GE $160 #4 5",
    // //     "23 KG BUY NFLX $416 #6 10",
    // //     "23 OT SELL NFLX $466 #1 10",
    //     "24 OT SELL AMZN $425 #16 9",
    // //     "24 SM BUY AMD $15 #14 1",
    // //     "25 OT SELL AMD $429 #9 6",
    // //     "25 SM BUY AMZN $477 #5 19",
    // //     "26 SM BUY AMD $397 #7 12",
    // //     "26 OT SELL GE $29 #6 7",
    // //     "27 AKG SELL GE $30 #3 3",
    // //     "27 SM BUY GE $449 #7 11",
    //     "27 KG SELL AMZN $166 #5 10",
    // //     "28 AA BUY AMD $242 #8 12",
    // //     "29 SM BUY AMD $192 #6 9",
    // //     "29 OT SELL AMD $271 #11 -1",
    // //     "29 SV SELL AAPL $396 #4 -1",
    // //     "30 OT BUY AMD $422 #16 10",
    // //     "31 AA SELL AMD $67 #6 9",
    // //     "31 KA BUY GE $337 #6 16",
    //     "31 AA BUY AMZN $379 #14 14",
    // //     "32 OT BUY AAPL $377 #14 -1",
    //     "33 SV BUY AMZN $350 #7 4",
    //     "33 KA SELL AMZN $7 #7 16"
    // };
    
    //cout<<getStockName(tokZ("0 A BUY AMZN 1 $100 #30 10"))<<endl;
    //cout<<getStockName(tokZ("0 A BUY AMZN 1 NFLZ -1 $100 #30 10"))<<endl;
    //cout<<getStockName(tokZ("0 A BUY AMZN NFLZ -1 LSAD $100 #30 10"))<<endl;

    // CollectInfo(stockline,order_book);
    // CollectInfo(stockline,order_book);
    // processWindow(order_book);
    // for(auto line : order_book){
    //     cout<<line<<endl;
    // }
    // trade(order_book,stockline);
    return 0;
}