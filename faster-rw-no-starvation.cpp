#include <iostream>
#include <semaphore.h>
#include <thread>
#include <vector>
#include <unistd.h> // for sleep()
#include <mutex> // for std::mutex

using namespace std;

// Semaphores and shared variables
sem_t rw_mutex;    // Controls access to the critical section
sem_t entry_mutex; // Ensures fairness for readers and writers
sem_t out_mutex;   // Controls out_count access

int in_count = 0;  // Tracks number of readers currently in the critical section
int out_count = 0; // Tracks number of readers that have left the critical section
bool writer_waiting = false; // Indicates if a writer is waiting

std::mutex coutMutex; // Mutex for thread-safe printing

// Thread-safe function for printing with color
enum Color { RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37 };

void atomicPrint(const string& message, Color color = WHITE) {
    lock_guard<mutex> lock(coutMutex);
    cout << "\033[" << color << "m" << message << "\033[0m" << endl;
}

void fasterReader(int processId) {
    // Wait on entry_mutex to ensure fairness and update in_count
    sem_wait(&entry_mutex);
    ++in_count;
    sem_post(&entry_mutex);

    // ***** CRITICAL SECTION ***** //
    atomicPrint("Reader " + to_string(processId) + " is reading.", GREEN);
    sleep(1); // Simulate reading operation

    // Update out_count and handle writer waiting
    sem_wait(&out_mutex);
    ++out_count;
    if (writer_waiting && in_count == out_count) {
        sem_post(&rw_mutex);
    }
    sem_post(&out_mutex);
}

void fasterWriter(int processId) {
    // Wait on entry_mutex to ensure fairness
    sem_wait(&entry_mutex);

    sem_wait(&out_mutex);
    if (in_count == out_count) {
        // No readers are in the critical section
        sem_post(&out_mutex);
    } else {
        // Readers are in the critical section, writer must wait
        writer_waiting = true;
        sem_post(&out_mutex);
        sem_wait(&rw_mutex);
        writer_waiting = false;
    }

    // ***** CRITICAL SECTION ***** //
    atomicPrint("Writer " + to_string(processId) + " is writing.", BLUE);
    sleep(1); // Simulate writing operation

    sem_post(&entry_mutex);
}

int main() {
    // Initialize semaphores
    sem_init(&rw_mutex, 0, 0);
    sem_init(&entry_mutex, 0, 1);
    sem_init(&out_mutex, 0, 1);

    // Simulate readers and writers
    std::vector<std::thread> threads;

    // Create reader threads
    for (int i = 1; i <= 5; ++i) {
        threads.emplace_back(fasterReader, i);
    }

    // Create writer threads
    for (int i = 1; i <= 3; ++i) {
        threads.emplace_back(fasterWriter, i);
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    // Destroy semaphores
    sem_destroy(&rw_mutex);
    sem_destroy(&entry_mutex);
    sem_destroy(&out_mutex);

    return 0;
}
