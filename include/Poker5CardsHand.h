#ifndef POKER_5_CARDS_HAND_H
#define POKER_5_CARDS_HAND_H

#include "arrays.h"
#include <string>
#include <algorithm>

class Poker5CardsHand{
private:
    /* 
    each card is of type integer (and therefore, four bytes long).

    +--------+--------+--------+--------+
    |xxxbbbbb|bbbbbbbb|cdhsrrrr|xxpppppp|
    +--------+--------+--------+--------+

    p = prime number of rank (deuce=2,trey=3,four=5,...,ace=41)
    r = rank of card (deuce=0,trey=1,four=2,five=3,...,ace=12)
    cdhs = suit of card (bit turned on based on suit of card)
    b = bit turned on depending on rank of card

    examples: xxxAKQJT 98765432 CDHSrrrr xxPPPPPP
              00001000 00000000 01001011 00100101    King of Diamonds
              00000000 00001000 00010011 00000111    Five of Spades
              00000010 00000000 10001001 00011101    Jack of Clubs
    */

    static unsigned int getBinaryFromSuit(char s){
        if(s == 'C')    return 1<<15;
        if(s == 'D')    return 1<<14;
        if(s == 'H')    return 1<<13;
        if(s == 'S')    return 1<<12;
    }

    static unsigned int getBinaryFromRank(char r){
        if(r == '2')    return (1<<16) + (0<<8) + 2;
        if(r == '3')    return (1<<17) + (1<<8) + 3;
        if(r == '4')    return (1<<18) + (2<<8) + 5;
        if(r == '5')    return (1<<19) + (3<<8) + 7;
        if(r == '6')    return (1<<20) + (4<<8) + 11;
        if(r == '7')    return (1<<21) + (5<<8) + 13;
        if(r == '8')    return (1<<22) + (6<<8) + 17;
        if(r == '9')    return (1<<23) + (7<<8) + 19;
        if(r == 'T')    return (1<<24) + (8<<8) + 23;
        if(r == 'J')    return (1<<25) + (9<<8) + 29;
        if(r == 'Q')    return (1<<26) + (10<<8) + 31;
        if(r == 'K')    return (1<<27) + (11<<8) + 37;
        if(r == 'A')    return (1<<28) + (12<<8) + 41;
    }

    static unsigned int convertCardToBitForm(std::string card){
        return getBinaryFromRank(card[0]) + getBinaryFromSuit(card[1]);
    }

    unsigned int bit_cards[5];

public:
    // An example of card: AH -- Ace of hearts
    Poker5CardsHand(const std::string cards[5]){
        for(unsigned int i=0; i<5; i++)
            this->bit_cards[i] = convertCardToBitForm(cards[i]);
    }


    short int getRank(){
        unsigned int c1=bit_cards[0], c2=bit_cards[1], c3=bit_cards[2], c4=bit_cards[3], c5=bit_cards[4];

        // check if flush -- all cards have same suit
        if((c1 & c2 & c3 & c4 & c5 & 0xF000) != 0){
            short int q = (c1|c2|c3|c4|c5) >> 16;
            return flushes[q];
        }
        
        // check if straight or high card -- all have different values
        short int q = (c1|c2|c3|c4|c5) >> 16;
        short straight_or_high_rank = unique5[q];
        if(straight_or_high_rank != 0)
            return straight_or_high_rank;

        // remaining possible hands
        unsigned int prime_mult_val = (c1 & 0xFF) * (c2 & 0xFF) * (c3 & 0xFF) * (c4 & 0xFF) * (c5 & 0xFF);
        unsigned int size = 4887;
        unsigned int index = std::lower_bound(products, products + size, prime_mult_val) - products;
        return values[index];
    }
};

#endif