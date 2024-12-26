#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <random>

class DiningPhilosophers {
private:
    struct ChopstickPair {
        int first, second;
        ChopstickPair(int f, int s) : first(f), second(s) {}
    };

    const int numPhilosophers = 7;
    std::vector<bool> chopsticks;
    std::queue<int> waitingPhilosophers;
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    bool running = true;

    // Mutex for atomic console output
    std::mutex coutMutex;

    // Random number generator for eating time
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> eatTime{1000, 3000};

    // Thread-safe function for printing
    void atomicPrint(const std::string& message) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << message << std::endl;
    }

public:
    DiningPhilosophers() : chopsticks(numPhilosophers, true) {
        // Initially all philosophers are waiting
        for (int i = 0; i < numPhilosophers; i++) {
            waitingPhilosophers.push(i);
        }
    }

    bool canTakeChopsticks(int philosopherId) {
        int leftChopstick = philosopherId;
        int rightChopstick = (philosopherId + 1) % numPhilosophers;
        return chopsticks[leftChopstick] && chopsticks[rightChopstick];
    }

    void takeChopsticks(int philosopherId) {
        int leftChopstick = philosopherId;
        int rightChopstick = (philosopherId + 1) % numPhilosophers;
        chopsticks[leftChopstick] = false;
        chopsticks[rightChopstick] = false;
    }

    void returnChopsticks(int philosopherId) {
        int leftChopstick = philosopherId;
        int rightChopstick = (philosopherId + 1) % numPhilosophers;
        chopsticks[leftChopstick] = true;
        chopsticks[rightChopstick = true;
    }

    void philosopherBehavior(int id) {
        while (running) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                
                // Wait until it's this philosopher's turn and they can take chopsticks
                while (waitingPhilosophers.front() != id || !canTakeChopsticks(id)) {
                    cv.wait(lock);
                    if (!running) return;
                }

                // Take chopsticks and remove philosopher from waiting queue
                atomicPrint("Philosopher " + std::to_string(id) + " takes chopsticks " 
                            + std::to_string(id) + " and " 
                            + std::to_string((id + 1) % numPhilosophers));
                takeChopsticks(id);
                waitingPhilosophers.pop();
            }

            // Eating
            atomicPrint("Philosopher " + std::to_string(id) + " is eating");
            std::this_thread::sleep_for(std::chrono::milliseconds(eatTime(gen)));

            {
                std::unique_lock<std::mutex> lock(mtx);
                
                // Return chopsticks and go back to waiting queue
                atomicPrint("Philosopher " + std::to_string(id) + " returns chopsticks");
                returnChopsticks(id);
                waitingPhilosophers.push(id);
                
                // Notify other philosophers that chopsticks are available
                cv.notify_all();
            }

            // Thinking
            atomicPrint("Philosopher " + std::to_string(id) + " is thinking");
            std::this_thread::sleep_for(std::chrono::milliseconds(eatTime(gen)));
        }
    }

    void start() {
        // Create threads for all philosophers
        for (int i = 0; i < numPhilosophers; i++) {
            threads.emplace_back(&DiningPhilosophers::philosopherBehavior, this, i);
        }
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            running = false;
        }
        cv.notify_all();
        for (auto& thread : threads) {
            thread.join();
        }
    }
};

int main() {
    DiningPhilosophers dp;
    dp.start();

    // Let the simulation run for 30 seconds
    std::this_thread::sleep_for(std::chrono::seconds(30));
    
    dp.stop();
    std::cout << "Simulation ended" << std::endl;
    return 0;
}
