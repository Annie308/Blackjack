#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <optional>


inline std::map<char, int> card_vals = {
    {'A', 11}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, 
    {'8', 8}, {'9', 9}, {'10',10}, {'J', 10}, {'Q', 10}, {'K', 10}
};

inline std::vector<std::string> cards = {
    "AD","2D","3D","4D","5D","6D","7D","8D","9D","10D","JD","QD","KD",
    "AS","2S","3S","4S","5S","6S","7S","8S","9S","10S","JS","QS","KS",
    "AH","2H","3H","4H","5H","6H","7H","8H","9H","10H","JH","QH","KH",
    "AC","2C","3C","4C","5C","6C","7C","8C","9C","10C","JC","QC","KC"
};
