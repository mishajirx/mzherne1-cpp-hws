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
using std::cerr;

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
    
    // Check if file can be opened
    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cerr << "ERROR: Unable to open file '" << argv[1] << "'" << endl;
        return 1;
    }
    
    // Read the input file
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
    
    // Check if file was empty
    if (text.empty()) {
        cerr << "ERROR: Input file is empty" << endl;
        return 2;
    }
    
    // Parse and validate k
    int maxK;
    try {
        maxK = std::stoi(argv[2]);
    } catch (const std::invalid_argument& e) {
        cerr << "ERROR: k value '" << argv[2] << "' is not numeric" << endl;
        return 3;
    } catch (const std::out_of_range& e) {
        cerr << "ERROR: k value '" << argv[2] << "' is out of range" << endl;
        return 3;
    }
    
    if (maxK < 1) {
        cerr << "ERROR: k value must be at least 1" << endl;
        return 4;
    }
    
    // Check if k is larger than the text length
    if (maxK > text.length()) {
        cerr << "ERROR: k value (" << maxK << ") is larger than text length (" << text.length() << ")" << endl;
        return 5;
    }
    
    // Parse and validate n (or check for -m flag)
    int n = 0;
    bool printJSON = false;
    std::string thirdArg = argv[3];
    
    if (thirdArg == "-m") {
        printJSON = true;
    } else {
        try {
            n = std::stoi(argv[3]);
        } catch (const std::invalid_argument& e) {
            cerr << "ERROR: n value '" << argv[3] << "' is not numeric and is not '-m'" << endl;
            return 6;
        } catch (const std::out_of_range& e) {
            cerr << "ERROR: n value '" << argv[3] << "' is out of range" << endl;
            return 6;
        }
        
        if (n < maxK) {
            cerr << "ERROR: n value (" << n << ") must be at least k (" << maxK << ")" << endl;
            return 7;
        }
    }
    
    // Handle -m flag for JSON output
    if (printJSON) {
        // Build distribution for level maxK only
        std::map<std::string, std::map<char, int>> distribution;
        
        // Special case handling: prepend the last k characters to the beginning
        std::string processedText = text.substr(text.length() - maxK) + text;
        
        // Build the distribution
        for (size_t i = 0; i <= processedText.length() - maxK - 1; i++) {
            std::string kgram = processedText.substr(i, maxK);
            char nextChar = processedText[i + maxK];
            distribution[kgram][nextChar]++;
        }
        
        // Print JSON output
        cout << "{" << endl;
        bool firstKgram = true;
        for (std::map<std::string, std::map<char, int>>::iterator kgramIt = distribution.begin(); 
             kgramIt != distribution.end(); ++kgramIt) {
            if (!firstKgram) {
                cout << "," << endl;
            }
            firstKgram = false;
            
            cout << "    \"" << kgramIt->first << "\": {" << endl;
            
            bool firstChar = true;
            for (std::map<char, int>::iterator charIt = kgramIt->second.begin(); 
                 charIt != kgramIt->second.end(); ++charIt) {
                if (!firstChar) {
                    cout << "," << endl;
                }
                firstChar = false;
                
                cout << "        \"" << charIt->first << "\": " << charIt->second;
            }
            cout << endl << "    }";
        }
        cout << endl << "}" << endl;
        
        return 0;
    }
    
    // Generate text for each level from 1 to k
    for (int k = 1; k <= maxK; k++) {
        // Build the character distribution for level k
        // Map from k-length string to map of next characters and their counts
        std::map<std::string, std::map<char, int>> distribution;
        
        // Special case handling: prepend the last k characters to the beginning
        std::string processedText;
        if (k <= text.length()) {
            processedText = text.substr(text.length() - k) + text;
        } else {
            processedText = text;  // If k > text length, just use the text as is
        }
        
        // Build the distribution
        for (size_t i = 0; i <= processedText.length() - k - 1; i++) {
            std::string kgram = processedText.substr(i, k);
            char nextChar = processedText[i + k];
            distribution[kgram][nextChar]++;
        }
        
        // Generate random text
        cout << "Level " << k << ": ";
        
        // Start with the first k characters from the original text
        std::string currentKgram;
        if (k <= text.length()) {
            currentKgram = text.substr(0, k);
        } else {
            currentKgram = text;  // If k > text length, use entire text
        }
        cout << currentKgram;
        
        // Generate n - k more characters
        for (int i = k; i < n; i++) {
            // Get the distribution for the current k-gram
            if (distribution.find(currentKgram) != distribution.end()) {
                std::map<char, int>& charCounts = distribution[currentKgram];
                
                // Calculate total occurrences
                int total = 0;
                for (std::map<char, int>::iterator it = charCounts.begin(); 
                     it != charCounts.end(); ++it) {
                    total += it->second;
                }
                
                // Generate random number and select character based on probabilities
                int randNum = std::rand() % total;
                int cumulative = 0;
                char nextChar = ' ';
                
                for (std::map<char, int>::iterator it = charCounts.begin(); 
                     it != charCounts.end(); ++it) {
                    cumulative += it->second;
                    if (randNum < cumulative) {
                        nextChar = it->first;
                        break;
                    }
                }
                
                // Output the character and update current k-gram
                cout << nextChar;
                if (k > 1) {
                    currentKgram = currentKgram.substr(1) + nextChar;
                } else {
                    currentKgram = std::string(1, nextChar);
                }
            } else {
                // This shouldn't happen with the special case handling
                break;
            }
        }
        
        cout << endl;
        cout << "~~~" << endl;
    }

    return 0;

}