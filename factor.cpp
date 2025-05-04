#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>
#include <numeric>
#include "InfInt.h"

// Shared variables for thread communication
std::atomic<bool> factors_found(false);
InfInt result_p, result_q;
std::mutex result_mutex;

// Corrected Fermat Factorization function for a thread
void fermat_factor_thread(InfInt number, InfInt start_a, unsigned int step, unsigned long long max_iterations) {
    InfInt a = start_a;
    unsigned long long iterations = 0;

    while (!factors_found.load() && (max_iterations == 0 || iterations < max_iterations)) {
        InfInt b_squared = a * a - number;

        // Optimization: Check last digit/hex digit for square possibility if needed (not implemented here)

        if (b_squared >= 0) { // Ensure b_squared is not negative
            InfInt b = b_squared.intSqrt();
            if (b * b == b_squared) { // Check if b_squared is a perfect square
                // Factors found
                InfInt p = a + b;
                InfInt q = a - b;

                // Atomically set the flag and check if this thread is the first
                bool already_found = factors_found.exchange(true);
                if (!already_found) {
                    std::lock_guard<std::mutex> lock(result_mutex);
                    result_p = p;
                    result_q = q;
                    // Optional: Print which thread found it
                    // std::cout << "Factors found by thread starting with a = " << start_a << ": " << p << ", " << q << std::endl;
                }
                return; // Exit the function as factors are found
            }
        }

        // Move to the next 'a' value for this thread
        a += step;
        iterations++;
    }
    // Optional: Indicate thread completion without finding factors within limits
    // if (!factors_found.load()) {
    //     std::cout << "Thread starting with a = " << start_a << " finished search limit." << std::endl;
    // }
}

// Main factorization function using threads
void factor(InfInt number) {
    // Reset shared variables for a new run
    factors_found = false;
    result_p = 0;
    result_q = 0;

    // Handle edge cases
    if (number <= 1) {
        std::cout << "Number must be greater than 1." << std::endl;
        return;
    }
    if (number % 2 == 0) {
        std::cout << "Number is even. Factors include 2." << std::endl;
        // Optionally, continue factoring number / 2
        // factor(number / 2);
        result_p = 2;
        result_q = number / 2;
        factors_found = true;
        // Print factors if needed, consistent with the later print statement
        // std::cout << result_p << "\n\n" << result_q << std::endl;
        return;
    }

    // Start Fermat factorization
    InfInt start_a = number.intSqrt();
    if (start_a * start_a < number) { // Ensure we start >= sqrt(N)
         start_a += 1;
    }
    // Check if N is a perfect square itself
    if (start_a * start_a == number) {
        std::cout << "Number is a perfect square." << std::endl;
        result_p = start_a;
        result_q = start_a;
        factors_found = true;
        return;
    }


    // Determine number of threads
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) {
        num_threads = 4; // Default to 4 threads if hardware concurrency is not available
    }
    std::cout << "Using " << num_threads << " threads." << std::endl;

    // Set a reasonable limit for iterations per thread to prevent infinite loops for non-suitable numbers
    // This limit might need adjustment based on expected number size and factor proximity
    unsigned long long max_iterations_per_thread = 100000000; // Example limit: 100 million iterations

    std::vector<std::thread> threads;
    std::cout << "Starting search from a = " << start_a << std::endl;

    // Launch threads
    for (unsigned int i = 0; i < num_threads; ++i) {
        InfInt thread_start_a = start_a + i;
        threads.push_back(std::thread(fermat_factor_thread, number, thread_start_a, num_threads, max_iterations_per_thread));
    }

    // Wait for all threads to complete (or until factors are found and threads exit)
    for (auto &th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    // Check if factors were found
    if (factors_found.load()) {
        std::cout << "Factors found:" << std::endl;
        std::cout << result_p << "\n\n" << result_q << std::endl;
    } else {
        std::cout << "Factors not found within the search limit." << std::endl;
        std::cout << "Fermat's method is efficient only when factors are close to sqrt(N)." << std::endl;
    }
}

int main() {
    InfInt myint1;

    std::cout << "Enter number to factor (must be odd and > 1): \n";
    std::cin >> myint1;

    factor(myint1);

    return 0;
}
