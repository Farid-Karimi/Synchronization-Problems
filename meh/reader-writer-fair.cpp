#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <random>

std::mutex resource_mutex;          // Mutex for protecting shared resource access
std::mutex reader_count_mutex;      // Mutex for protecting reader count variable
std::mutex cout_mutex;              // Mutex for thread-safe printing
std::condition_variable cv;         // Condition variable for thread synchronization

int reader_count = 0;               // Global variable to keep track of reader count
bool writer_active = false;         // Flag to indicate if a writer is active
int shared_memory = 0;              // Shared integer memory

int random(int min, int max) {
    // Ensure min is less than or equal to max
    if (min > max) {
        std::swap(min, max);
    }
    
    // Random number generator setup
    std::random_device rd;  // Seed for the random number engine
    std::mt19937 gen(rd()); // Mersenne Twister random number engine
    std::uniform_int_distribution<> dis(min, max); // Uniform distribution

    return dis(gen);
}

// Thread-safe function for printing with color
enum Color { RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37 };

void atomicPrint(const std::string& message, Color color = WHITE) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "\033[" << color << "m" << message << " | Shared Memory: " << shared_memory << "\033[0m" << std::endl;
}

void read(int reader_id) {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(reader_count_mutex);
            cv.wait(lock, [] { return !writer_active; }); // Wait if a writer is active
            reader_count++;
        }

        resource_mutex.lock(); // Lock resource for individual reading
        atomicPrint("Reader " + std::to_string(reader_id) + " is reading.", GREEN);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        atomicPrint("Reader " + std::to_string(reader_id) + " has finished reading.", CYAN);
        resource_mutex.unlock(); // Unlock resource after reading

        {
            std::unique_lock<std::mutex> lock(reader_count_mutex);
            reader_count--;
            if (reader_count == 0) {
                cv.notify_all(); // Notify waiting writers
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(random(200, 500))); // Pause before next read
    }
}

void write(int writer_id) {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(reader_count_mutex);
            cv.wait(lock, [] { return !writer_active && reader_count == 0; }); // Wait if readers are active or a writer is active
            writer_active = true;
        }

        resource_mutex.lock(); // Lock resource for writing
        shared_memory += 1; // Update shared memory
        atomicPrint("Writer " + std::to_string(writer_id) + " is writing.", RED);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        atomicPrint("Writer " + std::to_string(writer_id) + " has finished writing.", MAGENTA);
        resource_mutex.unlock(); // Unlock resource after writing

        {
            std::unique_lock<std::mutex> lock(reader_count_mutex);
            writer_active = false;
            cv.notify_all(); // Notify waiting readers or writers
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(random(100, 300))); // Pause before next write
    }
}

int main() {
    const int simulation_duration = 10; // Simulation time in seconds
    const auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> readers;
    std::vector<std::thread> writers;

    for (int i = 1; i <= 5; ++i) {
        writers.push_back(std::thread(write, i));
        readers.push_back(std::thread(read, i));
    }

    // Run simulation for the specified duration
    while (std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::steady_clock::now() - start_time)
               .count() < simulation_duration) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Stop threads after simulation ends
    for (auto& t : readers) t.detach();
    for (auto& t : writers) t.detach();

    atomicPrint("Simulation complete!", YELLOW);

    return 0;
}
