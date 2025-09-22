#pragma once
#include "global.h"

struct State{
    int hand_val;
    int dealer_val;
    double amt_bet;               //proportion of money we bet
    int id;

    bool operator<(const State& other) const {
        return id < other.id;
    }
};

enum class ACTION {
    HIT,
    STAND,
    SPLIT,
    DOUBLE,
    NONE
};

enum class RESULT {
    WIN,
    LOSS,
    TIE,
    NONE
};
extern std::map<State, ACTION> Q_TABLE;

struct Node{
    State state;
    ACTION action;
    double prob;
    double reward;
};

Node* create_node(State state, ACTION action);