#pragma once
#include "global.h"
#include "simulation.h"

using namespace std;

void run_game();

int add(int x, int y);

class Game {
protected:
    vector<string> hand;
    vector<string> hand_2;  //for splitting
    int card_val = 0;
    int card_val2 = 0;
    double bet_amt = 0;
    ACTION act = ACTION::NONE;

public:

    void deal();
    int get_hand_val();
    virtual void show_hand();

    void hit();
    void stand();
};

class Player : public Game {
private:
    double cash = 0;
    double old_cash = 0;
    double reward = 0;
    int turn = 0;

public:
    Player(double _cash);

    void show_hand() override;
    double get_bet_amt() const;

    void update_turn(int i);
    int get_turn() const;
    void bet(double amount);
    void split();
    void double_down();
    void surrender();
    
    double settleBet(RESULT res);

    ACTION get_action() const;
};

class Dealer : public Game {

};