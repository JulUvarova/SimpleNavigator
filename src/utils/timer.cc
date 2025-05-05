#include "timer.h"

namespace s21 {

// Initialize static member
clock::time_point Timer::start_time_ = clock::now();

void Timer::Start() {
  start_time_ = clock::now();
  std::cout << "Timer started." << std::endl;
}

void Timer::Stop() {
  auto end_time = clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
  std::cout << "Timer stopped. Elapsed time: " << elapsed.count() << " ms" << std::endl;
}

}  // namespace s21 