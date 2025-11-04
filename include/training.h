#pragma once
#include "global.h"
#include "qlearning.h"

using namespace std;

class Game {
protected:
    vector<string> hand;
    vector<string> hand2;
    int card_val = 0;
    double bet_amt = 0;

public:
    void deal(){
        uniform_int_distribution<int>dist(0, 51);
        int i = dist(rng);
        hand.push_back(cards[i]);
        card_val = get_hand_val();
    }
    int get_hand_val(){
        int c = 0;
        for (auto &h : hand) {
            if (h[0] == 'A') {
                c += (c + 11 > 21) ? 1 : 11;
                continue;
            }
            else c+= (h.size() == 3) ? 10 : card_vals[h[0]];
        }
        return c;
    }
    virtual void show_hand() {
        if (verbose) {
            for (auto h : hand) {
                cout << h << " ";
            }
            cout <<"    "<<get_hand_val()<<endl;
        }
    }

  bool is_soft() {
    int total = 0;
    int ace_count = 0;

    for (auto &h : hand) {
        if (h[0] == 'A') ace_count++;
        else total += (h.size() == 3) ? 10 : card_vals[h[0]];
    }

    if (ace_count == 0) return false;

    if (total + 11 + (ace_count - 1) <= 21) return true;
    return false;
    }

    void reset(){
        hand = {};
        hand2 = {};
    }
};

class Player : public Game {
private:
    double cash = 0;
    double old_cash = 0;
    double reward = 0;
    RESULT res;
    Action act = Action::NONE;
    bool split=false;
    Node* split_hand;

public:
    
    Player(double _cash) : cash(_cash) { 
        old_cash = cash; 
        card_val = get_hand_val();
    }
    bool can_split(){
        return hand.size() == 2 && hand[0][0] == hand[1][0] && !split;
    }

    bool is_split(){
        return split;
    }
    
    void show_hand() override {
        if (verbose) cout <<"YOUR HAND: ";
        Game::show_hand();
        if (verbose) cout << "BET AMOUNT: " << bet_amt << "   CASH REMAINING: " << cash << '\n' << endl;
    }

    double get_bet_amt() const{return bet_amt; }

    void bet(double amount){bet_amt = amount;}

    void split_hand_1(){
        if (hand.size() ==2){
            hand.erase(hand.begin());
            hand2 = hand;
        }
        deal();
        split = true;
        act = Action::SPLIT;
    }

    void split_hand_2(){ 
        hand = hand2;
        split = true;
        act = Action::SPLIT;
        deal();
    }

    bool is_pair() {
        if (hand[0][0] == hand[1][0] && hand.size()==2) return true;
        return false;
    }
 
    void double_down(){
        bet_amt += bet_amt;
        act = Action::DOUBLE;
        deal();
    }
    void hit(){
        act = Action::HIT;
        deal();
    }
    void stand(){
        act = Action::STAND;
    }
    void surrender(){
        cash -= 0.5 * (bet_amt);
        act = Action::SURRENDER;
    }

    double get_cash() const{ return cash; }

    RESULT get_result() const { return res; }
    Action get_action() const {return act; }

    void settleBet(RESULT result){
        double actual_bet = bet_amt * old_cash;  // Convert proportion to actual amount
        switch (result) {
            case RESULT::WIN:
                cash += actual_bet; 
                res = RESULT::WIN;
                break; 
            case RESULT::LOSS:
                cash -= actual_bet;
                res = RESULT::LOSS;
                break;
            case RESULT::TIE:
                res = RESULT::TIE;
                break;
            case RESULT::BLACKJACK:
                cash += actual_bet * 1.5;  
                res = RESULT::BLACKJACK;
                break;
        }
    }

    int get_hand_size() const { 
        return (int)hand.size();
    }
};

class Dealer : public Game {
private:
    vector<string> original_hand;
    int original_card_val = 0;
public:
    Dealer(){
        deal();
        original_hand = hand;
        original_card_val = card_val;
    }
    void play(){
        while (get_hand_val() < 17) {
            deal();
        }   
    }
    void show_hand() override{
        if (verbose) cout <<"DEALER'S HAND: ";
        Game::show_hand();
    }
    int original_hand_val(){return original_card_val; }

    void reset_for_splits(){
        hand = original_hand;
    }
};

Node* run_ep(Player &player, Dealer &dealer, bool is_split);
int init_training_ep(Player &player, Dealer &dealer);
Action get_action(Player &player, Dealer &dealer, bool training);
void train();
