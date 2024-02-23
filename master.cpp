#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define LIMIT 10000000

std::mutex prime_mutex;

void handle_client(int client_socket, int start, int end, std::vector<int> &primes) {
    for (int n = start; n <= end; n++) {
        bool is_prime = true;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            std::lock_guard<std::mutex> guard(prime_mutex);
            primes.push_back(n);
        }
    }

    // Send result back to client
    send(client_socket, primes.data(), primes.size() * sizeof(int), 0);
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int upperBound, numThreads;
    std::cout << "Enter the upper bound for checking primes (upper bound is " << LIMIT << "): ";
    while (!(std::cin >> upperBound) || upperBound < 2 || upperBound > LIMIT) {
        std::cout << "Invalid input. Please enter an integer greater than 1 and less than " << LIMIT << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "Enter the number of threads (1 or more): ";
    while (!(std::cin >> numThreads) || numThreads < 1) {
        std::cout << "Invalid input. Please enter an integer that is 1 or more: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<int> primes;
    std::vector<std::thread> threads;

    int chunk = upperBound / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunk + (i == 0 ? 2 : 1);
        int end = (i == numThreads - 1) ? upperBound : (i + 1) * chunk;

        // Create a new socket for each client
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        threads.push_back(std::thread(handle_client, new_socket, start, end, std::ref(primes)));
    }

    for (auto &th : threads) {
        th.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << primes.size() << " primes were found." << std::endl;
    std::cout << "Time taken: " << duration << " milliseconds." << std::endl;

    return 0;
}
