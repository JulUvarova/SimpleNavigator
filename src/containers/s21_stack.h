#ifndef S21_STACK_H_
#define S21_STACK_H_
// #include "s21_container.h"
#include <iostream>
#include <limits>

#include "s21_vector.h"

namespace s21 {

template <typename T>
class stack {
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using size_type = std::size_t;
  using container_type = s21::vector<value_type>;

 private:
  container_type container_;

 public:
  stack() {}

  stack(std::initializer_list<value_type> const &items) {
    for (const auto &item : items) {
      push(item);
    }
  }

  // copy constructor
  stack(const stack &s) : container_(s.container_) {}

  // move constructor
  stack(stack &&s) noexcept { swap(s); }

  // destructor
  ~stack() {}

  // assignment copy operator
  stack &operator=(const stack &s) {
    if (this != &s) {
      container_ = s.container_;
    }
    return *this;
  }

  // assignment operator overload for moving object
  stack &operator=(stack &&s) noexcept {
    if (this != &s) {
      swap(s);
    }
    return *this;
  }

  // accesses the top elements
  const_reference top() { return (container_.back()); }

  // checks whether the container is empty
  bool empty() { return container_.empty(); }

  // returns the number of elements
  size_type size() { return container_.size(); }

  // inserts element at the top
  void push(const_reference value) { container_.push_back(value); }

  // removes the top element
  void pop() { container_.pop_back(); }

  // swaps the contents
  void swap(stack &other) {
    container_type d = container_;
    container_ = other.container_;
    other.container_ = d;
  }

  // Appends new elements to the end of the container
  template <typename... Args>
  void insert_many_back(Args &&...args) {
    container_.insert_many_back(args...);
  }
};

}  // namespace s21

#endif