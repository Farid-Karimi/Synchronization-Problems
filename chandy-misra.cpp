#include <array>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>
#include <condition_variable>

std::mutex g_lockprint;
constexpr  int no_of_philosophers = 7;

class sync_channel
{
   std::mutex mutex;
   std::condition_variable cv;

public:
   void wait()
   {
      std::unique_lock<std::mutex> lock(mutex);
      cv.wait(lock);
   }

   void notifyall()
   {
      std::unique_lock<std::mutex> lock(mutex);
      cv.notify_all();
   }
};

struct table_setup
{
   std::atomic<bool> done{ false };
   sync_channel      channel;
};

class fork
{
   int            id;
   int            owner;
   bool           dirty;
   std::mutex     mutex;
   sync_channel   channel;

public:
   fork(int const forkId, int const ownerId):
      id(forkId), owner(ownerId), dirty(true)
   {}

   void request(int const ownerId)
   {
      while (owner != ownerId)
      {
         if (dirty)
         {
            std::lock_guard<std::mutex> lock(mutex);

            dirty = false;
            owner = ownerId;
         }
         else
         {
            channel.wait();
         }
      }
   }

   void done_using()
   {
      dirty = true;
      channel.notifyall();
   }

   std::mutex& getmutex() { return mutex; }
};

struct philosopher
{
private:
   int               id;
   std::string const name;
   table_setup&      setup;
   fork&             left_fork;
   fork&             right_fork;
   std::thread       lifethread;
public:
   philosopher(int const id, std::string const & n, table_setup & s, fork & l, fork & r) :
      id(id), name(n), setup(s), left_fork(l), right_fork(r), lifethread(&philosopher::dine, this)
   {
   }

   ~philosopher()
   {
      lifethread.join();
   }

   void dine()
   {
      setup.channel.wait();

      do
      {
         think();
         eat();
      } while (!setup.done);
   }

   void print(std::string const & text)
   {
      std::lock_guard<std::mutex> cout_lock(g_lockprint);
      std::cout
         << std::left << std::setw(10) << std::setfill(' ')
         << name << text << std::endl;
   }

   void eat()
   {
      left_fork.request(id);
      right_fork.request(id);

      std::lock(left_fork.getmutex(), right_fork.getmutex());

      std::lock_guard<std::mutex> left_lock(left_fork.getmutex(), std::adopt_lock);
      std::lock_guard<std::mutex> right_lock(right_fork.getmutex(), std::adopt_lock);

      print(" started eating.");
      print(" finished eating.");

      left_fork.done_using();
      right_fork.done_using();
   }

   void think()
   {
      print(" is thinking ");
   }
};

class table
{
   table_setup    setup;

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
         { 1, "Aristotle", setup, forks[0], forks[1] },
         { 2, "Platon",    setup, forks[1], forks[2] },
         { 3, "Descartes", setup, forks[2], forks[3] },
         { 4, "Kant",      setup, forks[3], forks[4] },
         { 5, "Nietzsche", setup, forks[4], forks[5] },
         { 6, "Hume",      setup, forks[5], forks[6] },
         { 7, "Russell",   setup, forks[6], forks[0] },
      }
   };

public:
   void start()
   {
      setup.channel.notifyall();
   }

   void stop()
   {
      setup.done = true;
   }
};

void dine()
{
   std::cout << "Dinner started!" << std::endl;

   {
      table table;

      table.start();
      std::this_thread::sleep_for(std::chrono::seconds(60));
      table.stop();
   }

   std::cout << "Dinner done!" << std::endl;
}

int main()
{  
   dine();

   return 0;
}