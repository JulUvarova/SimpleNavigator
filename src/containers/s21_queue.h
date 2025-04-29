#ifndef S21_QUEUE_H_
#define S21_QUEUE_H_
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <memory>

#include "s21_list.h"
namespace s21 {
template <typename T, typename Container = list<T>>
class queue {
 protected:
  Container container_;

 public:
  using value_type = T;
  using size_type = size_t;
  using reference = T &;
  using const_reference = const T &;

  queue() : container_() {};
  queue(const queue &other) : container_(other.container_) {}
  queue(queue &&other) noexcept : container_(std::move(other.container_)) {}
  explicit queue(std::initializer_list<T> init) : container_(init) {}
  ~queue() { container_.clear(); }

  queue &operator=(const queue &other) {
    if (this == &other) {
      return *this;
    }
    container_ = other.container_;
    return *this;
  }

  queue &operator=(queue &&other) noexcept {
    if (this != &other) {
      container_ = std::move(other.container_);
    }
    return *this;
  }

  bool empty() const { return container_.empty(); }

  size_t size() const { return container_.size(); }

  T &front() { return container_.front(); }

  T &back() { return container_.back(); }

  void push(const T &value) { container_.push_back(value); }

  void pop() { container_.pop_front(); }

  void swap(queue &other) { container_.swap(other.container_); }

  template <typename... Args>
  void insert_many_back(Args &&...args) {
    ((push(std::forward<Args>(args))), ...);
  }
};
}  // namespace s21

#endif