#include <iostream>
#include <semaphore.h>
#include <thread>
#include <vector>
#include <unistd.h> // for sleep()
#include <mutex> // for std::mutex

using namespace std;

// Semaphores and shared variables
sem_t rw_mutex;    // Controls access to the critical section
sem_t read_mutex;  // Protects the read_count variable
sem_t entry_mutex; // Ensures starve-free access for readers and writers

int read_count = 0; // Keeps track of the number of readers
std::mutex coutMutex; // Mutex for thread-safe printing

// Thread-safe function for printing with color
enum Color { RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37 };

void atomicPrint(const string& message, Color color = WHITE) {
    lock_guard<mutex> lock(coutMutex);
    cout << "\033[" << color << "m" << message << "\033[0m" << endl;
}

void starveFreeReader(int processId) {
    // Wait on entry_mutex to ensure fairness
    sem_wait(&entry_mutex);

    // Acquire read_mutex to safely update read_count
    sem_wait(&read_mutex);
    ++read_count;
    if (read_count == 1) {
        // If this is the first reader, wait on rw_mutex
        sem_wait(&rw_mutex);
    }
    sem_post(&read_mutex);

    // Release entry_mutex for other readers or writers
    sem_post(&entry_mutex);

    // ***** CRITICAL SECTION ***** //
    atomicPrint("Reader " + to_string(processId) + " is reading.", GREEN);
    sleep(1); // Simulate reading operation

    // Acquire read_mutex to safely update read_count
    sem_wait(&read_mutex);
    --read_count;
    if (read_count == 0) {
        // If this is the last reader, release rw_mutex
        sem_post(&rw_mutex);
    }
    sem_post(&read_mutex);
}

void starveFreeWriter(int processId) {
    // Wait on entry_mutex to ensure fairness
    sem_wait(&entry_mutex);

    // Wait on rw_mutex to enter the critical section
    sem_wait(&rw_mutex);

    // Release entry_mutex for other readers or writers
    sem_post(&entry_mutex);

    // ***** CRITICAL SECTION ***** //
    atomicPrint("Writer " + to_string(processId) + " is writing.", BLUE);
    sleep(1); // Simulate writing operation

    // Release rw_mutex after writing
    sem_post(&rw_mutex);
}

int main() {
    // Initialize semaphores
    sem_init(&rw_mutex, 0, 1);
    sem_init(&read_mutex, 0, 1);
    sem_init(&entry_mutex, 0, 1);

    // Simulate readers and writers
    std::vector<std::thread> threads;

    // Create reader threads
    for (int i = 1; i <= 5; ++i) {
        threads.emplace_back(starveFreeReader, i);
    }

    // Create writer threads
    for (int i = 1; i <= 3; ++i) {
        threads.emplace_back(starveFreeWriter, i);
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    // Destroy semaphores
    sem_destroy(&rw_mutex);
    sem_destroy(&read_mutex);
    sem_destroy(&entry_mutex);

    return 0;
}