#include "global.h"
#include "helpers.h"
#include "simulation.h"
#include "game.h"

using namespace std;
using Reward = double;

mt19937 rng;

int add(int x, int y) {
    return x + y;
}
/*===================== GAME CLASS SETUP =====================*/

void Game::deal() {
    uniform_int_distribution<int>dist(0, 51);
    int i = dist(rng);
    hand.push_back(cards[i]);
    
    if (act == ACTION::SPLIT){
        int j = dist(rng);
        hand_2.push_back(cards[j]);
    }
}
void Game::show_hand() {
    for (auto h : hand) {
        cout << h << " ";
    }
    cout << "TOTAL: " << card_val << endl;
}
int Game::get_hand_val() {
    int c = 0;
    for (auto &h : hand) {
        int face = (h.size() == 3) ? 10 : card_vals[h[0]];
        c += face;
    }
    card_val = c;
    return card_val;
}
void Game::hit() { act = ACTION::HIT; deal(); }
void Game::stand() { act = ACTION::STAND; }

/*===================== PLAYER CLASS SETUP =================*/

Player::Player(double _cash) : cash(_cash) { old_cash = cash; }

void Player::show_hand() {
    Game::show_hand();
    cout << "     ";
    for (auto h : hand_2) {
        cout << h << " ";
    }
    cout << endl;
    cout << "BET AMOUNT: " << bet_amt * old_cash << "   CASH REMAINING: " << cash << '\n' << endl;
}

double Player::get_bet_amt() const{ return bet_amt; }
void Player::update_turn(int i) { turn += i; }
int Player::get_turn() const{ return turn; }

void Player::bet(double amount) {bet_amt = amount;}
void Player::split() { 
    act = ACTION::SPLIT;
    hand_2.push_back(hand[0]);
    hand.erase(hand.begin());
    deal();
}
void Player::double_down() {act = ACTION::DOUBLE;}
void Player::surrender() {cash -= 0.5 * (bet_amt);}

double Player::settleBet(RESULT res) {
    switch (res) {
    case RESULT::WIN: cash += bet_amt; return bet_amt;
    case RESULT::LOSS: cash -= bet_amt; return -bet_amt;
    default: return 0;
    }
}
ACTION Player::get_action() const{return act;}


Player player(100);
Dealer dealer;
/* ================= UPDATE Q TABLE VALUES =====================*/

static optional<Reward> get_reward() {
    if (dealer.get_hand_val() > player.get_hand_val() && dealer.get_hand_val() <= 21 || player.get_hand_val() > 21) {
        cout << "YOU LOST!" << endl;
        return player.settleBet(RESULT::LOSS);
    }
    if (dealer.get_hand_val() < player.get_hand_val() && player.get_hand_val() <= 21 || (dealer.get_hand_val() > 21 && player.get_hand_val() < 21)) {
        cout << "YOU WON!" << endl;
        return player.settleBet(RESULT::WIN);
    }
    if (dealer.get_hand_val() == player.get_hand_val() && player.get_hand_val() <= 21 && dealer.get_hand_val() <= 21) {
        cout << "YOU TIED!" << endl;
        return player.settleBet(RESULT::TIE);
    }
    return nullopt; 
}

static State update_and_return_state() {
    State state;
    state.amt_bet = player.get_bet_amt();
    state.hand_val = player.get_hand_val();
    state.dealer_val = dealer.get_hand_val();

    if (Q_TABLE.find(state) == Q_TABLE.end()) {
        state.id += Q_TABLE.size() + 1;
    }
    return state;
}


/*========================= RUN GAME =========================*/

void run_game() {
    rng.seed(time(nullptr));

    //run the simulation and try to find best moves
    player.bet(0.2);
    cout << "YOUR BET: " << player.get_bet_amt() << endl;

    player.deal();
    player.deal();
    dealer.deal();

    cout << "YOUR HAND: ";
    player.show_hand();

    cout << "DEALER'S HAND: ";
    dealer.show_hand();
    cout << "\n\n\n";

    ACTION act = player.get_action();
    int i = 0;
    bool busted = false;

    while (act != ACTION::STAND){
        
        if (i == 0)player.split();
        else if (i != 0) player.hit();
        ++i;
        cout << "YOUR HAND: ";
        player.show_hand();
 
        act = player.get_action();
        player.update_turn(i);

        State state = update_and_return_state();
        Node* node = create_node(state, act);
        if (player.get_hand_val() > 21) {
            busted = true; 
            break;
        }
       
    }

    //DEALER
    while (dealer.get_hand_val() <= 17 && !busted) {
        dealer.deal();
    }

    cout << "DEALER'S HAND: ";
    dealer.show_hand();

    optional<Reward> reward = get_reward();
    cout << endl;
    if (reward) cout << *reward << endl;
}