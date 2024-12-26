#include <array>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>
#include <condition_variable>

enum Color { RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37 };

std::mutex coutMutex;  // Add this at the global scope

void atomicPrint(const std::string& message, Color color = WHITE) {
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << "\033[" << color << "m" << message << "\033[0m" << std::endl;
}

std::mutex g_lockprint;
constexpr  int no_of_philosophers = 7;

class sync_channel{
    std::mutex mutex;
    std::condition_variable cv;

public:
    void wait(){
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock);
    }

    void notifyall(){
        std::unique_lock<std::mutex> lock(mutex);
        cv.notify_all();
    }
};

struct table_setup{
   std::atomic<bool> done{ false };
   sync_channel channel;
};

class fork{
   int id;
   int owner;
   bool dirty;
   std::mutex mutex;
   sync_channel channel;

public:
    fork(int const forkId, int const ownerId): id(forkId), owner(ownerId), dirty(true){}

    int getId() const { return id; }  // Add this getter

    void request(int const ownerId)
    {
        while (owner != ownerId)
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(mutex);
                dirty = false;
                owner = ownerId;
                
                // Print when fork is picked up
                std::string message = "Philosopher " + std::to_string(ownerId) + 
                                    " picked up fork " + std::to_string(id);
                atomicPrint(message, CYAN);
            }
            else
            {
                channel.wait();
            }
        }
    }

    void done_using()
    {
        // Print when fork is put down
        std::string message = "Philosopher " + std::to_string(owner) + 
                            " put down fork " + std::to_string(id);
        atomicPrint(message, MAGENTA);
        
        dirty = true;
        channel.notifyall();
    }
    std::mutex& getmutex() { return mutex; }
};

struct philosopher {

public:
   int id;
   std::string const name;
   table_setup& setup;
   fork& left_fork;
   fork& right_fork;
   std::thread lifethread;

    philosopher(int const id, std::string const & n, table_setup & s, fork & l, fork & r): id(id), name(n), setup(s), left_fork(l), right_fork(r), lifethread(&philosopher::dine, this){}

    ~philosopher(){
      lifethread.join();
    }

    void dine(){
        setup.channel.wait();

        do{
         think();
         eat();
        } while (!setup.done);
    }

    void print(std::string const & text){
        std::string formatted_message = std::string(name) + text;
        
        // Color coding different states
        if (text.find("thinking") != std::string::npos) {
            atomicPrint(formatted_message, BLUE);
        }
        else if (text.find("started eating") != std::string::npos) {
            atomicPrint(formatted_message, GREEN);
        }
        else if (text.find("finished eating") != std::string::npos) {
            atomicPrint(formatted_message, YELLOW);
        }
        else {
            atomicPrint(formatted_message, WHITE);
        }
    }

    void eat()
    {
        atomicPrint(name + " attempting to pick up forks...", YELLOW);
        
        left_fork.request(id);
        right_fork.request(id);

        std::lock(left_fork.getmutex(), right_fork.getmutex());

        std::lock_guard<std::mutex> left_lock(left_fork.getmutex(), std::adopt_lock);
        std::lock_guard<std::mutex> right_lock(right_fork.getmutex(), std::adopt_lock);

        std::string eating_msg = name + " started eating with forks " + 
                                std::to_string(left_fork.getId()) + " and " + 
                                std::to_string(right_fork.getId());
        atomicPrint(eating_msg, GREEN);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        atomicPrint(name + " finished eating.", YELLOW);

        left_fork.done_using();
        right_fork.done_using();
    }

    void think(){
        print(" is thinking ");
    }
};

class table {
public: 
    table_setup setup;

    std::array<fork, no_of_philosophers> forks
    {
        {
            { 1, 1 },
            { 2, 2 },
            { 3, 3 },
            { 4, 4 },
            { 5, 5 },
            { 6, 6 },
            { 7, 1 },
        }
    };

    std::array<philosopher, no_of_philosophers> philosophers
    {
        {
            { 1, "1", setup, forks[0], forks[1] },
            { 2, "2", setup, forks[1], forks[2] },
            { 3, "3", setup, forks[2], forks[3] },
            { 4, "4", setup, forks[3], forks[4] },
            { 5, "5", setup, forks[4], forks[5] },
            { 6, "6", setup, forks[5], forks[6] },
            { 7, "7", setup, forks[6], forks[0] },
        }
    };

    void start(){
        setup.channel.notifyall();
    }

    void stop(){
        setup.done = true;
    }
};

void dine(){
    atomicPrint("Dinner started!", MAGENTA);

    {
        table table;

        table.start();
        std::this_thread::sleep_for(std::chrono::seconds(60));
        table.stop();
    }

    atomicPrint("Dinner done!", MAGENTA);
}

int main(){  
   dine();

   return 0;
}