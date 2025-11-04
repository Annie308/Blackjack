#include "global.h"
#include "qlearning.h"
#include "training.h"
#include "files.h"

using namespace std;

//training variables
bool verbose = false;
long long episodes = 10000000;
long long progress_interval = static_cast<long long>(episodes / 10);

double discount_factor = 0.9;  

double initial_exploration_rate = 1.0;
double minimum_exploration_rate = 0.3;
double exploration_prob = initial_exploration_rate;

double initial_learning_rate = 0.8;
double learning_rate = initial_learning_rate;
double minimum_learning_rate = 0.1;

double E = 2.7182818284;

double learning_decay_rate = pow(E, (1.0 / episodes) * std::log(minimum_learning_rate / initial_learning_rate) );
double exploration_decay_rate = pow(E, (1.0 / episodes) * std::log(minimum_exploration_rate / initial_exploration_rate) );

int init_training_ep(Player &player, Dealer &dealer){
    if (verbose) {
        cout <<endl;
        cout <<"========= INITIALIZING GAME ==========="<<endl;
    }
    //not implemented yet
    double bet_amt = bet_strategy(player);
    player.bet(bet_amt);

    if (verbose) cout << "YOUR BET: " << player.get_bet_amt() << endl;
    
    player.deal();
    player.deal();

    if (verbose) cout << "YOUR HAND: ";
    player.show_hand();

    if (verbose) cout << "DEALER'S HAND: ";
    dealer.show_hand();

    if (verbose) {
        cout <<endl;
        cout <<"========== RUNNING GAME ==========="<<endl;
    }

    Node* trajectory = run_ep(player, dealer, false);

    cleanNodes(trajectory);

    int win_count =0;

    if (get_reward(player, dealer, player.get_action()) > 0) win_count ++;
   
    return win_count;

    dealer.reset();
    player.reset();
}

Node* run_ep(Player &player, Dealer &dealer, bool is_split) {
    bool split_occurred = false;
    Node* end_node = nullptr;
    Node* prev_node = nullptr;

    while (!end_node) {
        //create node for current state and action
        State state = update_and_return_state(player, dealer);
        Action act = get_action(player, dealer, true);
        Node* n = create_node(state, act);
        n->previous = prev_node;
        prev_node = n;

        if (verbose) player.show_hand();

        //gameplay
        if (act == Action::HIT) {
            player.hit();
        }
        else if (act == Action::STAND) {
            player.stand();
            end_node = n;
        }
        else if (act == Action::DOUBLE) {
            player.double_down();
            end_node = n;
        }
        else if (act == Action::SURRENDER) {
            player.surrender();
            end_node = n;
        }
        else if (act == Action::SPLIT && !is_split) {
            //only allowed if not already a split hand. Player will mark itself as split and will not consider splitting as a future action
            player.split_hand_1();
            //mark that a split has occurred
            split_occurred = true;
            //continue gameplay for hand 1
            if (verbose) cout << "\n========= HAND 1 =========\n";
        }

        //check end conditions
        int hand_val = player.get_hand_val();

        if (hand_val >= 21) end_node = n;

        if (end_node) {
            //if the player has not busted or surrendered, let dealer play
            if (hand_val <= 21 && player.get_action() != Action::SURRENDER) dealer.play();
            dealer.show_hand();
            end_node->reward = get_reward(player, dealer, act);
            break;
        }
    }

    //split handling
    if (split_occurred && !is_split) {
        if (verbose) cout << "\n========= HAND 2 =========\n";
        //update the second hand
        player.split_hand_2();

        //play hand 2
        Node* split_end_node = run_ep(player, dealer, true);

        // Q-values for hand 2
        if (verbose) cout << "\n========= HAND 2 GAME LOG =================\n";
        update_q_values(player, split_end_node, dealer, 0.0);

        // Combine rewards
        double split_reward = get_split_reward(split_end_node);

        if (verbose) {
            cout << "\n========= SPLIT HAND GAME LOG =================\n";
            cout << "Split Reward: " << split_reward << endl;
        }

        // Update Q-values for hand 1 using reward from hand 2
        update_q_values(player, end_node, dealer, split_reward);

        return end_node;
    }

    if (!split_occurred && !is_split) {
        if (verbose) cout << "\n========= GAME LOG =================\n";
        update_q_values(player, end_node, dealer, 0.0);
    }

    return end_node;
}


void train(){
    int win_count = 0;
    int split_win_count =0;
    int ties = 0;
    rng.seed(static_cast<unsigned int>
        ((std::chrono::steady_clock::now().time_since_epoch().count())));

    std::time_t currentTime = std::time(nullptr);
    char* dateTimeString = std::ctime(&currentTime);
    auto startTime = std::chrono::high_resolution_clock::now();

    cout <<"\nSIMULATION START TIME: " << dateTimeString<<endl;
    cout <<"SIMULATING "<<episodes<<" GAMES..."<<endl;

    fetch_states();
    fetch_table();
    
    for (long long i=0; i< episodes; i++){
        double initial_cash = 1000.0;

        Dealer dealer;
        Player player(initial_cash);
        win_count += init_training_ep(player, dealer);
      
        if (player.get_result() == RESULT::TIE) {ties++;}

        exploration_prob = initial_exploration_rate* pow(exploration_decay_rate, i);
        learning_rate = initial_learning_rate * pow(learning_decay_rate, i);

        if (i % progress_interval == 0 && i != 0) {
            if (!verbose && episodes > 10000) cout << "Progress: " << static_cast<long long>((i * 100) / episodes) << "%" << endl;
            //cout <<"LEARNING RATE: "<<learning_rate<<endl;
            cout <<"EXPLORATION RATE: "<<exploration_prob<<endl;
            cout <<"LEARNING RATE: "<<learning_rate<<endl;
        }
    }
    exploration_prob = initial_exploration_rate;
    learning_rate = initial_learning_rate;

    //SAVE TABLE AND STATES

    update_q_table();
    write_table_to_file();
    write_states_to_file();

    value_table.clear();


    //PRINT RESULTS
    cout <<"WIN RATE: "<< static_cast<double>(win_count) /static_cast<double>(episodes)<<endl;
    cout <<"TIE RATE: "<< static_cast<double>(ties)/static_cast<double>(episodes)<<endl;

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime-startTime);
    long long duration_seconds = duration.count();
    
    long long minutes = int(duration_seconds)/60;
    long long seconds = duration_seconds - minutes*60;
    long long hours = int(minutes)/60;

    cout <<"\nSIMULATION DURATION: " << hours<<":"<<minutes<<":"<< duration_seconds<<endl;
    cout <<"SIMULATION END TIME: " << std::ctime(&currentTime)<<endl;
}