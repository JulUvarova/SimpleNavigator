#pragma once

#include <chrono>
#include <iostream>

namespace s21 {
  using clock = std::chrono::steady_clock;

class Timer {
 public:
  static void Start();
  static void Stop();

 private:
  static clock::time_point start_time_;
};

}  // namespace s21 