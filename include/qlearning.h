#pragma once
#include "global.h"


extern double discount_factor;  //immediate rewards vs future rewards
extern double initial_exploration_rate;
extern double minimum_exploration_rate;
extern double decay_rate;
extern double learning_rate;
extern double exploration_prob;  //how often to explore vs exploit

class Player;
class Dealer;

enum class Action {
    HIT,
    STAND,
    DOUBLE,
    SPLIT,
    SURRENDER,
    NONE
};

struct State{
    int hand_val;
    int dealer_val;
    int id;
    bool player_soft=false;
    bool is_pair = false;

    bool operator==(const State& other) const{
        return hand_val == other.hand_val && 
            dealer_val == other.dealer_val;
    }
    bool operator<(const State& other) const {
        return id < other.id;
    }
};

enum class RESULT {
    WIN,
    LOSS,
    TIE,
    BLACKJACK,
    NONE
};

struct HashSt{
    std::size_t operator()(const State &s)const{
        size_t h1 = std::hash<int>()(s.hand_val);
        size_t h2 = std::hash<int>()(s.dealer_val);
        size_t h3 = std::hash<bool>()(s.player_soft);
        size_t h4 = std::hash<bool>()(s.is_pair);
        return (((h1 ^ (h2 << 1)) ^ (h3 << 2)) ^ (h4 << 3));
    }
};

extern std::vector<std::vector<double>> Q_TABLE;
extern std::unordered_map<State, int, HashSt> STATES;
inline std::vector<std::vector<std::pair<double,int>>> value_table;

struct Node{
    State state;
    Action action;
    double reward;
    Node* previous;
};

Node* create_node(State state, Action action);
int update_and_return_id(State s, Player &player, Dealer &dealer);

State update_and_return_state(Player &player, Dealer &dealer);
void update_q_values(Player &player, Node* n, Dealer &dealer, double split_reward);
double get_reward(Player &player, Dealer &dealer, Action action);
void cleanNodes(Node*& end);
double bet_strategy(Player &player);
double get_split_reward(Node* n);

