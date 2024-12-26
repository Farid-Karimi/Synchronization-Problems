#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <random>

using namespace std;

class DiningPhilosophers {
private:
    struct ChopstickPair {
        int first, second;
        ChopstickPair(int f, int s) : first(f), second(s) {}
    };

    const int numPhilosophers = 7;
    vector<bool> chopsticks;
    queue<int> waitingPhilosophers;
    mutex mtx;
    condition_variable cv;
    vector<thread> threads;
    bool running = true;

    // Mutex for atomic console output
    mutex coutMutex;

    // Random number generator for eating time
    random_device rd;
    mt19937 gen{rd()};
    uniform_int_distribution<> eatTime{1000, 3000};

    // Thread-safe function for printing with color
    enum Color { RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37 };

    void atomicPrint(const string& message, Color color = WHITE) {
        lock_guard<mutex> lock(coutMutex);
        cout << "\033[" << color << "m" << message << "\033[0m" << endl;
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
        chopsticks[rightChopstick] = true;
    }

    void philosopherBehavior(int id) {
        while (running) {
            {
                unique_lock<mutex> lock(mtx);
                
                // Wait until it's this philosopher's turn and they can take chopsticks
                while (waitingPhilosophers.front() != id || !canTakeChopsticks(id)) {
                    cv.wait(lock);
                    if (!running) return;
                }

                // Take chopsticks and remove philosopher from waiting queue
                atomicPrint("Philosopher " + to_string(id) + " takes chopsticks " 
                            + to_string(id) + " and " 
                            + to_string((id + 1) % numPhilosophers), GREEN);
                takeChopsticks(id);
                waitingPhilosophers.pop();
            }

            // Eating
            atomicPrint("Philosopher " + to_string(id) + " is eating", BLUE);
            this_thread::sleep_for(chrono::milliseconds(eatTime(gen)));

            {
                unique_lock<mutex> lock(mtx);
                
                // Return chopsticks and go back to waiting queue
                atomicPrint("Philosopher " + to_string(id) + " returns chopsticks", YELLOW);
                returnChopsticks(id);
                waitingPhilosophers.push(id);
                
                // Notify other philosophers that chopsticks are available
                cv.notify_all();
            }

            // Thinking
            atomicPrint("Philosopher " + to_string(id) + " is thinking", MAGENTA);
            this_thread::sleep_for(chrono::milliseconds(eatTime(gen)));
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
            unique_lock<mutex> lock(mtx);
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
    this_thread::sleep_for(chrono::seconds(30));
    
    dp.stop();
    cout << "\033[32mSimulation complete!\033[0m" << endl;
    return 0;
}
