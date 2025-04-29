#ifndef S21_VECTOR_H_
#define S21_VECTOR_H_
#include <iostream>
#include <limits>

namespace s21 {

template <typename T>
class vector {
  using size_type = std::size_t;
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using iterator = T *;
  using const_iterator = const T *;

 private:
  T *data_;             // Pointer to the dynamic array
  size_type size_;      // Number of elements currently in the vector
  size_type capacity_;  // Allocated capacity

 public:
  // Constructors, destructor, and assignment operators will be declared here

  // Default constructor
  vector() : data_(nullptr), size_(0), capacity_(0) {}

  // Parameterized constructor, creates the vector of size n
  vector(size_type n) : vector() { resize(n); }

  // Initializer list constructor, creates a vector initizialized using
  // std::initializer_list
  vector(std::initializer_list<value_type> const &items) : vector() {
    for (const auto &item : items) {
      push_back(item);
    }
  }

  // Copy Constructor
  vector(const vector &other)
      : data_(nullptr), size_(other.size_), capacity_(other.size_) {
    if (capacity_ > 0) {
      data_ = static_cast<T *>(::operator new(capacity_ * sizeof(T)));
      for (size_t i = 0; i < size_; ++i) {
        new (&data_[i]) T(other.data_[i]);  // Copy construct each element
      }
    }
  }

  // Move Constructor
  vector(vector &&other) noexcept
      : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  // Destructor
  ~vector() {
    clear();                   // Destroy all elements
    ::operator delete(data_);  // Deallocate memory
  }

  // Copy Assignment Operator
  vector<T> &operator=(const vector &other) {
    if (this != &other) {
      vector temp(other);  // Copy constructor
      swap(temp);          // Swap with temporary
    }
    return *this;
  }

  // Move Assignment Operator
  vector<T> &operator=(vector &&other) noexcept {
    if (this != &other) {
      clear();  // Destroy current elements

      ::operator delete(data_);  // Deallocate current memory

      data_ = other.data_;
      size_ = other.size_;
      capacity_ = other.capacity_;

      other.data_ = nullptr;
      other.size_ = 0;
      other.capacity_ = 0;
    }
    return *this;
  }

  // Access a specified element
  reference operator[](size_t index) {
    // Optionally add bounds checking in debug mode
    return data_[index];
  }

  // Access a specified element with bounds checking
  reference at(size_type index) {
    if (index >= size_) {
      throw std::out_of_range("out of range");
    }
    return data_[index];
  }

  // Access the first element
  const_reference front() { return data_[0]; }

  // Access the last element
  const_reference back() { return data_[size_ - 1]; }

  // direct access the underlying array
  T *data() { return data_; }

  // returns an iterator to the beginning
  iterator begin() { return &data_[0]; }

  // returns an iterator to the end
  iterator end() { return &data_[size_]; }

  // checks whether the container is empty
  bool empty() { return !(bool)size_; }

  // Returns the number of elements in container
  size_t size() const { return size_; }

  // returns the maximum possible number of elements
  size_type max_size() {
    return (std::numeric_limits<size_type>::max() / sizeof(value_type)) / 2;
  }

  // Allocate storage of size elements and copies current array elements to a
  // newely allocated array
  void reserve(size_t new_capacity) {
    if (new_capacity <= capacity_) return;

    if (new_capacity > max_size()) {
      throw std::length_error("new capacity greater than maximum capacity");
    }

    T *new_data = static_cast<T *>(::operator new(new_capacity * sizeof(T)));
    for (size_t i = 0; i < size_; ++i) {
      new (&new_data[i])
          T(std::move(data_[i]));  // Move construct into new array
      data_[i].~T();               // Destroy old object
    }
    ::operator delete(data_);  // Deallocate old memory
    data_ = new_data;
    capacity_ = new_capacity;
  }

  // Returns the number of elements that can be held in currently
  // allocated storage
  size_t capacity() const { return capacity_; }

  // Resizes the container
  void resize(size_t new_size) {
    if (new_size < size_) {
      for (size_t i = new_size; i < size_; ++i) {
        data_[i].~T();  // Destroy surplus elements
      }
    } else if (new_size > size_) {
      if (new_size > capacity_) {
        reserve(new_size);  // Reserve enough capacity
      }
      for (size_t i = size_; i < new_size; ++i) {
        new (&data_[i]) T();  // Default-construct new elements
      }
    }
    size_ = new_size;
  }

  // Reduces memory usage by freeing unused memory
  void shrink_to_fit() {
    size_t new_capacity = size_;

    T *new_data = static_cast<T *>(::operator new(new_capacity * sizeof(T)));
    for (size_t i = 0; i < size_; ++i) {
      new (&new_data[i])
          T(std::move(data_[i]));  // Move construct into new array
      data_[i].~T();               // Destroy old object
    }
    ::operator delete(data_);  // Deallocate old memory
    data_ = new_data;
    capacity_ = new_capacity;
  }

  // Clear container elements
  void clear() {
    for (size_t i = 0; i < size_; ++i) {
      data_[i].~T();  // Manually call destructor
    }
    size_ = 0;
  }

  // inserts elements into concrete pos and returns the iterator that points
  // to the new element iterator
  iterator insert(const_iterator pos, const_reference value) {
    size_type index = pos - begin();
    resize(size_ + 1);
    for (size_type i = size_ - 1; i > index; --i) {
      new (&data_[i]) T(std::move(data_[i - 1]));
    }
    new (&data_[index]) T(value);
    return begin() + index;
  }

  // erases an element at position
  void erase(const_iterator pos) {
    size_type index = pos - begin();
    data_[index].~T();
    for (size_type i = index; i < size_ - 1; ++i) {
      new (&data_[i]) T(std::move(data_[i + 1]));
    }
    --size_;
  }

  // Copy adds an element to the end
  void push_back(const T &value) {
    if (size_ == capacity_) {
      reserve(capacity_ == 0 ? 1 : capacity_ * 2);  // Double capacity
    }
    new (&data_[size_]) T(value);  // Copy-construct new element
    ++size_;
  }

  // Move adds an element to the end
  void push_back(T &&value) {
    if (size_ == capacity_) {
      reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    new (&data_[size_]) T(std::move(value));  // Move-construct new element
    ++size_;
  }

  template <typename... Args>
  void emplace_back(Args &&...args) {
    if (size_ == capacity_) {
      reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    new (&data_[size_])
        T(std::forward<Args>(args)...);  // Perfect-forward arguments
    ++size_;
  }

  // Removes the last element
  void pop_back() {
    if (size_ > 0) {
      data_[size_ - 1].~T();
      --size_;
    }
  }

  // Swap Function (Helper)
  void swap(vector &other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  // Inserts new elements into the container directly before pos
  template <typename... Args>
  iterator insert_many(const_iterator pos, Args &&...args) {
    iterator it = iterator(pos);
    ((it = insert(it, std::forward<Args>(args)) + 1), ...);
    return it;
  }

  template <typename... Args>
  void insert_many_back(Args &&...args) {
    ((push_back(std::forward<Args>(args))), ...);
  }
};

}  // namespace s21

#endif