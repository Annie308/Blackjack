#include "global.h"
#include "qlearning.h"
#include "files.h"

using namespace std;

/* test how stupid my bot is by cross checking with the state and action with the q_value and optimal strategy*/

void test_cases(State s, Action a, bool is_pair, int &failed){
    vector<pair<State, Action>> failed_cases;
    int hand_val = s.hand_val;
    int dealer_val = s.dealer_val;
    bool player_soft = s.player_soft;

    if (!is_pair && !player_soft) {
        // ===== HARD HANDS =====

        // Case 1: 2–8 → HIT
        if (hand_val <= 8) {
            if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Case 2: 9 → DOUBLE if dealer 3–6 else HIT
        else if (hand_val == 9) {
            if (dealer_val >= 3 && dealer_val <= 6) {
                if (a != Action::DOUBLE) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Case 3: 10 → DOUBLE if dealer 2–9 else HIT
        else if (hand_val == 10) {
            if (dealer_val >= 2 && dealer_val <= 9) {
                if (a != Action::DOUBLE) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Case 4: 11 → DOUBLE unless dealer has Ace
        else if (hand_val == 11) {
            if (dealer_val != 11) {
                if (a != Action::DOUBLE) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Case 5: 12 → STAND if dealer 4–6 else HIT
        else if (hand_val == 12) {
            if (dealer_val >= 4 && dealer_val <= 6) {
                if (a != Action::STAND) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Case 6: 13–16 → STAND if dealer 2–6 else HIT
        else if (hand_val >= 13 && hand_val < 16) {
            if (dealer_val <= 6) {
                if (a != Action::STAND) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        //STAND 2-6, HIT 7,8, SURRENDER 9,10,A
        else if (hand_val == 16){
            if (dealer_val <=2 && dealer_val <=6) {
                if (a != Action::STAND) failed_cases.push_back({s,a});
            }
            else if  (dealer_val ==7 || dealer_val ==8) {
                if (a != Action::HIT) failed_cases.push_back({s,a});
            } 
            else if (a != Action::SURRENDER) failed_cases.push_back({s,a});
        }
        // Case 7: 17–21 → STAND
        else if (hand_val >= 17 && hand_val <= 21) {
            if (a != Action::STAND) failed_cases.push_back({s,a});
        }
    }
    else if (player_soft && !is_pair) {
        // ===== SOFT HANDS =====
        // A,2 or A,3 (soft 13–14) → DOUBLE if dealer 5–6 else HIT
        if (hand_val == 13 || hand_val == 14) {
            if (dealer_val == 5 || dealer_val == 6) {
                if (a != Action::DOUBLE) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // A,4 or A,5 (soft 15–16) → DOUBLE if dealer 4–6 else HIT
        else if (hand_val == 15 || hand_val == 16) {
            if (dealer_val >= 4 && dealer_val <= 6) {
                if (a != Action::DOUBLE) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // A,6 (soft 17) → DOUBLE if dealer 3–6 else HIT
        else if (hand_val == 17) {
            if (dealer_val >= 3 && dealer_val <= 6) {
                if (a != Action::DOUBLE) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // A,7 (soft 18) → STAND if dealer 2,7,8; DOUBLE if 3–6; HIT if 9–A
        else if (hand_val == 18) {
            if (dealer_val >= 3 && dealer_val <= 6) {
                if (a != Action::DOUBLE) failed_cases.push_back({s,a});
            } else if (dealer_val == 2 || dealer_val == 7 || dealer_val == 8) {
                if (a != Action::STAND) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // A,8 or A,9 (soft 19–20) → STAND
        else if (hand_val == 19 || hand_val == 20) {
            if (a != Action::STAND) failed_cases.push_back({s,a});
        }
    }
    else if (is_pair) {
        // ===== PAIRS =====
        // Pair of A,A (12) or 8,8 (16) → SPLIT
        if (hand_val == 12 || hand_val == 16) {
            if (a != Action::SPLIT) failed_cases.push_back({s,a});
        }
        // Pair of 2,2 (4) or 3,3 (6) → SPLIT if dealer 2–7 else HIT
        else if (hand_val == 4 || hand_val == 6) {
            if (dealer_val >= 2 && dealer_val <= 7) {
                if (a != Action::SPLIT) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Pair of 4,4 (8) → SPLIT if dealer 5–6 else HIT
        else if (hand_val == 8) {
            if (dealer_val == 5 || dealer_val == 6) {
                if (a != Action::SPLIT) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Pair of 5,5 (10) → DOUBLE if dealer 2–9 else HIT
        else if (hand_val == 10) {
            if (dealer_val >= 2 && dealer_val <= 9) {
                if (a != Action::DOUBLE) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Pair of 6,6 (12) → SPLIT if dealer 2–6 else HIT
        else if (hand_val == 12) {
            if (dealer_val >= 2 && dealer_val <= 6) {
                if (a != Action::SPLIT) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Pair of 7,7 (14) → SPLIT if dealer 2–7 else HIT
        else if (hand_val == 14) {
            if (dealer_val >= 2 && dealer_val <= 7) {
                if (a != Action::SPLIT) failed_cases.push_back({s,a});
            } else if (a != Action::HIT) failed_cases.push_back({s,a});
        }
        // Pair of 9,9 (18) → SPLIT if dealer 2–6 or 8–9 else STAND
        else if (hand_val == 18) {
            if ((dealer_val >= 2 && dealer_val <= 6) || dealer_val == 8 || dealer_val == 9) {
                if (a != Action::SPLIT) failed_cases.push_back({s,a});
            } else if (a != Action::STAND) failed_cases.push_back({s,a});
        }
        // Pair of 10,10 (20) → STAND
        else if (hand_val == 20) {
            if (a != Action::STAND) failed_cases.push_back({s,a});
        }
    }

    failed+= failed_cases.size();

    for (auto &[state, action]: failed_cases){
        if (is_pair){
            cout << "FAILED ID: " << state.id << " HAND: " << state.hand_val/2<<", "<<state.hand_val/2 << " DEALER: " << state.dealer_val<<" ACTION: "<<static_cast<int>(action);
        }
        else if (!is_pair) cout << "FAILED ID: " << state.id << " HAND: " << state.hand_val << " DEALER: " << state.dealer_val<<" ACTION: "<<static_cast<int>(action);
        
        if (s.player_soft) cout <<" SOFT";
        cout <<endl;
    }
}

void validate_table(){
    fetch_states();
    fetch_table();
    int correct = 0;
    int total = 0;

    int failed = 0;

    cout <<"\n==================== STATES LOG ===============\n"<<endl;

    for (auto &[state, id]: STATES){
        if (Q_TABLE.size() <= id) continue;

        double max_q = -1e9;
        int action_idx = -1;
        for (int i=0; i<Q_TABLE[id].size(); i++){
            //ignore actions that's not possible (ex. split when you can't split)
            if (Q_TABLE[id][i]> max_q && Q_TABLE[id][i] != 0.0) {
                max_q = Q_TABLE[id][i];
                action_idx = i;
            }
        }
        
        test_cases(state, static_cast<Action>(action_idx), state.is_pair, failed);
        total++;

    }

    cout <<"\nPASSED: "<<total-failed<<"/"<<total<<endl;
}