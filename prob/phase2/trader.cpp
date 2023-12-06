#include <iostream>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>

using namespace std;
extern mutex printMutex;
extern std::atomic<int> commonTimer;


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
string getSN(vector<string> tokens){
    string stock_name="";
    for(int i = 3; i<tokens.size()-3; i++){
        stock_name+= tokens[i];
        stock_name+=" ";
    }
    stock_name.pop_back();
    return stock_name;
}

// prints the order to terminal
void printOrder(vector<string> line_toks, int max_window_time){
    string order="";
    
    order = order + to_string(min(commonTimer.load(), max_window_time)) + " ";

    order = order + "22b0413_22b1818 ";
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
    order += line_toks[line_toks.size()-2];
    order += " ";

    if(line_toks[line_toks.size()-1]!= "-1")
        order += to_string(s2I(line_toks[0]) + s2I(line_toks[line_toks.size()-1]) - commonTimer.load());
    else    
        order += "-1";

    std::lock_guard<std::mutex> lock(printMutex);
    std::cout<<order<<"\n";
}

//__________________________________________________________Order Processing___________________________________________________________________________________________________________________________________

//checks if the input string is a valid ticker
bool isPureString(string s){
    for(char c : s){
        if(!(static_cast<int>('a') <= tolower(c) && static_cast<int>('z') >= tolower(c))){
            return false;
        }
    }
    return true;
}

//checks if given string is a number both positive and negative included
bool isNum(string n){

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
bool isPositiveNum(string n){

    for(int i =0 ; i<n.size(); i++){
        if(!(48 <= n[i] && n[i]<=57)){
            return false;
        }
    }

    return true;
}

//pass by reference makes the passed string into capitals
void makeCap(string& s){
    for(auto &c : s){
        c = toupper(c);
    }
}

// checks order validity
// also converts AMZN 1 type orders into AMZN
bool checkOrderVal(string& order){

    vector<string> tokens = tokZ(order);
    if(tokens.size()<6){
        return false;
    }

    //check first and last token entry time and exit time
    if(!isNum(tokens[0]) || !isNum(tokens[tokens.size()-1]))
        return false;
    
    //quantity check
    if(!(tokens[tokens.size()-2][0] == '#' && isPositiveNum(tokens[tokens.size()-2].substr(1)))){
        return false;
    }

    //price check
    if(!(tokens[tokens.size()-3][0] == '$' && isPositiveNum(tokens[tokens.size()-3].substr(1)))){
        return false;
    }

    makeCap(tokens[2]);
    if(!(tokens[2] == "BUY" || tokens[2] == "SELL")){
        return false;
    }

    string name = getSN(tokens);
    makeCap(name);

    vector<string> name_tok = tokZ(name);

    if(name_tok.size() == 1){
        if(!isPureString(name_tok[0])){
            return false;
        }
    }
    else{
        if(name_tok.size()%2 == 1){
            return false;
        }
        else{
            for(int i = 0; i<name_tok.size(); i++){
                if(i%2 == 0 && !isPureString(name_tok[i])){
                    return false;
                }
                if(i%2 == 1 && !isNum(name_tok[i])){
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

//______________________________________Arbitrage Functions__________________________________________________________________________________________________________________________________________________________________________________________________________________

// function to check for matching stock combo : imported from phase 2 part 1
bool checkMatch(string old_order, string name){

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
    for(auto line : combination){
        vector<string> tokens = tokZ(line);
        if(tokens[tokens.size()-1] == "-1")
            continue;
        if(commonTimer.load() > s2I(tokens[0]) + s2I(tokens[tokens.size()-1])){ //expired order
            return 1;
        }
    }

    return 0;

    // //checks for invalid time
    // for(int i = 0; i < combination.size(); i++){
    //     vector<string> tokens = tokZ(combination[i]);
    //     if(tokens[tokens.size()-1] == "-1")
    //         continue;
    //     if(commonTimer.load() > s2I(tokens[0]) + s2I(tokens[tokens.size()-1])){
    //         return 1;
    //     }
    // }

    // for(int i=0; i<combination.size(); i++){
    //     vector<string> tok1 = tokZ(combination[i]);
    //     string s1;
    //     //compile the bundle 1
    //     int it = 3;
    //     while(tok1[it][0]!='$'){
    //         s1+=tok1[it];
    //         s1+=" ";
    //         it++;
    //     }
    //     s1.pop_back();
    //     for(int j = i+1; j<combination.size(); j++){
    //         vector<string> tok2 = tokZ(combination[j]);

    //         if(tok1[2] != tok2[2] && tok1[1] != tok2[1]){ // we have a possible cancellation if BUY SELL combination not by the same person
    //             //the stock bundle is the same then we have a match anyways                
    //             string s2;
    //             it = 3;
    //             while(tok2[it][0]!='$'){
    //                 s2+=tok2[it];
    //                 s2+=" ";
    //                 it++;
    //             }
    //             s2.pop_back();
    //             if(checkMatch(s1,s2))
    //                 return 1;
    //         }
    //     }
    // }
    // return 0;
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
            if(isPureString(tokens[i]) && findTicker(tickers,tokens[i])==-1){ //if string and it is not in tickers already we add it into tickers
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
                if(isPureString(tokens[i])){ // update the quantities based on the bundle quantity and the quantity of the stock in the bundle itself
                    int it = findTicker(tickers, tokens[i]);
                    if(isPureString(tokens[i+1]) || tokens[i+1][0] == '$'){ //quantity of 1
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

// processes the window to get only the most efficient trades and avoid self arbitrage
void processWindow(vector<string>& window){
    for(int i = 0; i<window.size(); i++){
        if(window[i] == "X"){
            continue;
        }

        vector<string> tokens = tokZ(window[i]);
        

        for(int j = 0; j < i; j++){
            if(window[j] == "X")
                continue;
            vector<string> tokensj = tokZ(window[j]);

            if(checkMatch(getSN(tokens),getSN(tokensj))){ //possible cancellations
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

// runs our arbitrage detection and trading algorithm
bool tradeArbitrage(vector<string> window, vector<pair<string,vector<int>>> stockline, int max_window_time){
    
    vector<pair<int,int>> best_arbitrage = findBestArbitrage(window, window.size()); //given the number of lines

    //to check if valid arbitrage not present in which case output no trade and continue
    bool flag = true;
    for(int j=0; j<best_arbitrage.size(); j++){
        if(best_arbitrage[j].first==1){
            flag = false;
            break;
        }
    }

    if(flag)
        return false;
    
    if(!flag){
        // if there exists an arbitrage
        // we display the arbitrage in reverse order
        // as we display we update the quantities in the order book and delete the ones where quantities become zero
        int min_time_to_expiry = 2147483647;
        for(int iterator = 0; iterator<window.size(); iterator++){
            if(best_arbitrage[iterator].first == 1 && s2I(tokZ(window[iterator])[tokZ(window[iterator]).size()-1])!= -1)
                min_time_to_expiry = min(min_time_to_expiry, s2I(tokZ(window[iterator])[tokZ(window[iterator]).size()-1])+ s2I(tokZ(window[iterator])[0]));
        }
        if(min_time_to_expiry == 2147483647)
            min_time_to_expiry = -1;
        for(int iterator = window.size()-1; iterator>=0; iterator--){
            if(best_arbitrage[iterator].first == 1){ // this is a part of our arbitrage so we display it

                vector<string> line_toks = tokZ(window[iterator]);
                string order="";
                order = order + to_string(min(commonTimer.load(), max_window_time)) + " ";
                order = order + "22b0413_22b1818 ";
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

                if(min_time_to_expiry != -1)
                    order += to_string(min_time_to_expiry - commonTimer.load());
                else    
                    order += "-1";

                std::lock_guard<std::mutex> lock(printMutex);
                std::cout<<order<<"\n";

            }
        }
        return true;
    }
}

//runs our median trading algorithm
void tradeMedian(vector<string> window, vector<pair<string,vector<int>>> stockline, int max_window_time){

    vector<vector<string>> orders; 

    // we collect the relevant lines for each stock 
    //int min_time_to_expiry = 2147483647;
    for(auto line : window){
        
        if(detectError(vector<string>{line}))
            continue; //no point of executing any trades there
        
        //now if the line is tradable, we try and trade with it, in a way as to maximize our profits

        
        vector<string> tokens = tokZ(line);
        string name = getSN(tokens);
        string type = tokens[2];
        int price = s2I(tokens[tokens.size()-3].substr(1,tokens[tokens.size()-3].size()-1));
        int quantity = s2I(tokens[tokens.size()-2].substr(1,tokens[tokens.size()-2].size()-1));
        

        // fetch median
        double median_price = 0;
        for(auto stock:stockline){
            if(checkMatch(stock.first,name)){
                //hack
                if(stock.second.size() == 0)    
                    break;
                median_price = (stock.second.size()%2==0)? ((stock.second[stock.second.size()/2]+stock.second[stock.second.size()/2-1])/2.0):stock.second[stock.second.size()/2];
                break;
            }
        }

        if(type == "BUY"){
            // we want to sell above the median price
            if(median_price != 0 && price >= median_price){
                orders.push_back(tokens);
                // if(min_time_to_expiry > s2I(tokens[tokens.size()-1]))
                //     min_time_to_expiry = s2I(tokens[tokens.size()-1]);

            }
        }
        else{
            if(median_price != 0 && price <= median_price){
                orders.push_back(tokens);
                // if(min_time_to_expiry > s2I(tokens[tokens.size()-1]) && s2I(tokens[tokens.size()-1])!= -1)
                //     min_time_to_expiry = s2I(tokens[tokens.size()-1]);
            }
        }
        
    }
    // if(min_time_to_expiry == 2147483647)
    //     min_time_to_expiry = -1;

    for(auto order : orders){
        printOrder(order, max_window_time);
    }

    return; //once we go through the lines we return since no arbitrage anyway 
}

//_______________________________________________Moving Median Functions______________________________________________________________________________________________________________________________________________________________________________________________________

void CollectInfo(vector<pair<string,vector<int>>>& stockline, vector<string> window){
    for(auto order : window){
        vector<string> tokens = tokZ(order);
        string name = getSN(tokens);
        int price = s2I(tokens[tokens.size()-3].substr(1,tokens[tokens.size()-3].size()-1));
        int quantity = s2I(tokens[tokens.size()-2].substr(1,tokens[tokens.size()-2].size()-1));

        bool flag = 0;
        for(auto& line : stockline){
            if(checkMatch(line.first,name)){
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

//___________________________________________________Main Code________________________________________________________________________________________________________________________________________________________________________________________________________________

//our strategy is to break the orderbook into windows of size 4 and run our arbitrage strategy on it 
int reader(int time)
{
    vector<string> order_book;
    vector<pair<string,vector<int>>> stockline;
    vector<pair<string,int>> window;

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
            int index = find(order_book,line);
            if(index == -1 && 48 <= line[0] && line[0]<=57 && tokZ(line)[1] != "22b0413_22b1818"){
                removeHC(line);
                if(checkOrderVal(line))
                    order_book.push_back(line);  
            }
        }
        
        if(!flag)
            break;

        // now that we have the order book let us execute our trading quantity 
        
        //first compile the new lines into our window
        for(int i = 0; i<order_book.size(); i++){
            if(i < window.size()){
                window[i].second = 0;
            }
            if(i >= window.size()){
                window.push_back(make_pair(order_book[i], 1));
            }
        }

        vector<string> neworders;
        int max_window_time = s2I(tokZ(order_book[order_book.size()-1])[0]);
        
        for(auto i : window){
            if(i.second == 1 && tokZ(i.first)[tokZ(i.first).size()-2].substr(1) != "0"){ //filters out zero quantity
                neworders.push_back(i.first);
            }
        }
        //now we process the window to remove overhead and avoid self arbitrage

        processWindow(neworders);

        if(neworders.size() > 5){
            tradeMedian(neworders, stockline, max_window_time);
        }
        else{ //small window size we can check arbitrage
            if(!tradeArbitrage(neworders,stockline, max_window_time)){
                tradeMedian(neworders, stockline, max_window_time);
            }
        }

        CollectInfo(stockline, neworders);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        order_book.clear();
    }

    return 1;
}

int trader(std::string *message)
{
    return 1;
}