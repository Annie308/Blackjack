#pragma once
#include "global.h"
#include "simulation.h"

using namespace std;


class Game {
protected:
    vector<string> hand;
    int card_val = 0;
    double bet_amt = 0;
    Action act = Action::NONE;

public:

    void deal();
    int get_hand_val();
    virtual void show_hand();

    void hit();
    void stand();

    void clear_hand();
};

class Player : public Game {
private:
    double cash = 0;
    double old_cash = 0;
    double reward = 0;
    RESULT res = RESULT::NONE;

public:

    bool split = false;

    Player(double _cash);

    void show_hand() override;
    double get_bet_amt() const;

    void bet(double amount);
    void split_hand_1();
    void split_hand_2();
    void double_down();
    void surrender();

    double get_cash() const;
    void settleBet(RESULT res);

    Action get_action() const;
    int get_hand_size() const { return (int)hand.size();}
};

class Dealer : public Game {
public:
    Dealer(){
        //hand = {"2C", "10D"};
        //card_val = get_hand_val();
    }
    void play();
    void show_hand() override;
};

Node* run_game(Player &player, Dealer &dealer);
void game(Player &player, Dealer &dealer);
Action game_strategy(Player &player, Dealer &dealer);
double get_reward(Player &player, Dealer &dealer);