#include "Helper.h"
#include "PokerProbCalculator.h"
#include <iostream>
#include <vector>
#include <sstream>

// for testing and debugging purposes
// #define DEBUG
#define LINE_UP "\x1b[A"
#define CONSOLE_PRE ">>> "

enum Command{
    HELP,
    STATUS,
    NEXT,
    FOLD,
    QUIT,
    INVALID
};
// #define HELP 0
// #define STATUS 1
// #define NEXT 2
// #define FOLD 3
// #define QUIT 4

using namespace PokerProbability;


void printHelpMenu(){
    std::cout<<"help menu: \n";
    std::cout<<"> help   --- print this help menu\n";
    std::cout<<"> status --- print the current status of the game\n";
    std::cout<<"> next   --- proceed to next part of the game\n";
    std::cout<<"> fold   --- add some players who are folding at this point of the game\n";
    std::cout<<"> quit   --- quit and exit\n";
}


Command getCommandFromString(std::string cmd_str){
    if(cmd_str.length() >= 8)           return INVALID;
    if(std::string("help").rfind(cmd_str, 0) == 0)   return HELP;
    if(std::string("status").rfind(cmd_str, 0) == 0) return STATUS;
    if(std::string("next").rfind(cmd_str, 0) == 0)   return NEXT;
    if(std::string("fold").rfind(cmd_str, 0) == 0)   return FOLD;
    if(std::string("quit").rfind(cmd_str, 0) == 0)   return QUIT;

    return INVALID;
}


// Entry point of program
int main(int argc, char const *argv[]){
    // Print greeting message
    std::cout<<"Let's calculate the probability of winning in poker "<<SPADE<<CLUB<<HEART<<DIAMOND<<" :)\n\n";


    // Take input the number of players
    int num_players;
    std::cout<<CONSOLE_PRE<<"Enter number of players: ";
    std::cin>>num_players;
    if(num_players > 23)
        errorListener("number of players are restricted to 23");
    

    // Take input the name of players
    std::string names_string;
    std::cout<<CONSOLE_PRE<<"Enter commas separated names for players (type 'd' for using default name of that player): ";
    std::cin.ignore(10, '\n');
    std::getline(std::cin, names_string, '\n');
    std::vector<std::string> player_names = getNamesFromInputString(names_string, num_players);

    #ifdef DEBUG
        std::cout<<"Players names details: \n";
        printVector<std::string>(player_names);
    #endif

    
    // enter cards of players
    std::vector<std::pair<std::string, std::string> > cards(num_players);
    std::string cards_string;
    for(int i=0; i<num_players; i++){
        std::cout<<CONSOLE_PRE<<"Enter 2 cards in "<<player_names[i]<<"'s hand ex. 2H JS (2 of Hearts and Jack of Spades): ";
        getline(std::cin, cards_string, '\n');
        cards[i] = vectorToPair(getCardsFromString(cards_string, 2)); 

        // clearing up for other player, so they can enter their cards in sequence
        std::cout<<LINE_UP;
        for(int i=0; i<45 + cards_string.length() + player_names[i].length(); i++)
            std::cout<<"  ";
        std::cout<<"\r";
    }

    // Pass the data to probability calculator
    PokerProbCalculator calc_obj(player_names, cards);
    if(calc_obj.errorStatus() == true)
        errorListener("invalid cards entered: " + calc_obj.errorMessage());
    std::cout<<"All players cards entered successfully.\nNow you can progress through by entering appropiate commands.";
    std::cout<<" Enter 'help' command if you are stuck at any point.\n";

    Command cmd_choice;
    std::string cmd;
    int next_count = 0;
    int round_cards[] = {3, 1, 1};
    do{
        // menu for continuing
        std::cout<<CONSOLE_PRE;
        getline(std::cin, cmd);
        cmd_choice = getCommandFromString(cmd);
        
        switch(cmd_choice){
            case HELP  : {printHelpMenu(); break;};
            case STATUS: {calc_obj.printGameStatus(); break;};

            case NEXT: {
                if(next_count >= 3)
                    std::cout<<"5 cards have been dealt, all rounds are finished\n";
                else{
                    ++next_count;
                    std::string is_plural = next_count > 1 ? "s":"";
                    std::cout<<("Enter the " + std::to_string(round_cards[next_count-1]) + " card" + is_plural + " dealt in this round: ");
                    getline(std::cin, cards_string, '\n');
                    calc_obj.addDealtCards(getCardsFromString(cards_string, round_cards[next_count-1]));
                    if(calc_obj.errorStatus() == true)
                        errorListener(calc_obj.errorMessage(), true);
                }
                break;
            };

            case FOLD: {
                calc_obj.displayRemainingPlayers();
                std::cout<<"Enter space separated indices of players folded: ";
                std::string temp_str;
                std::vector<int>fold_indices;
                getline(std::cin, temp_str, '\n');
                fold_indices = getIntsFromStr(temp_str);
                calc_obj.addFoldedPlayers(fold_indices);
                if(calc_obj.errorStatus() == true)
                    errorListener(calc_obj.errorMessage(), true);

                break;
            };

            case INVALID: std::cout<<"!!! invalid choice entered.\n\n";
            case QUIT: break;
        }
        
    }
    while(cmd_choice != QUIT);

    return 0;
}