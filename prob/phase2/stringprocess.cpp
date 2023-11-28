#include "string"
#include "vector"
#include "iostream"

using namespace std;

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

vector<string> final_tokenize(vector<string> tokens){
    vector<string> final_tokens;
    final_tokens.push_back(tokens[0]);//pushes back the time of entry
    string name;
    int i;
    for( i=1;i<tokens.size();i++)
    {
        while(tokens[i]!="SELL" || tokens[i]!="BUY")
        {
            name+=tokens[i]+" ";
            continue;
        }
        break;
    }
    //remove last space in the name which will be extra and we dont want it
    name.pop_back();
    // so now our name has "XYZ -1 ABC 1"
    //at this pt i is basically the i at which i becomes SELL or BUY so at the old values  
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
        final_tokens.push_back(tokens[i]);
    }
    return final_tokens;
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

//___________________________________________________________New Functions__________________________________________________________________________________________

// AMZN 1 -> AMZN automatically when checkValidity is called. 
// check validity autocapitalizes BUY and SELL in an order

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

//checks if the input string is a valid ticker
bool isAlphabetical(string s){
    for(char c : s){
        if(!(static_cast<int>('a') <= tolower(c) && static_cast<int>('z') >= tolower(c))){
            return false;
        }
    }
    return true;
}

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

// checks order validity
// also converts AMZN 1 type orders into AMZN
bool checkOrderValidity(string& order){

    vector<string> tokens = tokenize(order);
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

    vector<string> name_tok = tokenize(name);

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

//_________________________________________________________________Update to checkMatching___________________________________________________________

// checkMatching("AMZN 1", "AMZN") is true although redundant


// function to check for matching stock combo : imported from phase 2 part 1
bool checkMatching(string old_order, string name){

    bool flag = false;
   
    vector<string> tokens = tokenize(old_order);
    vector<string> name_tokens = tokenize(name);

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
