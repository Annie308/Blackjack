#include "global.h"
#include "training.h"
#include "q_table_validation.h"

std::mt19937 rng;

int main(){
    int loop = 1;

    for (int i=0; i<loop; i++){
        train();
        validate_table();
    }
    return 0;
}