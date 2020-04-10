#ifndef POKER_PROB_CALCULATOR_H
#define POKER_PROB_CALCULATOR_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace PokerProbability{

    #define SPADE "\u2664"
    #define CLUB "\u2667"
    #define HEART "\u2665"
    #define DIAMOND "\u2666"

    class PokerProbCalculator{
    private:
        bool error_status;
        std::string error_msg;
        std::vector<std::string> players_names;
        std::vector<bool> folded_players;
        std::vector<std::string> dealt_cards;
        std::vector<std::pair<std::string, std::string> > players_hand_cards;
        std::unordered_map<std::string, bool> map_of_seen_cards;
        static const std::vector<char> suits;
        static const std::vector<char> card_numbers;
        static const std::vector<std::string> cards_deck;

        static std::vector<std::string> cartesianProduct(std::vector<char> a, std::vector<char> b);
        static std::string formattedCard(std::string card);
        bool validateCard(std::string card);
        bool validateCards(std::vector<std::string> cards);

    public:
        PokerProbCalculator(std::vector<std::string> players_names, std::vector<std::pair<std::string, std::string> > players_cards);
        void addDealtCards(std::vector<std::string> cards);
        void printGameStatus() const;
        void displayRemainingPlayers();
        void addFoldedPlayers(std::vector<int> indices);

        std::string errorMessage() const {return error_msg;}
        bool errorStatus() const {return error_status;}
    };
}

#endif