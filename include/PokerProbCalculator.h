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
        std::vector<float> players_win_probability;

        static void makeCombinationsUtil(std::vector<std::vector<int> >& ans, std::vector<int> &a, std::vector<int>& tmp, int left, int k);
        static std::vector<std::vector<int> > makeCombinations(std::vector<int> &a, int k);
        static std::vector<std::string> cartesianProduct(std::vector<char> a, std::vector<char> b);
        static std::string formattedCard(std::string card);
        bool validateCard(std::string card);
        bool validateCards(std::vector<std::string> cards);
        std::vector<int> getRemainingPlayersIndices();
        void calculateWinCount(const std::vector<std::vector<int> > &indices_combination, std::vector<int> &win_count, const std::vector<int> &remaining_player_indices, const std::vector<int> &deck_indices, int start_index, int end_index);

    public:
        PokerProbCalculator(std::vector<std::string> players_names, std::vector<std::pair<std::string, std::string> > players_cards);
        void addDealtCards(std::vector<std::string> cards);
        void printGameStatus() const;
        void displayRemainingPlayers();
        void addFoldedPlayers(std::vector<int> indices);
        void calculateProb();

        std::string errorMessage() const {return error_msg;}
        bool errorStatus() const {return error_status;}
    };
}

#endif