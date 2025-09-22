#include "game.h"
#include "global.h"
#include "simulation.h"

using namespace std;

double learning_rate = 0.8;     //how much to change old Q-values
double discount_factor = 0.95;  //immediate rewards vs future rewards
double exploration_prob = 0.2;  //1-learning rate
double epochs = 1000;           //how many times to run

map<State, ACTION> Q_TABLE;

Node* create_node(State state, ACTION action){
    Node* new_node = new Node();
    new_node ->state = state;
    new_node ->action = action;
    new_node ->reward = 0;
    new_node -> prob = 0;      
    return new_node;
}
/*
    row = states
    col = actions
    {state, action}        {hand_val, dealer_hand_val, proportion_of_hand_bet}
    ex: {1,2}    ===>      {15, 5, 0.1}, {Hit} = 0.4
*/

