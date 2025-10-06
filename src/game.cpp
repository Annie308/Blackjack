#include "global.h"
#include "simulation.h"
#include "game.h"

using namespace std;
using Reward = double;

int add(int x, int y) {
    return x + y;
}
/*===================== GAME CLASS SETUP =====================*/

void Game::deal() {
    uniform_int_distribution<int>dist(0, 51);
    int i = dist(rng);
    hand.push_back(cards[i]);
    card_val = get_hand_val();
}
void Game::show_hand() {
    if (verbose) {
        for (auto h : hand) {
            cout << h << " ";
        }
        cout << "TOTAL: " << card_val << endl;
    }
}
int Game::get_hand_val() {
    int c = 0;
    for (auto &h : hand) {
        int face = (h.size() == 3) ? 10 : card_vals[h[0]];
        c += face;
    }
    return c;
}
void Game::hit() { 
    act = Action::HIT; 
    deal(); 
}
void Game::stand() { act = Action::STAND; }
void Game::clear_hand(){ hand = {};}

/*===================== PLAYER CLASS SETUP =================*/

Player::Player(double _cash) : cash(_cash) { 
    old_cash = cash; 
    card_val = get_hand_val();
}

void Player::show_hand() {
    if (verbose) cout <<"YOUR HAND: ";
    Game::show_hand();
    if (verbose) cout << "BET AMOUNT: " << bet_amt * cash << "   CASH REMAINING: " << cash << '\n' << endl;
}

double Player::get_bet_amt() const{ return bet_amt; }
double Player::get_cash() const { return cash; }

void Player::bet(double amount) {
    bet_amt = amount;
}
void Player::split_hand_1() { 
    if (hand.size() ==2) hand.erase(hand.begin());
    deal();
}
void Player::split_hand_2(){
    if (hand.size() == 2) hand.pop_back();
    deal();
}
void Player::double_down() {
    act = Action::DOUBLE;
    bet_amt += bet_amt;
}
void Player::surrender() {cash -= 0.5 * (bet_amt);}

void Player::settleBet(RESULT res) {
    double actual_bet = bet_amt * old_cash;  // Convert proportion to actual amount
    switch (res) {
    case RESULT::WIN:
        cash += actual_bet * 1.5;  
    case RESULT::LOSS:
        cash -= actual_bet;
    }
}
Action Player::get_action() const{return act;}

void Dealer::play(){
    while (get_hand_val() <= 17) {
        deal();
    }
}
void Dealer::show_hand(){
    if (verbose) cout <<"DEALER'S HAND: ";
    Game::show_hand();
}

double get_reward(Player &player, Dealer &dealer) {
    if (player.get_hand_val() == 21 && dealer.get_hand_val() != 21) {
        if (verbose) cout << "YOU WON!" << endl;
        player.settleBet(RESULT::WIN);
        return (player.get_action() == Action::DOUBLE) ? 2.0 : 1.0;
    }
    if ((dealer.get_hand_val() > player.get_hand_val() && dealer.get_hand_val() <= 21) || player.get_hand_val() > 21) {
        if (verbose) cout << "YOU LOST!" << endl;
        player.settleBet(RESULT::LOSS);
        return (player.get_action() == Action::DOUBLE) ? -2.0 : -1.0;
    }
    if ((dealer.get_hand_val() < player.get_hand_val() && player.get_hand_val() <= 21)|| (dealer.get_hand_val() > 21 && player.get_hand_val() < 21)) {
        if (verbose) cout << "YOU WON! HAND VAL: "<<player.get_hand_val() << endl;
        player.settleBet(RESULT::WIN);
        return (player.get_action() == Action::DOUBLE) ? 2.0 : 1.0;
    }
    if (dealer.get_hand_val() == player.get_hand_val() && player.get_hand_val() <= 21 && dealer.get_hand_val() <= 21) {
        if (verbose) cout << "YOU TIED!" << endl;
        player.settleBet(RESULT::TIE);
        return 0.0;
    }
    if (player.get_cash() <=0){
        if (verbose) cout << "YOU LOST! OUT OF CASH!" << endl;
        player.settleBet(RESULT::LOSS);
        return -1;
    }
    return 0.0; 
}

/*========================= RUN GAME =========================*/

Node* run_game(Player &player, Dealer &dealer) {
    bool busted = false;

    Node* prev_node = nullptr;
    Node* end_node = nullptr;

    while (true){
        player.show_hand();

        Action act = game_strategy(player, dealer);
        State state = update_and_return_state(player, dealer);
        
        Node* n = create_node(state, act);
        n->previous = prev_node;
        prev_node = n;

        if (act == Action::HIT) player.hit();

        else if (act == Action::STAND) {
            end_node = n;
            break;
        }
        else if (act == Action::DOUBLE) {
            player.double_down();  // apply hit + double bet
            end_node = n;
            break;
        }
    
        if (player.get_hand_val() == 21){
            end_node = n;
            break;
        }
        if (player.get_hand_val() > 21){
            busted = true;
            end_node = n;
            break;
        }
    }

    if (!busted){
        dealer.play();
        dealer.show_hand();
    }

    return end_node;
}

void training(Player &player, Node* node, Dealer &dealer){
    double reward = get_reward(player, dealer);
    if (verbose) {
        cout <<endl;
        cout <<"========= GAME LOG ================="<<endl;
    }

    if (node) {
        node->reward = reward;
        update_table(node);
        cleanNodes(node);
    }
}

double bet_strategy(Player &player){

    uniform_int_distribution<int> dist(1,10);
    return static_cast<double>(dist(rng))/10.0;
}

Action game_strategy(Player &player, Dealer &dealer) {
    // use vector instead of map
    vector<pair<double, Action>> q_to_act;  
    Action action = Action::NONE; // default

    for (auto &[state, id] : STATES) {
        if (player.get_hand_val() == state.hand_val && 
            dealer.get_hand_val() == state.dealer_val &&
            state.amt_bet == player.get_bet_amt()) 
        {
            if (Q_TABLE.size() <= id){
                Q_TABLE.resize(id+1);
                Q_TABLE[id] = {0,0,0,0,0};
            }
            for (auto act : {Action::HIT, Action::STAND, Action::SPLIT, Action::DOUBLE}) {
                q_to_act.push_back({Q_TABLE[id][static_cast<int>(act)], act});
            }
        }

    if (rand() % 10 < exploration_prob) {
        if(player.get_hand_size() == 2) {
            action = static_cast<Action>(rand() % 3); 
        }
        else {
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

void game(Player &player, Dealer &dealer){
    if (verbose) {
        cout <<endl;
        cout <<"========= INITIALIZING GAME ==========="<<endl;
    }
    double bet_amt = bet_strategy(player);
    player.bet(bet_amt);

    if (verbose) cout << "YOUR BET: " << player.get_bet_amt() << endl;
    
    player.deal();
    player.deal();
    dealer.deal();

    if (verbose) cout << "YOUR HAND: ";
    player.show_hand();

    if (verbose) cout << "DEALER'S HAND: ";
    dealer.show_hand();

    bool split = false;
    bool double_down = false;

    if (game_strategy(player, dealer) == Action::DOUBLE) {
        double_down = true;
    }
    if (game_strategy(player, dealer) == Action::SPLIT) {
        split = true;
        player.split_hand_1();
    }
    if (verbose) {
        cout <<endl;
        cout <<"========== RUNNING GAME ==========="<<endl;
    }
    
    if (double_down) player.double_down();
    Node* trajectory = run_game(player, dealer);
    training(player, trajectory, dealer);

    dealer.clear_hand();
    player.clear_hand();
}