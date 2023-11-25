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