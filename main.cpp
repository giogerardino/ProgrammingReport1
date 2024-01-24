#include <iostream>
#include <vector>

#define LIMIT 10000000
#define DEFAULT_THREADS 1

/*
This function checks if an integer n is prime.

Parameters:
n : int - integer to check

Returns true if n is prime, and false otherwise.
*/
bool check_prime(const int &n);

int main() {
    int upper_bound = 0;
    int num_threads = DEFAULT_THREADS; // default
    
    std::<cout << "Enter upper bound of integers";
    std::cin >> upper_bound;
    
    std::<cout << "Enter the number of threads";
    std:: cin >> num_threads;
    if (num_threads <= 0) // error checking 
       num_threads = DEFAULT_THREADS; 
    
    std::vector<int> primes;
        
    for (int current_num = 2; current_num <= upper_bound; current_num++) {
        
        if (check_prime(current_num)) {
           primes.push_back(current_num);
        }
    }
    
    std::cout << primes.size() << " primes were found." << std::endl;
    
    return 0;
}

bool check_prime(const int &n) {
     for (int i = 2; i * i <= n; i++) {
         if (n % i == 0) {
            return false;
         }
     }
   return true; 
}
