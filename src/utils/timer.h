#pragma once

#include <chrono>
#include <iostream>

namespace s21 {
  using clock = std::chrono::steady_clock;

class Timer {
 public:
  static void Start();
  static void Stop();
  static double GetElapsedTimeMs();

 private:
  static clock::time_point start_time_;
  static clock::time_point end_time_;
};

}  // namespace s21 