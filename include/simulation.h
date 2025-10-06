#pragma once
#include "global.h"


inline double learning_rate = 0.8;     //how much to change old Q-values
inline double discount_factor = 0.95;  //immediate rewards vs future rewards
inline double exploration_prob = 1-learning_rate;  //how often to explore vs exploit
inline double epochs = 1000;           //how many times to run

class Player;
class Dealer;

enum class Action {
    HIT,
    STAND,
    DOUBLE,
    SPLIT,
    NONE
};

struct State{
    int hand_val;
    int dealer_val;
    double amt_bet;               //proportion of money we bet
    int id;
    Action action;

    bool operator==(const State& other) const{
        return hand_val == other.hand_val && 
            dealer_val == other.dealer_val && 
            fabs(amt_bet - other.amt_bet) < 1e-6;
    }
    bool operator<(const State& other) const {
        return id < other.id;
    }
};

enum class RESULT {
    WIN,
    LOSS,
    TIE,
    NONE
};

struct HashSt{
    std::size_t operator()(const State &s)const{
        std::size_t h1 = std::hash<int>()(s.hand_val);
        std::size_t h2 = std::hash<int>()(s.dealer_val);
        std::size_t h3 = std::hash<int>()(static_cast<int>(s.action));
        std::size_t h4 = std::hash<int>()(static_cast<int>(s.amt_bet*100));
        return (((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1)) ^ (h4 << 2);
    }
};

extern std::vector<std::vector<double>> Q_TABLE;
extern std::unordered_map<State, int, HashSt> STATES;

struct Node{
    State state;
    Action action;
    double prob;
    double reward;
    Node* previous;
};

Node* create_node(State state, Action action);
int get_state_id(const State& s);

void fetch_states();
void fetch_table();
void write_table_to_file();
void write_states_to_file();

State update_and_return_state(Player &player, Dealer &dealer);
void update_table(Node* n);
void print_table();
void cleanNodes(Node*& end);