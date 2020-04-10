#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace PokerProbability{
    // possible list of whitespaces
    const std::string WHITESPACES = " \t\f\v\n\r";


    // Trim a string
    static std::string trim(const std::string &str){
        const std::string WHITESPACES = " \t\f\v\n\r";
        if(str.empty()) return str;

        std::size_t firstScan = str.find_first_not_of(WHITESPACES);
        std::size_t first     = firstScan == std::string::npos ? str.length() : firstScan;
        std::size_t last      = str.find_last_not_of(WHITESPACES);
        return str.substr(first, last-first+1);
    }


    // capitalize a string
    static void capitalize(std::string &str){
        bool new_word = true;
        for(int i=0;i<str.length();i++){
            if(new_word == true && (str.at(i) >= 'a' && str.at(i) <= 'z'))
                str.at(i) = str.at(i) - 'a' + 'A';
            else if(new_word == false && (str.at(i) >= 'A' && str.at(i) <= 'Z'))
                str.at(i) = str.at(i) - 'A' + 'a';
            
            if(str.at(i) == ' ')    new_word = true;
            else                    new_word = false;
        }
    }


    // error listener
    static void errorListener(const std::string &msg, bool soft = false){
        std::cerr<<"!!! error: "<<msg<<std::endl;
        if(soft == false)
            exit(0);
    }


    // process a player_name
    static std::string processPlayerName(const std::string &name){
        std::string mod_name = trim(name);
        capitalize(mod_name);
        return mod_name;
    }


    // split string into integers
    static std::vector<int> getIntsFromStr(std::string inp, char delim = ' '){
        std::vector<int> res;
        std::stringstream ss(inp);
      
        // Parse the input string
        while(ss.good()){
            std::string num_str;
            std::getline(ss, num_str, ' ');
            try {
                res.push_back(std::stoi(trim(num_str)));
            }
            catch (const std::invalid_argument& ia) {
                errorListener("Invalid argument: " + std::string(ia.what()), true);
            }
        }

        return res;
    }


    // printing a generic vector
    template <typename Vector_Type>
    static void printVector(const std::vector<Vector_Type> inp, std::string delim = " ", bool end_with_newline=true){
        std::cout<<"Size: "<<inp.size()<<std::endl;
        std::cout<<"Elements: ";
        for(int idx=0; idx<inp.size(); ++idx){
            if((idx+1)!=inp.size())     std::cout<<inp[idx]<<delim;
            else                        std::cout<<inp[idx];
        }

        if(end_with_newline)
            std::cout<<std::endl;
    }


    // extract player names from comma separated input string
    static std::vector<std::string> getNamesFromInputString(std::string names_string, int num_players){
        std::vector<std::string> player_names;
        std::stringstream ss(names_string);
      
        // Parse the player names string
        int entered_players = 0;
        while(ss.good()) { 
            ++entered_players;
            std::string name;
            std::getline(ss, name, ',');

            if(name == "d"){
                player_names.push_back("Player" + std::to_string(entered_players));
                continue;
            }
            
            player_names.push_back(processPlayerName(name));
        }

        // verifying number of player names entered
        if(entered_players!= num_players)
            errorListener("number of names and number of players do not match");

        return player_names;
    }


    // get cards from space separated cards string
    static std::vector<std::string> getCardsFromString(std::string cards_string, int num_of_cards){
        std::vector<std::string> cards;
        std::stringstream ss(cards_string);
      
        // Parse the player names string
        int entered_cards = 0;
        while(ss.good()) { 
            ++entered_cards;
            std::string card;
            std::getline(ss, card, ' ');
            cards.push_back(card);
        }

        // verifying number of cards entered
        if(entered_cards != num_of_cards)
            errorListener("number of cards entered are not equal to " + std::to_string(num_of_cards));

        return cards;
    }


    // convert vector of two elements into pair
    template <typename Type>
    static std::pair<Type, Type> vectorToPair(std::vector<Type> inp){
        assert(inp.size() == 2);
        return std::make_pair(inp[0], inp[1]);
    }
}

#endif