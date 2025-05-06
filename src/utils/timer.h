#pragma once

#include <chrono>
#include <iostream>

namespace s21 {
using clock = std::chrono::steady_clock;

class Timer {
 public:
  static void Start() {
    start_time_ = clock::now();
    std::cout << "Timer started." << std::endl;
  }

  static void Stop() {
    end_time_ = clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time_ - start_time_);
    std::cout << "Timer stopped. Elapsed time: " << elapsed.count() << " ms"
              << std::endl;
  }

  static double GetElapsedTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ -
                                                                 start_time_)
        .count();
  }

 private:
  // Initialize static members with inline to avoid multiple definition errors
  inline static clock::time_point start_time_ = clock::now();
  inline static clock::time_point end_time_ = clock::now();
};

}  // namespace s21