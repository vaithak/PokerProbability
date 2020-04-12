#include "PokerProbCalculator.h"
#include "Poker7CardsHand.h"
#include <algorithm>
#include <iomanip>
#include <thread>

// for testing and debugging purposes
// #define DEBUG

using namespace PokerProbability;

static unsigned int max(unsigned int a, unsigned int b){
    return a>b?a:b;
}


// definition of suits and card_numbers
const std::vector<char> PokerProbCalculator::suits = {'H', 'D', 'C', 'S'};
const std::vector<char> PokerProbCalculator::card_numbers = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
const std::vector<std::string> PokerProbCalculator::cards_deck = PokerProbCalculator::cartesianProduct(PokerProbCalculator::card_numbers, PokerProbCalculator::suits);



std::vector<std::string> PokerProbCalculator::cartesianProduct(std::vector<char> a, std::vector<char> b){
    std::vector<std::string> res;
    for(size_t i=0; i<a.size(); i++)
        for(size_t j=0; j<b.size(); j++)
            res.push_back(std::string(1, a[i]) + std::string(1, b[j]));

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



PokerProbCalculator::PokerProbCalculator(std::vector<std::string> players_names, std::vector<std::pair<std::string, std::string> > players_cards) : folded_players(players_names.size(), false), players_win_probability(players_names.size(), 0.00){
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
    this->calculateProb();
}



void PokerProbCalculator::addDealtCards(std::vector<std::string> cards){
    if(!validateCards(cards)){
        this->error_status = true;
        this->error_msg = "invalid dealt cards entered";
        return;
    }

    assert((this->dealt_cards.size() == 0) || (this->dealt_cards.size() == 3) || (this->dealt_cards.size() == 4) || (this->dealt_cards.size() == 5));

    if(this->dealt_cards.size()==0 && cards.size()!=3){
        this->error_status = true;
        this->error_msg = "number of dealt cards in flop (first) round should be = 3";
        return;
    }

    if(this->dealt_cards.size()==3 && cards.size()!=1){
        this->error_status = true;
        this->error_msg = "number of dealt cards in turn (second) round should be = 1";
        return;
    }

    if(this->dealt_cards.size()==4 && cards.size()!=1){
        this->error_status = true;
        this->error_msg = "number of dealt cards in river (last) round should be = 1";
        return;
    }

    if(this->dealt_cards.size() == 5){
        this->error_status = true;
        this->error_msg = "number of dealt cards cannot exceed 5";
        return;
    }

    for (std::vector<std::string>::iterator i = cards.begin(); i != cards.end(); ++i)
        map_of_seen_cards[*i] = true;

    dealt_cards.insert(std::end(dealt_cards), std::begin(cards), std::end(cards));
    this->calculateProb();
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

    this->calculateProb();
}



void PokerProbCalculator::printGameStatus() const{
    std::cout<<"---------------------------------------------------------------\n";
    std::cout<<"Game Status: \n\n";
    std::cout<<"Player's name, Player's cards in hand, Player's win probability (actually: win + tie)\n";
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

    std::cout<<"\n";

    // print probabilities
    std::cout << std::fixed;
    std::cout << std::setprecision(3);
    for(size_t i=0; i<players_names.size(); i++)
        if(folded_players[i] == true)
            std::cout<<std::setw(max(2*players_names[i].length() + 10, 20))<<std::left<<players_win_probability[i];
        else
            std::cout<<std::setw(max(2*players_names[i].length(), 10))<<std::left<<players_win_probability[i];

    std::cout<<"\n\nCards dealt till now: ";
    for(size_t i=0; i<dealt_cards.size(); i++)
        std::cout<<PokerProbCalculator::formattedCard(dealt_cards[i])<<" ";

    std::cout<<"\n---------------------------------------------------------------\n";
}



void PokerProbCalculator::makeCombinationsUtil(std::vector<std::vector<int> >& ans, std::vector<int> &a, std::vector<int>& tmp, int left, int k){ 
    // Pushing this vector to a vector of vector 
    if (k == 0) { 
        ans.push_back(tmp);
        return;
    }

    // i iterates from left to n. First time left will be 0
    for (size_t i = left; i < a.size(); ++i){
        tmp.push_back(a[i]); 
        makeCombinationsUtil(ans, a, tmp, i + 1, k - 1); 
        tmp.pop_back(); 
    }
} 


std::vector<std::vector<int> > PokerProbCalculator::makeCombinations(std::vector<int> &a, int k){
    std::vector<std::vector<int> > ans;
    std::vector<int> tmp;
    makeCombinationsUtil(ans, a, tmp, 0, k);
    return ans;
}



std::vector<int> PokerProbCalculator::getRemainingPlayersIndices(){
    std::vector<int> res;
    for(size_t i=0; i<players_names.size(); i++)
        if(folded_players[i] == false)
            res.push_back(i);

    return res;
}



void PokerProbCalculator::calculateWinCount(const std::vector<std::vector<int> > &indices_combination, std::vector<int> &win_count, const std::vector<int> &remaining_player_indices, const std::vector<int> &deck_indices, int start_index, int end_index){
    int dealt_cards_size = this->dealt_cards.size();
    for(int i=start_index; i<end_index; i++){
        // play current match
        int players = remaining_player_indices.size();
        std::vector<short int> player_ranks(players);

        std::string player_cards[7];
        short int min_rank = 7463;
        for(int j=0; j<players; ++j){
            // get rank for current player
            player_cards[0] = this->players_hand_cards[remaining_player_indices[j]].first;
            player_cards[1] = this->players_hand_cards[remaining_player_indices[j]].second;
            for(int k=0; k<dealt_cards_size; ++k)                  player_cards[2+k] = dealt_cards[k];
            for(size_t k=0; k<indices_combination[i].size(); ++k)  player_cards[2 + dealt_cards_size + k] = PokerProbCalculator::cards_deck[indices_combination[i][k]];
            Poker7CardsHand eval(player_cards);
            player_ranks[j] = eval.getRank();

            if(player_ranks[j] < min_rank)
                min_rank = player_ranks[j];

            #ifdef DEBUG
                std::cout<<player_cards[0]<<" "<<player_cards[1]<<" "<<player_cards[2]<<" "<<player_cards[3]<<" "<<player_cards[4]<<" "<<player_cards[5]<<" "<<player_cards[6]<<"---";
                std::cout<<player_ranks[j]<<" ";
            #endif
        }

        #ifdef DEBUG
            std::cout<<"; "<<min_rank<<"\n";
        #endif

        // those players whose rank is min_rank won this match
        for(int j=0; j<players; ++j){
            if(player_ranks[j] == min_rank)
                win_count[j] += 1;
        }
    }
}



void PokerProbCalculator::calculateProb(){
    // no need to calculate probability if remaining players are <= 1
    std::vector<int> remaining_player_indices = this->getRemainingPlayersIndices();
    if(remaining_player_indices.size() == 0){
        for(size_t i=0; i<this->players_win_probability.size(); i++)
            this->players_win_probability[i] = 0.00;

        return;
    }

    if(remaining_player_indices.size() == 1){
        for(int i=0; i<this->players_win_probability.size(); i++){
            if(i == remaining_player_indices[0])
                this->players_win_probability[i] = 1.00;
            else
                this->players_win_probability[i] = 0.00;
        }

        return;
    }


    // for >=2 remaining players

    // Initialize probabilities to 0.00
    for(size_t i=0; i<this->players_win_probability.size(); i++)
        this->players_win_probability[i] = 0.00;

    // get list of all possible matches
    std::vector<int> deck_indices;
    for(size_t i=0; i<PokerProbCalculator::cards_deck.size(); i++){
        std::string curr_card = PokerProbCalculator::cards_deck[i];
        if((map_of_seen_cards.find(curr_card) == map_of_seen_cards.end()) || (map_of_seen_cards[curr_card] == false))
            deck_indices.push_back(i);
    }

    int remaining_dealt_cards = 5 - dealt_cards.size();
    std::vector<std::vector<int> > indices_combinations = PokerProbCalculator::makeCombinations(deck_indices, remaining_dealt_cards);

    // get matches won by each player
    int size = indices_combinations.size();
    int players = remaining_player_indices.size();
    std::vector<int> match_wins(players, 0);

    #ifdef DEBUG
        std::cout<<"indices_combinations is of size: "<<size<<"\n";
        std::cout<<"remaining players number: "<<players<<"\n";
    #endif

    if(size >= 5e4){
        // distribute work into 2 threads
        std::vector<int> match_wins_1(players, 0);
        std::vector<int> match_wins_2(players, 0);

        std::thread t1(&PokerProbCalculator::calculateWinCount, this, std::ref(indices_combinations), std::ref(match_wins_1), std::ref(remaining_player_indices), std::ref(deck_indices), 0, size/2);
        std::thread t2(&PokerProbCalculator::calculateWinCount, this, std::ref(indices_combinations), std::ref(match_wins_2), std::ref(remaining_player_indices), std::ref(deck_indices), size/2, size);

        t1.join();
        t2.join();

        for(int i=0; i<players; ++i)
            match_wins[i] = (match_wins_1[i] + match_wins_2[i]);
    }
    else{
        calculateWinCount(indices_combinations, match_wins, remaining_player_indices, deck_indices, 0, size);
    }

    // update probabilities of player
    for(int i=0; i<players; ++i){
        float res = 0.00;
        res = (float)match_wins[i];
        res /= (float)size;
        this->players_win_probability[remaining_player_indices[i]] = res;
    }
}