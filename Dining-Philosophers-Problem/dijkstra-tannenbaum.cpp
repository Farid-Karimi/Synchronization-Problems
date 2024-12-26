#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <string>
#include <atomic>

using namespace std;

const int N = 5;
const int THINKING = 2;
const int HUNGRY = 1;
const int EATING = 0;

vector<int> state(N, THINKING);
vector<int> philosophers = {0, 1, 2, 3, 4};

mutex mtx;
mutex coutMutex; // Mutex for synchronized console output
condition_variable cv[N];
atomic<bool> should_terminate(false); // Flag to signal threads to terminate

#define LEFT(phnum) ((phnum + N - 1) % N)
#define RIGHT(phnum) ((phnum + 1) % N)

// ANSI color codes
enum Color { RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37 };

void atomicPrint(const std::string& message, Color color = WHITE) {
    lock_guard<mutex> lock(coutMutex);
    cout << "\033[" << color << "m" << message << "\033[0m" << endl;
}

void test(int phnum) {
    if (state[phnum] == HUNGRY && state[LEFT(phnum)] != EATING && state[RIGHT(phnum)] != EATING) {
        state[phnum] = EATING;

        atomicPrint("Philosopher " + to_string(phnum + 1) + " takes fork " +
                    to_string(LEFT(phnum) + 1) + " and " + to_string(phnum + 1), GREEN);
        atomicPrint("Philosopher " + to_string(phnum + 1) + " is Eating", BLUE);

        cv[phnum].notify_one();
    }
}

void take_fork(int phnum) {
    unique_lock<mutex> lock(mtx);

    state[phnum] = HUNGRY;
    atomicPrint("Philosopher " + to_string(phnum + 1) + " is Hungry", RED);

    test(phnum);

    while (state[phnum] != EATING && !should_terminate) {
        cv[phnum].wait_for(lock, chrono::milliseconds(100));
    }
}

void put_fork(int phnum) {
    unique_lock<mutex> lock(mtx);

    state[phnum] = THINKING;
    atomicPrint("Philosopher " + to_string(phnum + 1) + " putting fork " +
                to_string(LEFT(phnum) + 1) + " and " + to_string(phnum + 1) + " down", MAGENTA);
    atomicPrint("Philosopher " + to_string(phnum + 1) + " is thinking", YELLOW);

    test(LEFT(phnum));
    test(RIGHT(phnum));
}

void philosopher(int phnum) {
    while (!should_terminate) {
        this_thread::sleep_for(chrono::seconds(1));
        take_fork(phnum);
        if (!should_terminate) {
            this_thread::sleep_for(chrono::seconds(1));
            put_fork(phnum);
        }
    }
}

int main() {
    const int simulation_duration = 10; // Simulation time in seconds
    vector<thread> threads(N);

    // Start threads
    for (int i = 0; i < N; ++i) {
        threads[i] = thread(philosopher, i);
    }

    // Sleep for simulation duration
    this_thread::sleep_for(chrono::seconds(simulation_duration));

    // Signal threads to terminate
    should_terminate = true;

    // Wait for all threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    atomicPrint("Simulation complete!", RED);

    return 0;
}