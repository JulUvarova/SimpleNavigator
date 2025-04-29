#ifndef S21_ARRAY_H_
#define S21_ARRAY_H_

#include <cstddef>
#include <exception>
#include <iterator>
#include <utility>

namespace s21 {
template <typename T, std::size_t N>
struct array_wrapper_ {
  using content = T[N];

  static constexpr T& get_reference(const content& arr,
                                    std::size_t pos) noexcept {
    return const_cast<T&>(arr[pos]);
  }

  static constexpr T* get_pointer(const content& arr) noexcept {
    return const_cast<T*>(arr);
  }
};

template <typename T>
struct array_wrapper_<T, 0> {
  struct content {};

  [[noreturn]] static constexpr T& get_reference(const content&,
                                                 std::size_t) noexcept {
    std::terminate();
  }

  static constexpr T* get_pointer(const content&) noexcept { return nullptr; }
};

template <class T, std::size_t N>
struct array {
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using array_wrapper = array_wrapper_<T, N>;
  typename array_wrapper::content arr;

  inline constexpr size_type size() const noexcept { return N; }
  inline constexpr size_type max_size() const noexcept { return size(); }
  inline constexpr size_type empty() const noexcept { return (N == 0); }
  inline constexpr pointer data() noexcept {
    return array_wrapper::get_pointer(arr);
  }
  inline constexpr const_pointer data() const noexcept {
    return array_wrapper::get_pointer(arr);
  }

  inline constexpr iterator begin() noexcept { return iterator(data()); }
  inline constexpr const_iterator begin() const noexcept {
    return iterator(data());
  }
  inline constexpr const_iterator cbegin() const noexcept {
    return iterator(data());
  }
  inline constexpr iterator end() noexcept { return iterator(data() + N); }
  inline constexpr const_iterator end() const noexcept {
    return const_iterator(data() + N);
  }
  inline constexpr const_iterator cend() const noexcept {
    return const_iterator(data() + N);
  }
  inline constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  inline constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  inline constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  inline constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  inline constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  inline constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(begin());
  }

  inline constexpr reference operator[](size_type n) noexcept {
    return array_wrapper::get_reference(arr, n);
  }

  inline constexpr const_reference operator[](size_type n) const noexcept {
    return array_wrapper::get_reference(arr, n);
  }

  constexpr reference at(size_type pos);
  constexpr const_reference at(size_type pos) const;

  inline constexpr reference front() { return *begin(); }
  inline constexpr const_reference front() const { return *begin(); }
  inline constexpr reference back() { return *(end() - 1); }
  inline constexpr const_reference back() const { return *(end() - 1); }

  void fill(const_reference value);
  void swap(array& other) noexcept(std::is_nothrow_swappable_v<T>);
};

template <class T, std::size_t N>
constexpr typename array<T, N>::reference array<T, N>::at(size_type pos) {
  return const_cast<reference>(std::as_const(*this).at(pos));
}

template <class T, std::size_t N>
constexpr typename array<T, N>::const_reference array<T, N>::at(
    size_type pos) const {
  if (pos >= N) {
    throw std::out_of_range("Index out of range");
  }
  return (*this)[pos];
}

template <class T, std::size_t N>
void array<T, N>::fill(const_reference value) {
  for (size_type i = 0; i < N; ++i) {
    (*this)[i] = value;
  }
}

template <class T, std::size_t N>
void array<T, N>::swap(array& other) noexcept(std::is_nothrow_swappable_v<T>) {
  for (size_type i = 0; i < N; ++i) {
    std::swap((*this)[i], other[i]);
  }
}

template <class T, std::size_t N>
bool operator==(const array<T, N>& lhs, const array<T, N>& rhs) {
  bool res = true;
  for (size_t i = 0; res && i < N; ++i) {
    if (lhs[i] != rhs[i]) {
      res = false;
    }
  }
  return res;
}

template <class T, std::size_t N>
bool operator!=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return !(lhs == rhs);
}

template <class T, std::size_t N>
bool operator<(const array<T, N>& lhs, const array<T, N>& rhs) {
  for (size_t i = 0; i < N; ++i) {
    if (lhs[i] < rhs[i]) {
      return true;
    } else if (lhs[i] > rhs[i]) {
      return false;
    }
  }
  return false;
}

template <class T, std::size_t N>
bool operator<=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return lhs < rhs || lhs == rhs;
}

template <class T, std::size_t N>
bool operator>(const array<T, N>& lhs, const array<T, N>& rhs) {
  return !(lhs < rhs) && lhs != rhs;
}

template <class T, std::size_t N>
bool operator>=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return lhs > rhs || lhs == rhs;
}

template <class T, std::size_t N>
void swap(array<T, N>& lhs,
          array<T, N>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

template <std::size_t I, class T, std::size_t N>
constexpr T& get(array<T, N>& a) noexcept {
  return a[I];
}

template <std::size_t I, class T, std::size_t N>
constexpr T&& get(array<T, N>&& a) noexcept {
  return std::move(a[I]);
}

template <std::size_t I, class T, std::size_t N>
constexpr const T& get(const array<T, N>& a) noexcept {
  return a[I];
}

template <std::size_t I, class T, std::size_t N>
constexpr const T&& get(const array<T, N>&& a) noexcept {
  return std::move(a[I]);
}
}  // namespace s21

#endif