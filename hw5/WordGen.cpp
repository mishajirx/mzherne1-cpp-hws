#include <string>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

// You are not required to use any of the next three functions, but might find
// them useful.  Modify them if it seems appropriate to do so, or delete them.
// They may prove useful for trimming the contents of the file that you read.

/* Removes whitespace from the left end of a string */
void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

/* Removes whitespace from the right end of a string */
void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

/* Removes whitespace from both ends of a string */
void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

using std::endl;
using std::cout;

int main(int argc, char * argv[]){

    // seed the RNG
    std::srand( ( unsigned int ) std::time( nullptr ) );

    // read command-line arguments
    // arg1: filename
    // arg2: max value of K
    // arg3: num characters to generate (or -m)
    if (4 != argc){
        cout << "ERROR: Wrong number of command-line arguments (expected 3, received " << (argc-1) << ")" << endl;
        return -1;
    }


    // other error checking, rest of your main code to go here :)
    
    // Read the input file
    std::ifstream infile(argv[1]);
    std::string text;
    std::string line;
    while (std::getline(infile, line)) {
        text += line + "\n";
    }
    // Remove the last newline if it was added
    if (!text.empty() && text.back() == '\n') {
        text.pop_back();
    }
    infile.close();
    
    // For now, hardcode k = 1
    int k = 1;
    int n = std::stoi(argv[3]);
    
    // Build the character distribution for level 1
    // Map from k-length string to map of next characters and their counts
    std::map<std::string, std::map<char, int>> distribution;
    
    // Special case handling: prepend the last k characters to the beginning
    std::string processedText = text.substr(text.length() - k) + text;
    
    // Build the distribution
    for (size_t i = 0; i < processedText.length() - k; i++) {
        std::string kgram = processedText.substr(i, k);
        char nextChar = processedText[i + k];
        distribution[kgram][nextChar]++;
    }
    
    // Generate random text
    cout << "Level 1: ";
    
    // Start with the first k characters from the original text
    std::string currentKgram = text.substr(0, k);
    cout << currentKgram;
    
    // Generate n - k more characters
    for (int i = k; i < n; i++) {
        // Get the distribution for the current k-gram
        if (distribution.find(currentKgram) != distribution.end()) {
            auto& charCounts = distribution[currentKgram];
            
            // Calculate total occurrences
            int total = 0;
            for (const auto& pair : charCounts) {
                total += pair.second;
            }
            
            // Generate random number and select character based on probabilities
            int randNum = std::rand() % total;
            int cumulative = 0;
            char nextChar = ' ';
            
            for (const auto& pair : charCounts) {
                cumulative += pair.second;
                if (randNum < cumulative) {
                    nextChar = pair.first;
                    break;
                }
            }
            
            // Output the character and update current k-gram
            cout << nextChar;
            currentKgram = currentKgram.substr(1) + nextChar;
        } else {
            // This shouldn't happen with the special case handling
            break;
        }
    }
    
    cout << endl;
    cout << "~~~" << endl;

    return 0;

}