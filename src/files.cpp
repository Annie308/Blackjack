#include "global.h"
#include "qlearning.h"
#include "training.h"

using namespace std;

string q_table_file = "C:\\Users\\annie\\Documents\\CppProjects\\Backjack\\include\\q_table.csv";
string states_file =  "C:\\Users\\annie\\Documents\\CppProjects\\Backjack\\include\\states.csv";

void fetch_table(){
    Q_TABLE.clear();
    value_table.clear();
    ifstream infile(q_table_file);

    if (!infile.is_open()){
        cerr << "Error! Cannot open table file for reading."<<endl;
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
    STATES.clear();
    ifstream infile(states_file);

    if (!infile.is_open()){
        cerr << "Error! Cannot open states file for reading."<<endl;
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
        string str_player_soft;
        string str_is_pair;


        getline(ss, str_id, ',');
        getline(ss, str_hand_val, ',');
        getline(ss, str_dealer_val, ',');
        getline(ss, str_player_soft, ',');
        getline(ss, str_is_pair, ',');

        state.id = stoi(str_id);
        state.hand_val = stoi(str_hand_val);
        state.dealer_val = stoi(str_dealer_val);
        state.player_soft = (str_player_soft == "1") ? true : false;
        state.is_pair = (str_is_pair == "1") ? true : false;

        STATES.emplace(state, state.id);
    }
}

/* ============== SAVE Q_TABLE TO FILE ========================*/

void update_q_table(){
    int total_visited = 0;

    for (int state_idx =0; state_idx < (int)Q_TABLE.size(); state_idx++){
        for (int action_idx=0; action_idx < (int)Q_TABLE[state_idx].size(); action_idx++){

            double sum_reward;
            int visited;
            if (state_idx < value_table.size() && action_idx < value_table[state_idx].size()){
                sum_reward = value_table[state_idx][action_idx].first;
            }
            else continue;

            if (state_idx < value_table.size() && action_idx < value_table[state_idx].size()){
                visited = value_table[state_idx][action_idx].second;
            }
            else continue;

            if (visited >0){
                double G = sum_reward/(double)visited; //average return
                double V =  Q_TABLE[state_idx][action_idx];

                Q_TABLE[state_idx][action_idx] = V + learning_rate*(G-V);

                cout << "ID: " << state_idx
                << " ACTION: ";
                switch(action_idx){
                    case 0: cout << "HIT"; break;
                    case 1: cout << "STAND"; break;
                    case 2: cout << "DOUBLE"; break;
                    case 3: cout << "SPLIT"; break;
                    case 4: cout << "SURRENDER"; break;
                    default: cout << "UNKNOWN"; break;
                }
                cout <<" REWARD: "<<sum_reward
                << " VISITED: " << visited
                << " UPDATED Q VAL FROM: "<<  V <<" TO: "<<  Q_TABLE[state_idx][action_idx]<<endl;
                total_visited+=visited;
            }
        }
    }

    cout <<"TOTAL STATES VISTED: "<<total_visited<<endl;
}

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

/* ===================== SAVE STATES TO FILE ==============*/

void write_states_to_file(){
    ofstream outfile(states_file);
    
    if (outfile.is_open()){
        for (auto &[state, id]: STATES){
            if (state.hand_val >=21) continue;
            string player_soft = (state.player_soft) ? "1": "0";
            string is_pair = (state.is_pair) ? "1": "0";
            outfile <<id<<","<<state.hand_val<<","<<state.dealer_val<<","<<player_soft<<","<<is_pair<<endl;
        }
    }
    else{
        cerr <<"Cannot open file for writing. "<<endl;
        return;
    }
}