#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <string>

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
    atomicPrint("Philosopher " + to_string(phnum + 1) + " is Hungry", YELLOW);

    test(phnum);

    while (state[phnum] != EATING) {
        cv[phnum].wait(lock);
    }
}

void put_fork(int phnum) {
    unique_lock<mutex> lock(mtx);

    state[phnum] = THINKING;
    atomicPrint("Philosopher " + to_string(phnum + 1) + " putting fork " +
                to_string(LEFT(phnum) + 1) + " and " + to_string(phnum + 1) + " down", MAGENTA);
    atomicPrint("Philosopher " + to_string(phnum + 1) + " is thinking", CYAN);

    test(LEFT(phnum));
    test(RIGHT(phnum));
}

void philosopher(int phnum) {
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
        take_fork(phnum);
        this_thread::sleep_for(chrono::seconds(1));
        put_fork(phnum);
    }
}

int main() {
    vector<thread> threads(N);

    for (int i = 0; i < N; ++i) {
        atomicPrint("Philosopher " + to_string(i + 1) + " is thinking", RED);
        threads[i] = thread(philosopher, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
