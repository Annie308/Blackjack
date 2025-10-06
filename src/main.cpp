#include "global.h"
#include "game.h"

std::mt19937 rng;

double initial_cash = 1000.0;
bool verbose = false;
int win_count = 0;
int episodes = 1000;
int progress_interval = episodes / 100;

int main(){
    rng.seed(static_cast<unsigned int>
        ((std::chrono::steady_clock::now().time_since_epoch().count())));

    std::time_t currentTime = std::time(nullptr);
    char* dateTimeString = std::ctime(&currentTime);
    auto startTime = std::chrono::high_resolution_clock::now();

    cout <<"\nSIMULATION START TIME: " << dateTimeString<<endl;
    cout <<"SIMULATING "<<episodes<<" GAMES..."<<endl;

    Player player(initial_cash);

    fetch_states();
    fetch_table();
    
    for (int i=0; i< episodes; i++){
 
        Dealer dealer;
        game(player, dealer);
      
        if (player.get_cash() > initial_cash) {win_count++;}

        if (i % progress_interval == 0 && i != 0) {
            if (!verbose) cout << "Progress: " << (i * 100) / episodes << "%" << endl;
        }

        if (player.get_cash() <= 0) {
            player = Player(initial_cash);
        }
    }

    write_states_to_file();
    write_table_to_file();


    //PRINT RESULTS

    cout <<"WIN RATE: "<< static_cast<double>(win_count)/static_cast<double>(episodes)<<endl;
    cout <<"FINAL CASH: "<< player.get_cash()<<endl;

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime-startTime);
    long long duration_seconds = duration.count();
    
    long long minutes = int(duration_seconds)/60;
    long long seconds = duration_seconds - minutes*60;
    long long hours = int(minutes)/60;

    cout <<"\nSIMULATION DURATION: " << hours<<":"<<minutes<<":"<< duration_seconds<<endl;
    cout <<"SIMULATION END TIME: " << std::ctime(&currentTime)<<endl;

    return 0;
}