/*
 * stress_test.cpp
 * Author: Yuki Furuta <furushchev@jsk.imi.i.u-tokyo.ac.jp>
 */

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>


struct Worker {
  std::mutex mutex_;
  std::condition_variable cv_;
  bool shutdown_;
  std::unique_ptr<std::thread> thread_;

  Worker() : shutdown_(false){}

  int tarai(int x, int y, int z) {
    if (x <= y) return y;
    else tarai(tarai(x-1, y, z), tarai(y-1, z, x), tarai(z-1, x, y));
  }

  void run() {
    std::cout << "[Thread " << std::this_thread::get_id() << "] spawned" << std::endl;
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]{
      tarai(12, 6, 0);
      return this->shutdown_;
    });
    std::cout << "[Thread " << std::this_thread::get_id() << "] shutdown" << std::endl;
    lock.unlock();
  }

  void start() {
    thread_ = std::unique_ptr<std::thread>(new std::thread(&Worker::run, this));
  }

  void shutdown() {
    mutex_.lock();
    shutdown_ = true;
    cv_.notify_one();
    mutex_.unlock();

    thread_->join();
  }
};

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Arguments: [worker_num] [duration (seconds)]" << std::endl;
    return 1;
  }
  int worker_num = std::atoi(argv[1]);
  int duration = std::atoi(argv[2]);
  std::vector<std::unique_ptr<Worker>> workers;
  std::cout << "Initializing " << worker_num << " workers" << std::endl;
  for (int i = 0; i < worker_num; ++i) {
    std::unique_ptr<Worker> worker(new Worker());
    worker->start();
    workers.push_back(std::move(worker));
  }

  std::cout << "Loading " << duration << " seconds" << std::flush;
  for (int i = 0; i < duration; ++i) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "." << std::flush;
  }
  std::cout << "Done" << std::endl;

  for ( int i = 0; i < worker_num; ++i) {
    workers[i]->shutdown();
  }
  return 0;
}
