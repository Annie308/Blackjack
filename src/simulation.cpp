#include "game.h"
#include "global.h"
#include "simulation.h"

using namespace std;

//Store both to file. When we want to look up a state first look up its id. Actions are numbered 0-5 in the enum
vector<vector<double>> Q_TABLE;
unordered_map<State, int, HashSt> STATES;
string q_table_file = "C:\\Users\\annie\\Documents\\CppProjects\\Backjack\\include\\q_table.csv";
string states_file =  "C:\\Users\\annie\\Documents\\CppProjects\\Backjack\\include\\states.csv";

Node* create_node(State state, Action action){
    Node* new_node = new Node();
    new_node ->state = state;
    new_node->action = action;
    new_node ->reward = 0;
    new_node -> prob = 0;      
    return new_node;
}

/* ================= FETCH Q TABLE =============================*/

int get_state_id(const State& s) {
    auto it = STATES.find(s);
    if (it != STATES.end()) {
        int existing_id = it->second;
        while (Q_TABLE.size() <= existing_id) {
            Q_TABLE.push_back(vector<double>(5, 0.0));
        }
        return existing_id;
    }

    int new_id = (int)STATES.size();
    while (Q_TABLE.size() <= new_id) {
        Q_TABLE.push_back(vector<double>(5, 0.0));
    }
    STATES[s] = new_id;
    return new_id;
}

void fetch_table(){
    ifstream infile(q_table_file);

    if (!infile.is_open()){
        cerr << "Cannot open file for reading."<<endl;
        // Initialize with empty table if file can't be opened
        Q_TABLE.clear();
        return;
    }

    int i =0;
    string line;

    while (getline(infile, line)){
        string str_val;
        stringstream ss(line);
        vector<double> row;
        for (int i=0; i< 5; i++){
            getline(ss, str_val, ',');
            double val = stod(str_val);
            row.push_back(val);
        }
        Q_TABLE.push_back(row);
    }

}

void fetch_states(){
    ifstream infile(states_file);

    if (!infile.is_open()){
        cerr << "Cannot open file for reading."<<endl;
        return;
    }

    string line;
    int i =0;

    while (getline(infile, line)){
        State state;
        stringstream ss(line);

        string str_id;
        string str_hand_val;
        string str_dealer_val;
        string str_bet_amt;

        getline(ss, str_id, ',');
        getline(ss, str_hand_val, ',');
        getline(ss, str_dealer_val, ',');
        getline(ss, str_bet_amt, ',');

        state.id = stoi(str_id);
        state.hand_val = stoi(str_hand_val);
        state.dealer_val = stoi(str_dealer_val);
        state.amt_bet = stod(str_bet_amt);
        state.id = i;
        ++i;

        STATES.emplace(state, state.id);
    }
}

/* ================= UPDATE Q TABLE VALUES =====================*/

State update_and_return_state(Player &player, Dealer &dealer) {
    State state;
    state.hand_val = player.get_hand_val();
    state.dealer_val = dealer.get_hand_val();
    state.amt_bet = player.get_bet_amt();
    state.id = get_state_id(state);
    return state;
}
void update_table(Node* n) {
    if (!n) return;

    int state_idx = n->state.id;
    int action_idx = static_cast<int>(n->action);
    
    // Validate indices
    if (state_idx < 0 || action_idx < 0 || action_idx >= 5) {
        if (verbose) cout << "Invalid indices - State: " << state_idx << " Action: " << action_idx << endl;
        return;
    }

    // Make sure we have space in Q_TABLE
    while (Q_TABLE.size() <= state_idx) {
        Q_TABLE.push_back(vector<double>(5, 0.0));
    }

    Q_TABLE[state_idx][action_idx] += learning_rate * (n->reward);

    if (verbose){
    cout << "ID: " << state_idx
         << " HAND: " << n->state.hand_val
         << " ACTION: ";
         switch(action_idx){
            case 0: cout << "HIT"; break;
            case 1: cout << "STAND"; break;
            case 2: cout << "SPLIT"; break;
            case 3: cout << "DOUBLE"; break;
            case 4: cout << "NONE"; break;
            default: cout << "UNKNOWN"; break;
         }
         cout<< " BET: " << n->state.amt_bet
         << " REWARD: " << n->reward 
         <<endl;
    }

    if (n->previous) {
        n->previous->reward = discount_factor*n->reward;
        update_table(n->previous);
    }
}

/* ============== SAVE Q_TABLE TO FILE ========================*/

void write_table_to_file(){
    ofstream outfile(q_table_file);
    
    if (outfile.is_open()){
        for (auto &vec: Q_TABLE){
            for (auto &val: vec){
                outfile << val<<",";
            }
            outfile <<endl;
        }
    }
    else{
        cerr <<"Cannot open file for writing. "<<endl;
        return;
    }
}

void write_states_to_file(){
    ofstream outfile(states_file);
    
    if (outfile.is_open()){
        for (auto &[state, id]: STATES){
            outfile <<id<<","<<state.hand_val<<","<<state.dealer_val<<","<<state.amt_bet<<endl;
        }
    }
    else{
        cerr <<"Cannot open file for writing. "<<endl;
        return;
    }
}

void print_table(){
    for (int row=0; row<Q_TABLE.size(); row++){
        cout <<row<<": ";
        for (int col=0; col<Q_TABLE[row].size(); col++){
            cout <<Q_TABLE[row][col]<<" ";
        }
        cout <<endl;
    }
}

void cleanNodes(Node*& end) {
    Node* curr = end;
    while (curr != nullptr) {
        Node* temp = curr;   // save pointer to current node
        curr = curr->previous; // move to next node
        delete temp;             // free memory
    }
    end = nullptr; // list is now empty
}

