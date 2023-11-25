#include "iostream"
#include "vector"
#include "string"
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
int main(){
    cout<<checkEquality("x 1 y -1","y -1 x 1");
    return 0;
}