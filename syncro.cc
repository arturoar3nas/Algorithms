#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <ctime>   
#include <iomanip>  

std::mutex m, m2, m3;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;

void worker_thread_1_impl()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "Worker thread 1 is processing..\n";
}

void worker_thread()
{
    // Wait until main() sends data
    for(auto i = 0; i < 10; ++i) {
      std::unique_lock<std::mutex> lk(m);
      cv.wait(lk, []{return ready;});
  
      // after the wait, we own the lock.
      std::cout << "Worker thread 1 is processing data\n";
      data += " after processing";

      
      worker_thread_1_impl();
  
      // Send data back to main()
      processed = true;
      std::cout << "Worker thread 1 signals data processing completed\n";
  
      // Manual unlocking is done before notifying, to avoid waking up
      // the waiting thread only to block again (see notify_one for details)
      lk.unlock();
      cv.notify_one();
    }
}

void worker_thread_2_impl()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "Worker thread 2 is processing..\n";
}


void worker_thread_2()
{
    // Wait until main() sends data
    for(auto i = 0; i < 10; ++i) {
      std::unique_lock<std::mutex> lk(m2);
      cv.wait(lk, []{return ready;});
  
      // after the wait, we own the lock.
      std::cout << "Worker thread 2 is processing data\n";
      data += " after processing";

      worker_thread_2_impl();
  
      // Send data back to main()
      processed = true;
      std::cout << "Worker thread 2 signals data processing completed\n";
  
      // Manual unlocking is done before notifying, to avoid waking up
      // the waiting thread only to block again (see notify_one for details)
      lk.unlock();
      cv.notify_one();
    }
}

void worker_thread_3_impl()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "Worker thread 3 is processing..\n";
}

void worker_thread_3()
{   
  for(auto i = 0; i < 10; ++i) {
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m3);
    cv.wait(lk, []{return ready;});
 
    // after the wait, we own the lock.
    std::cout << "Worker thread 3 is processing data\n";
    data += " after processing";

    worker_thread_3_impl();
 
    // Send data back to main()
    processed = true;
    std::cout << "Worker thread 3 signals data processing completed\n";
 
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
  }
}

 
int main()
{
    std::thread worker(worker_thread);
    std::thread worker_2(worker_thread_2);
    std::thread worker_3(worker_thread_3);
 
    data = "Example data";
    // send data to the worker thread
    {   std::cout << "main() waiting before send data\n";
        std::cout<<"Current Time :: ";

        auto current_time_point = [](std::chrono::system_clock::time_point timePt){
          time_t timeStamp = std::chrono::system_clock::to_time_t(timePt);
          std::cout << std::ctime(&timeStamp) << std::endl;
        };


        current_time_point(std::chrono::system_clock::now());
      
        std::chrono::system_clock::time_point timePt;
        std::string strunits = "hours";
        int ntime = 2;
        
        if(strunits.compare("seconds") == 0)
        timePt = std::chrono::system_clock::now() + std::chrono::seconds(ntime);
        else if(strunits.compare("minutes") == 0)
        timePt = std::chrono::system_clock::now() + std::chrono::minutes(ntime);
        else if(strunits.compare("hours") == 0)
        timePt = std::chrono::system_clock::now() + std::chrono::hours(ntime);
        else
          throw std::logic_error("Function name already exist: . "
                                "Please use unique function names");        
      
        std::cout << "Sleeping Until :: "; 
        current_time_point(timePt);
      
        std::this_thread::sleep_until(timePt);

        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_all();
 
    // wait for the worker
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return processed;});
    }
    std::cout << "Back in main(), data = " << data << '\n';
 
    worker.join();
    worker_2.join();
    worker_3.join();
}
