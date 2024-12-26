#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <vector>
#include <mutex>
#include <cstdint> // For intptr_t

using namespace std;

// Thread-safe function for printing with color
enum Color { RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37 };
mutex coutMutex;

void atomicPrint(const string &message, Color color = WHITE) {
    lock_guard<mutex> lock(coutMutex);
    cout << "\033[" << color << "m" << message << "\033[0m" << endl;
}

// Global variables for shared resources
int shared_var = 10, r_counter = 0;
sem_t rcnt;  // Semaphore for updating r_counter
sem_t rdwr;  // Semaphore to manage readers/writers access
sem_t addt;  // Semaphore to avoid reader starvation

// Reader thread function
void *reader(void *id) {
    int thread_id = static_cast<int>(reinterpret_cast<intptr_t>(id));

    // ENTRY_SECTION
    sem_wait(&addt);
    atomicPrint("Reader-" + to_string(thread_id) + " is in the ENTRY_SECTION", YELLOW);
    sem_wait(&rcnt);
    r_counter++;
    if (r_counter == 1) sem_wait(&rdwr);  // First reader blocks writers
    sem_post(&rcnt);
    sem_post(&addt);

    // CRITICAL_SECTION
    atomicPrint("Reader-" + to_string(thread_id) + " is reading the value = " + to_string(shared_var), CYAN);

    // EXIT_SECTION
    atomicPrint("Reader-" + to_string(thread_id) + " is in the EXIT_SECTION", YELLOW);
    sem_wait(&rcnt);
    r_counter--;
    if (r_counter == 0) sem_post(&rdwr);  // Last reader allows writers
    sem_post(&rcnt);

    return nullptr;
}

// Writer thread function
void *writer(void *id) {
    int thread_id = static_cast<int>(reinterpret_cast<intptr_t>(id));

    // ENTRY_SECTION
    sem_wait(&addt);
    atomicPrint("Writer-" + to_string(thread_id) + " is in the ENTRY_SECTION", MAGENTA);
    sem_wait(&rdwr);

    // CRITICAL_SECTION
    shared_var += 15;
    atomicPrint("Writer-" + to_string(thread_id) + " is writing value = " + to_string(shared_var), GREEN);

    // EXIT_SECTION
    atomicPrint("Writer-" + to_string(thread_id) + " is in the EXIT_SECTION", MAGENTA);
    sem_post(&rdwr);
    sem_post(&addt);

    return nullptr;
}

int main() {
    // Initialize semaphores
    sem_init(&rcnt, 0, 1);
    sem_init(&rdwr, 0, 1);
    sem_init(&addt, 0, 1);

    // Create reader and writer threads
    vector<pthread_t> readers(20), writers(5);

    for (size_t i = 0; i < readers.size(); ++i) {
        pthread_create(&readers[i], nullptr, reader, reinterpret_cast<void *>(static_cast<intptr_t>(i)));
    }
    for (size_t i = 0; i < writers.size(); ++i) {
        pthread_create(&writers[i], nullptr, writer, reinterpret_cast<void *>(static_cast<intptr_t>(i)));
    }

    // Wait for threads to finish
    for (auto &reader : readers) {
        pthread_join(reader, nullptr);
    }
    for (auto &writer : writers) {
        pthread_join(writer, nullptr);
    }

    // Clean up semaphores
    sem_destroy(&rcnt);
    sem_destroy(&rdwr);
    sem_destroy(&addt);

    return 0;
}
