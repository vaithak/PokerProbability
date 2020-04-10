#include "PokerProbCalculator.h"
#include <algorithm>
#include <iomanip>

using namespace PokerProbability;

static unsigned int max(unsigned int a, unsigned int b){
    return a>b?a:b;
}


// definition of suits and card_numbers
const std::vector<char> PokerProbCalculator::suits = {'H', 'D', 'C', 'S'};
const std::vector<char> PokerProbCalculator::card_numbers = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'J', 'Q', 'K'};
const std::vector<std::string> PokerProbCalculator::cards_deck = PokerProbCalculator::cartesianProduct(PokerProbCalculator::card_numbers, PokerProbCalculator::suits);



std::vector<std::string> PokerProbCalculator::cartesianProduct(std::vector<char> a, std::vector<char> b){
    std::vector<std::string> res;
    for(size_t i=0; i<a.size(); i++)
        for(size_t j=0; j<b.size(); j++)
            res.push_back(std::string(1, a[i]) + std::string(1, b[i]));

    return res;
}



std::string PokerProbCalculator::formattedCard(std::string card){
    if(card[1]=='H')         return std::string(1, card[0]) + HEART;
    else if(card[1]=='S')    return std::string(1, card[0]) + SPADE;
    else if(card[1]=='C')    return std::string(1, card[0]) + CLUB;
    else if(card[1]=='D')    return std::string(1, card[0]) + DIAMOND;

    return card;
}



bool PokerProbCalculator::validateCard(std::string card){
    if(card.length() != 2)                                                                          return false;
    if(std::find(suits.begin(), suits.end(), card[1]) == suits.end())                               return false;
    if(std::find(card_numbers.begin(), card_numbers.end(), card[0]) == card_numbers.end())          return false;
    if((map_of_seen_cards.find(card) != map_of_seen_cards.end()) || (map_of_seen_cards[card] == true))  return false;

    return true;
}



bool PokerProbCalculator::validateCards(std::vector<std::string> cards){
    for (std::vector<std::string>::iterator i = cards.begin(); i != cards.end(); ++i)
        if(!validateCard(*i)) return false;

    return true;
}



PokerProbCalculator::PokerProbCalculator(std::vector<std::string> players_names, std::vector<std::pair<std::string, std::string> > players_cards) : folded_players(players_names.size(), false){
    assert(players_names.size() == players_cards.size());
    for (size_t i = 0; i<players_cards.size(); ++i){
        if( !validateCard(players_cards[i].first) || !validateCard(players_cards[i].second) || (players_cards[i].first == players_cards[i].second)){
            this->error_status = true;
            this->error_msg = "invalid cards for player " + players_names[i];
            return;
        }
    }
    this->error_status = false;
    this->error_msg = "";

    // add cards into map_of_seen_cards
    for (size_t i = 0; i<players_cards.size(); ++i){
        map_of_seen_cards[players_cards[i].first] = true;
        map_of_seen_cards[players_cards[i].second] = true;
    }

    // save passed data
    this->players_hand_cards = players_cards;
    this->players_names = players_names;
}



void PokerProbCalculator::addDealtCards(std::vector<std::string> cards){
    if(!validateCards(cards)){
        this->error_status = true;
        this->error_msg = "invalid dealt cards entered";
        return;
    }

    if(this->dealt_cards.size() + cards.size() > 5){
        this->error_status = true;
        this->error_msg = "number of dealt cards cannot exceed 5";
        return;
    }

    for (std::vector<std::string>::iterator i = cards.begin(); i != cards.end(); ++i)
        map_of_seen_cards[*i] = true;

    dealt_cards.insert(std::end(dealt_cards), std::begin(cards), std::end(cards));
}



void PokerProbCalculator::displayRemainingPlayers(){
    std::cout<<"Remaining players with their indices: \n";
    for(size_t i=0; i<players_names.size(); i++)
        if(folded_players[i] == false)
            std::cout<<std::to_string(i+1)<<". "<<players_names[i]<<"\n";
}



void PokerProbCalculator::addFoldedPlayers(std::vector<int> indices){
    // error checking
    for(size_t i=0; i<indices.size(); i++){
        if(indices[i] > (int)players_names.size()){
            this->error_status = true;
            this->error_msg = "invalid player index: " + std::to_string(indices[i]);
            return;
        }

        if(folded_players[indices[i]-1] == true){
            this->error_status = true;
            this->error_msg = "player with index " + std::to_string(indices[i]) + "-" +  players_names[indices[i]-1] + " has already folded before";
            return;
        }
    }

    // folding passed players
    for(size_t i=0; i<indices.size(); i++)
        folded_players[indices[i]-1] = true;
}



void PokerProbCalculator::printGameStatus() const{
    std::cout<<"---------------------------------------------------------------\n";
    std::cout<<"Game Status: \n\n";
    for(size_t i=0; i<players_names.size(); i++)
        if(folded_players[i] == true)
            std::cout<<std::setw(max(2*players_names[i].length() + 10, 20))<<std::left<<players_names[i] + " (folded)";
        else
            std::cout<<std::setw(max(2*players_names[i].length(), 10))<<std::left<<players_names[i];

    std::cout<<"\n";

    for(size_t i=0; i<players_hand_cards.size(); i++)
        if(folded_players[i] == true)
            std::cout<<std::setw(max(2*players_names[i].length() + 4 + 10, 24))<<std::left<<PokerProbCalculator::formattedCard(players_hand_cards[i].first) + ", " + formattedCard(players_hand_cards[i].second);
        else
            std::cout<<std::setw(max(2*players_names[i].length() + 4, 14))<<std::left<<PokerProbCalculator::formattedCard(players_hand_cards[i].first) + ", " + formattedCard(players_hand_cards[i].second);

    std::cout<<"\n\nCards dealt till now: ";
    for(size_t i=0; i<dealt_cards.size(); i++)
        std::cout<<PokerProbCalculator::formattedCard(dealt_cards[i])<<" ";

    std::cout<<"\n---------------------------------------------------------------\n";
}