#ifndef POKER_7_CARDS_HAND_H
#define POKER_7_CARDS_HAND_H

#include "Poker5CardsHand.h"

namespace PokerProbability{
  class Poker7CardsHand{
  private:
      const static int perm7[21][5];
      std::string cards[7];

  public:

      // An example of card: AH -- Ace of hearts
      Poker7CardsHand(std::string cards[7]){
          this->cards[0] = cards[0]; this->cards[1] = cards[1]; this->cards[2] = cards[2];
          this->cards[3] = cards[3]; this->cards[4] = cards[4]; this->cards[5] = cards[5];
          this->cards[6] = cards[6];
      }

      short int getRank(){
          short int min_rank = 7643;
          
          // iterate over all permutations
          std::string data[5];
          for(short int i = 0; i<21; ++i){
            data[0] = this->cards[Poker7CardsHand::perm7[i][0]]; data[1] = this->cards[Poker7CardsHand::perm7[i][1]]; data[2] = this->cards[Poker7CardsHand::perm7[i][2]];
            data[3] = this->cards[Poker7CardsHand::perm7[i][3]]; data[4] = this->cards[Poker7CardsHand::perm7[i][4]];
            
            Poker5CardsHand eval(data);
            short int curr_rank = eval.getRank();

            if(curr_rank < min_rank)
              min_rank = curr_rank;
          }

          return min_rank;
      }
      
  };

  const int Poker7CardsHand::perm7[21][5] = {
        { 0, 1, 2, 3, 4 },
        { 0, 1, 2, 3, 5 },
        { 0, 1, 2, 3, 6 },
        { 0, 1, 2, 4, 5 },
        { 0, 1, 2, 4, 6 },
        { 0, 1, 2, 5, 6 },
        { 0, 1, 3, 4, 5 },
        { 0, 1, 3, 4, 6 },
        { 0, 1, 3, 5, 6 },
        { 0, 1, 4, 5, 6 },
        { 0, 2, 3, 4, 5 },
        { 0, 2, 3, 4, 6 },
        { 0, 2, 3, 5, 6 },
        { 0, 2, 4, 5, 6 },
        { 0, 3, 4, 5, 6 },
        { 1, 2, 3, 4, 5 },
        { 1, 2, 3, 4, 6 },
        { 1, 2, 3, 5, 6 },
        { 1, 2, 4, 5, 6 },
        { 1, 3, 4, 5, 6 },
        { 2, 3, 4, 5, 6 }
      };
}




#endif