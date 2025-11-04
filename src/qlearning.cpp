#include "training.h"
#include "global.h"
#include "qlearning.h"

using namespace std;

//Store both to file. When we want to look up a state first look up its id. Actions are numbered 0-5 in the enum
vector<vector<double>> Q_TABLE;
unordered_map<State, int, HashSt> STATES;


Node* create_node(State state, Action action){
    Node* new_node = new Node();
    new_node ->state = state;
    new_node->action = action;
    new_node ->reward;
    return new_node;
}

double bet_strategy(Player &player){
    uniform_int_distribution<int> dist(1,10);
    return static_cast<double>(dist(rng))/10.0;
}

Action get_action(Player &player, Dealer &dealer, bool training) {
    vector<pair<double, Action>> q_to_act;  
    Action action = Action::NONE; // default

    int hand_val = player.get_hand_val();

    for (auto &[state, id] : STATES) {
        if (hand_val == state.hand_val && 
            dealer.original_hand_val() == state.dealer_val &&
            state.player_soft==player.is_soft() &&
            state.is_pair == player.is_pair()) 
        {
            while (Q_TABLE.size() <= id){
                Q_TABLE.resize(id+1);
                Q_TABLE[id] = {0.0,0.0,0.0,0.0,0.0};
            }
            while (action == Action::NONE){
                for (auto act : {Action::HIT, Action::STAND, Action::DOUBLE, Action::SPLIT, Action::SURRENDER}) {
                    if (!player.can_split() && act == Action::SPLIT) continue;  //cannot split if not a pair or already split
                    if ((player.get_action() != Action::NONE && player.get_action() != Action::SPLIT) && act == Action::DOUBLE) continue; // cannot double after first turn unless split
                    if (player.get_action() != Action::NONE && act == Action::SURRENDER) continue; //cannot surrender after first turn
                    action = act;
                    q_to_act.push_back({Q_TABLE[id][static_cast<int>(action)], action});
                }
            }
        }
    uniform_int_distribution<int> dist (1,10);
    if (training && dist(rng) < exploration_prob*10) {
        int hand_size = player.get_hand_size();
        if(hand_size == 2) {
            do{
                action = static_cast<Action>(rand() % 5);
            }while ((action == Action::DOUBLE && player.get_bet_amt() > 0.5) || (action == Action::SPLIT && !player.can_split()));
        }
        //Cant double, split, or surrender after making a move
        else if (hand_size == 2 && player.get_action() == Action::NONE) {
            action = static_cast<Action>(rand() % 2); 
        }
    } 
    else {
        if (!q_to_act.empty()) {
            auto max_pair = *max_element(q_to_act.begin(), q_to_act.end(),
                                        [](const auto &a, const auto &b){ return a.first < b.first; });
            action = max_pair.second;
        }
    }
    }
    return action;
}

double get_split_reward(Node* n){
    if (!n) return 0.0;
    if (n->previous){
        n->previous->reward = discount_factor*n->reward;
        return get_split_reward(n->previous);
    }
    else{
        return n->reward;
    } 
}

double get_reward(Player &player, Dealer &dealer, Action action) {
    double reward = 0.0;
    //return early if surrender
    if (action == Action::SURRENDER) {
        if (verbose) cout << "YOU SURRENDERED!" << endl;
        return -0.5;
    }

    if (player.get_hand_val() == 21 && dealer.get_hand_val() != 21) {
        if (verbose) cout << "BLACKJACK!" << endl;
        player.settleBet(RESULT::BLACKJACK);
        reward = 1.5;
    }
    else if (player.get_hand_val() == 21 && dealer.get_hand_val() == 21 ||
(dealer.get_hand_val() == player.get_hand_val() && player.get_hand_val() <= 21 && dealer.get_hand_val() <= 21)) {
        if (verbose) cout << "YOU TIED!" << endl;
        player.settleBet(RESULT::TIE);
        reward = 0.0;
    }
    else if (dealer.get_hand_val() > player.get_hand_val() && dealer.get_hand_val() <= 21 || player.get_hand_val() > 21) {
        if (verbose) cout << "YOU LOST!" << endl;
        player.settleBet(RESULT::LOSS);
        reward= -1.0;
    }
    else if ((dealer.get_hand_val() < player.get_hand_val() && player.get_hand_val() <= 21)|| (dealer.get_hand_val() > 21 && player.get_hand_val() < 21)) {
        if (verbose) cout << "YOU WON! HAND VAL: "<<player.get_hand_val() << endl;
        player.settleBet(RESULT::WIN);
        reward= 1.0;
    }

    if (action == Action::DOUBLE) reward *= 2.0;
    
    return reward; 
}

State update_and_return_state(Player &player, Dealer &dealer) {
    State state;
    state.hand_val = player.get_hand_val();
    state.dealer_val = dealer.original_hand_val();
    state.player_soft = player.is_soft();
    state.is_pair = player.is_pair();
    state.id = update_and_return_id(state, player, dealer);
    STATES.emplace(state, state.id);
    return state;
}

void update_q_values(Player &player, Node* n, Dealer &dealer, double split_reward){
    if (!n) return;
    
    int state_idx = n->state.id;
    int action_idx = static_cast<int>(n->action);

    //validate indices
    if (state_idx < 0 || action_idx < 0 || action_idx >= 5) {
        if (verbose) cout << "Invalid indices - State: " << state_idx << " Action: " << action_idx << endl;
        return;
    }

    //make sure we have space in Q_TABLE. Do not write end states
    while (value_table.size() <= state_idx && n->state.hand_val < 21) {
        value_table.push_back({{0.0,0}, {0.0,0}, {0.0,0}, {0.0,0}, {0.0,0}});
    }

    // Update rule: Q = 1/N* SUM(Q(s,a)), where .first is reward and .second is visit counts
    if (n->state.hand_val < 21) {
        value_table[state_idx][action_idx].first += n->reward;
        value_table[state_idx][action_idx].second ++;
    }

    if (verbose){
    cout << "ID: " << state_idx
         << " HAND: " << n->state.hand_val
         << " DEALER: " << n->state.dealer_val
         << " ACTION: ";
         switch(action_idx){
            case 0: cout << "HIT"; break;
            case 1: cout << "STAND"; break;
            case 2: cout << "DOUBLE"; break;
            case 3: cout << "SPLIT"; break;
            case 4: cout << "SURRENDER"; break;
            default: cout << "UNKNOWN"; break;
         }
         cout << " REWARD: " << n->reward <<endl;
    }
    if (n->previous) {
        //if the previous action was a split, add the split reward to the discounted reward. Accounts for both branches after split
        if (n->previous->action == Action::SPLIT) n->previous->reward = discount_factor*(n->reward+split_reward);
        else n->previous->reward = discount_factor*n->reward;
        update_q_values(player, n->previous, dealer, split_reward);
    }
}


