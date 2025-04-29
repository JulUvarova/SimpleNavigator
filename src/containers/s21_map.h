#ifndef S21_MAP_H_
#define S21_MAP_H_

#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "s21_binary_tree.h"
#include "s21_vector.h"

namespace s21 {

template <class Key, class T, class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
class map : public BinaryTree<Key, T, Compare, Allocator> {
  map& ref_ = *this;

 public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const key_type, mapped_type>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using Node = typename BinaryTree<Key, T, Compare, Allocator>::Node;
  using size_type = std::size_t;

  map() : BinaryTree<Key, T, Compare, Allocator>() {};

  explicit map(std::initializer_list<value_type> const& items)
      : BinaryTree<Key, T, Compare, Allocator>() {
    for (const auto& item : items) {
      insert(item.first, item.second);
    }
  };

  map(const map& m) : BinaryTree<Key, T, Compare, Allocator>(m) {};

  map(map&& m) : BinaryTree<Key, T, Compare, Allocator>(std::move(m)) {};

  ~map() {};

  map& operator=(map&& m) noexcept {
    BinaryTree<Key, T, Compare, Allocator>::operator=(std::move(m));
    return *this;
  }

  map& operator=(const map& m) noexcept {
    BinaryTree<Key, T, Compare, Allocator>::operator=(m);
    return *this;
  }

  class Iterator;

  class ConstIterator : public Iterator {
    using Iterator::Iterator;
  };

  using iterator = Iterator;
  using const_iterator = ConstIterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  std::pair<iterator, bool> add(const Key& key, const T& obj,
                                bool assign = false) {
    Node* found = BinaryTree<Key, T, Compare, Allocator>::find_by_key(key);
    bool inserted = false;
    if (!found) {
      found = BinaryTree<Key, T, Compare, Allocator>::insert_by_key(key, obj);
      if (!found) {
        found = this->fake_;  // LCOV_EXCL_LINE
      } else {
        inserted = true;
      }
    } else if (assign) {
      found->value = obj;
    }
    return std::make_pair(Iterator(ref_, found), inserted);
  }

  std::pair<iterator, bool> insert(const Key& key, const T& obj) {
    return add(key, obj, false);
  }

  std::pair<iterator, bool> insert(const value_type& value) {
    return add(value.first, value.second, false);
  }

  std::pair<iterator, bool> insert_or_assign(const Key& key, const T& obj) {
    return add(key, obj, true);
  }

  T& at(const Key& key) {
    Node* found = BinaryTree<Key, T, Compare, Allocator>::find_by_key(key);
    if (!found) throw std::out_of_range("key not found");
    return found->value;
  }

  T& operator[](const Key& key) {
    Node* found = BinaryTree<Key, T, Compare, Allocator>::find_by_key(key);
    if (!found) {
      found = BinaryTree<Key, T, Compare, Allocator>::insert_by_key(key, T());
    }
    return found->value;
  }

  void merge_node(Node* node, map& m) {
    if (!node) return;

    auto left = node->left;
    auto right = node->right;

    auto addition_result = insert(node->key, node->value);
    if (addition_result.second) {
      m.erase(node->key);
    }

    merge_node(left, m);
    merge_node(right, m);
  }

  void merge(map& other) { merge_node(other.root_, other); }

  Iterator begin() const {
    return Iterator(ref_, BinaryTree<Key, T, Compare, Allocator>::get_begin());
  }

  Iterator end() const { return Iterator(ref_, this->fake_); }

  ConstIterator cbegin() const {
    return ConstIterator(ref_,
                         BinaryTree<Key, T, Compare, Allocator>::get_begin());
  }

  ConstIterator cend() const { return ConstIterator(ref_, this->fake_); }

  class Iterator {
    map& map_;
    Node* elem_;

   public:
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit Iterator(map& map, Node* elem = nullptr)
        : map_{map}, elem_{elem} {}

    Iterator(const Iterator& other) : Iterator(other.map_, other.elem_) {}

    Iterator(Iterator&& other) noexcept : Iterator(other.map_, other.elem_) {
      other.elem_ = nullptr;
    }

    Iterator& operator=(const Iterator& other) {
      if (this != &other) {
        elem_ = other.elem_;
      }
      return *this;
    }

    Iterator& operator=(const Iterator&& other) noexcept {
      if (this != &other) {
        elem_ = other.elem_;
        other.elem_ = nullptr;
      }
      return *this;
    }

    ~Iterator() noexcept = default;

    mapped_type& operator*() const { return elem_->value; }

    mapped_type* operator->() const { return &(elem_->value); }

    bool operator==(const Iterator& it) const { return elem_ == it.elem_; }

    bool operator!=(const Iterator& it) const { return elem_ != it.elem_; }

    // префикс
    Iterator& operator++() {
      elem_ = map_.increment_node(elem_);
      return *this;
    }

    Iterator& operator--() {
      elem_ = map_.decrement_node(elem_);
      return *this;
    }

    // постфикс
    Iterator operator++(int) {
      Iterator tmp = *this;
      operator++();
      return tmp;
    }

    Iterator operator--(int) {
      Iterator tmp = *this;
      operator--();
      return tmp;
    }
  };

  template <typename... Args>
  s21::vector<std::pair<iterator, bool>> insert_many(Args&&... args) {
    s21::vector<std::pair<iterator, bool>> result;
    result.reserve(sizeof...(args));
    (result.push_back(add(std::forward<Args>(args).first,
                          std::forward<Args>(args).second, false)),
     ...);
    return result;
  }
};
}  // namespace s21

#endif