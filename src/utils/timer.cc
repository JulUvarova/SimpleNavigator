#include "timer.h"

namespace s21 {

// Initialize static members
clock::time_point Timer::start_time_ = clock::now();
clock::time_point Timer::end_time_ = clock::now();

void Timer::Start() {
  start_time_ = clock::now();
  std::cout << "Timer started." << std::endl;
}

void Timer::Stop() {
  end_time_ = clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - start_time_);
  std::cout << "Timer stopped. Elapsed time: " << elapsed.count() << " ms" << std::endl;
}

double Timer::GetElapsedTimeMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - start_time_).count();
}

}  // namespace s21 