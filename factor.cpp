#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <thread>
#include "InfInt.h"

using namespace std;

void fermat_foctor(int i, InfInt number, InfInt sqrt_number){
    string midpoint_start = to_string(i);
    int zeros_to_append = sqrt_number.toString().length() - 5;
    for(int i = 0; i <= zeros_to_append; i++){
        midpoint_start+="0";
    }
    //Fermat integer factorization
    InfInt midpoint = midpoint_start;
    midpoint *= midpoint;
    InfInt other_number = (midpoint - number).intSqrt();

    for(int i = 0;true;i++){
        InfInt next_number = other_number + i;
        InfInt next_midpoint = (next_number*next_number + number).intSqrt();
        InfInt p = next_midpoint + next_number;
        InfInt q = next_midpoint - next_number;
        if(p * q == number){
            cout << p << "\n\n" << q;
            break;
        }
    }
}

void factor(InfInt number){
    string target = number.toString().substr(0,5);
    int int_target = stoi(target);
    InfInt sqrt_number = number.intSqrt();
    int target_sqrt_number = stoi(sqrt_number.toString().substr(0,4));
    vector<int> midpoints;
    set<int> midpoint_set;

    for(int p=1001;p<10000;p++){
        for(int q=9999;q>1000;q--){
            if(to_string(p*q).find(target) == 0){
                midpoints.push_back((int)((p+q)/2));
            }
        }
    }
    for(int i : midpoints){
        if(i < int_target&& i > target_sqrt_number){
            midpoint_set.insert(i);
        }
    }

    vector<thread> threads;

    for(int i: midpoint_set){
        threads.push_back(thread(fermat_foctor, i, number, sqrt_number));        
    }

    for (auto &thread: threads){
        if(thread.joinable()){
            thread.join();
            break;
        }
    }
}

int main() {
    InfInt myint1;

    cout << "Enter number: \n"; 
    cin >> myint1;
    factor(myint1);
    return 0;
}