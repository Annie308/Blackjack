#include "global.h"
#include "training.h"
#include "qlearning.h"

using namespace std;

/* ============== CLEAN UP NODES ========================*/

void cleanNodes(Node*& end) {
    Node* curr = end;
    while (curr != nullptr) {
        Node* temp = curr;   
        curr = curr->previous; 
        delete temp;             
    }
    end = nullptr; 
}

/* ======================= FETCH STATE ID ========================*/

int update_and_return_id(State s, Player &player, Dealer &dealer) {
    s.player_soft = player.is_soft();
    s.is_pair = player.is_pair();

    auto it = STATES.find(s);
    if (it != STATES.end()) {
        int existing_id = it->second;
        while (Q_TABLE.size() <= existing_id) {
             Q_TABLE.push_back({0.0, 0.0, 0.0, 0.0, 0.0});
        }
        return existing_id;
    }

    int new_id = (int)STATES.size();
    while (Q_TABLE.size() <= new_id) {
         Q_TABLE.push_back({0.0, 0.0, 0.0, 0.0, 0.0});
    }

    STATES[s] = new_id;
    return new_id;
}
