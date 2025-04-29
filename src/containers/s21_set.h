#ifndef S21_SET_H_
#define S21_SET_H_

#include "s21_binary_tree.h"
#include "s21_vector.h"

namespace s21 {
template <typename Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
class set : public BinaryTree<Key, char, Compare, Allocator> {
  set &ref_ = *this;

 public:
  using key_type = Key;
  using value_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using Node = typename BinaryTree<Key, char, Compare, Allocator>::Node;
  using size_type = size_t;

  set() : BinaryTree<Key, char, Compare, Allocator>() {};

  explicit set(std::initializer_list<key_type> const &items)
      : BinaryTree<Key, char, Compare, Allocator>() {
    for (auto item : items) {
      insert(item);
    }
  };

  set(const set &s) : BinaryTree<Key, char, Compare, Allocator>(s) {};

  set(set &&s) : BinaryTree<Key, char, Compare, Allocator>(std::move(s)) {};

  ~set() {};

  set &operator=(set &&s) noexcept {
    BinaryTree<Key, char, Compare, Allocator>::operator=(std::move(s));
    return *this;
  }

  set &operator=(const set &s) noexcept {
    BinaryTree<Key, char, Compare, Allocator>::operator=(s);
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

  std::pair<Iterator, bool> insert(const key_type &value) {
    Node *found = BinaryTree<Key, char, Compare, Allocator>::find_by_key(value);
    bool inserted = false;
    if (!found) {
      found =
          BinaryTree<Key, char, Compare, Allocator>::insert_by_key(value, 1);
      if (!found)
        found = this->fake_;  // если не создалось
      else
        inserted = true;
    }
    return std::make_pair(Iterator(ref_, found), inserted);
  }

  void merge(set &other) {
    s21::vector<Key> merged;
    for (auto item : other) {
      if (insert(item).second) {
        merged.push_back(item);  // cppcheck-suppress useStlAlgorithm
      }
    }
    for (auto item : merged) {
      other.erase(item);
    }
  }

  Iterator find(const key_type &key) const {
    Node *found = BinaryTree<Key, char, Compare, Allocator>::find_by_key(key);
    return Iterator(ref_, ((found == nullptr) ? this->fake_ : found));
  }

  void erase(Iterator pos) {
    BinaryTree<Key, char, Compare, Allocator>::erase(*pos);
  }

  size_type erase(const key_type &key) override {
    return BinaryTree<Key, char, Compare, Allocator>::erase(key);
  }

  Iterator lower_bound(const key_type &key) {
    Node *found =
        BinaryTree<Key, char, Compare, Allocator>::find_lower_bound(key);
    return found ? Iterator(ref_, found) : end();
  }

  Iterator upper_bound(const key_type &key) {
    Node *found =
        BinaryTree<Key, char, Compare, Allocator>::find_upper_bound(key);
    return found ? Iterator(ref_, found) : end();
  }

  std::pair<Iterator, Iterator> equal_range(const key_type &key) {
    return std::make_pair(lower_bound(key), upper_bound(key));
  }

  Iterator begin() const {
    return Iterator(ref_,
                    BinaryTree<Key, char, Compare, Allocator>::get_begin());
  }

  Iterator end() const { return Iterator(ref_, this->fake_); }

  ConstIterator cbegin() const {
    return ConstIterator(
        ref_, BinaryTree<Key, char, Compare, Allocator>::get_begin());
  }

  ConstIterator cend() const { return ConstIterator(ref_, this->fake_); }

  template <typename... Args>
  s21::vector<std::pair<Iterator, bool>> insert_many(Args &&...args) {
    s21::vector<std::pair<iterator, bool>> result;
    (result.push_back(insert(args)), ...);
    return result;
  }

  class Iterator {
    set &set_;
    Node *elem_;

   public:
    using difference_type = std::ptrdiff_t;
    using value_type = Key;
    using pointer = const Key *;
    using reference = Key &;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit Iterator(set &set, Node *elem = nullptr)
        : set_{set}, elem_{elem} {}

    Iterator(const Iterator &other) : Iterator(other.set_, other.elem_) {}

    Iterator(Iterator &&other) noexcept : Iterator(other.set_, other.elem_) {
      other.elem_ = nullptr;
    }

    Iterator &operator=(const Iterator &other) {
      if (this != &other) {
        elem_ = other.elem_;
      }
      return *this;
    }

    Iterator &operator=(const Iterator &&other) noexcept {
      if (this != &other) {
        elem_ = other.elem_;
        other.elem_ = nullptr;
      }
      return *this;
    }

    ~Iterator() noexcept = default;

    const reference operator*() const { return elem_->key; }

    pointer operator->() const { return &(elem_->key); }

    bool operator==(const Iterator &it) const { return elem_ == it.elem_; }

    bool operator!=(const Iterator &it) const { return elem_ != it.elem_; }

    Iterator &operator++() {
      elem_ = set_.increment_node(elem_);
      return *this;
    }

    Iterator &operator--() {
      elem_ = set_.decrement_node(elem_);
      return *this;
    }

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
};
}  // namespace s21

#endif