#ifndef S21_MULTISET_H_
#define S21_MULTISET_H_

#include "s21_binary_tree.h"
#include "s21_vector.h"

namespace s21 {
template <typename Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
class multiset : public BinaryTree<Key, char, Compare, Allocator> {
  multiset &ref_ = *this;

 public:
  using key_type = Key;
  using value_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = size_t;
  using Node = typename BinaryTree<Key, char, Compare, Allocator>::Node;

  multiset() : BinaryTree<Key, char, Compare, Allocator>() {};

  explicit multiset(std::initializer_list<key_type> const &items)
      : BinaryTree<Key, char, Compare, Allocator>() {
    for (auto item : items) {
      insert(item);
    }
  };

  multiset(const multiset &s) : BinaryTree<Key, char, Compare, Allocator>(s) {};

  multiset(multiset &&s)
      : BinaryTree<Key, char, Compare, Allocator>(std::move(s)) {};

  ~multiset() {};

  multiset &operator=(multiset &&s) noexcept {
    if (this != &s) {
      BinaryTree<Key, char, Compare, Allocator>::operator=(std::move(s));
    }
    return *this;
  }

  multiset &operator=(const multiset &s) noexcept {
    if (this != &s) {
      BinaryTree<Key, char, Compare, Allocator>::operator=(s);
    }
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

  size_type count(const key_type &key) const override {
    Node *found = BinaryTree<Key, char, Compare, Allocator>::find_by_key(key);
    return found ? found->value : 0;
  }

  iterator insert(const key_type &value) {
    Node *found = BinaryTree<Key, char, Compare, Allocator>::find_by_key(value);
    if (!found) {
      found =
          BinaryTree<Key, char, Compare, Allocator>::insert_by_key(value, 1);
      if (!found) found = this->fake_;
    } else {
      ++this->size_;
      ++found->value;
    }
    return Iterator(ref_, found);
  }

  void merge(multiset &other) {
    std::vector<Key> merged;
    for (const auto &item : other) {
      if (insert(item) != this->end()) {
        merged.push_back(item);  // cppcheck-suppress useStlAlgorithm
      }
    }
    for (const auto &item : merged) {
      other.erase(item);
    }
  }

  iterator find(const key_type &key) const {
    Node *found = BinaryTree<Key, char, Compare, Allocator>::find_by_key(key);
    return Iterator(ref_, ((found == nullptr) ? this->fake_ : found));
  }

  void erase(const Iterator &pos) {
    if (Node *found =
            BinaryTree<Key, char, Compare, Allocator>::find_by_key(*pos)) {
      if (found->value > 1) {
        --found->value;
        --this->size_;
      } else {
        BinaryTree<Key, char, Compare, Allocator>::remove_node(found);
      }
    }
  }

  size_type erase(const key_type &key) override {
    if (Node *found =
            BinaryTree<Key, char, Compare, Allocator>::find_by_key(key)) {
      size_type res = found->value;
      this->size_ -= (res - 1);
      BinaryTree<Key, char, Compare, Allocator>::remove_node(found);
      return res;
    }
    return 0;
  }

  iterator lower_bound(const key_type &key) {
    Node *found =
        BinaryTree<Key, char, Compare, Allocator>::find_lower_bound(key);
    return found ? Iterator(ref_, found) : end();
  }

  iterator upper_bound(const key_type &key) {
    Node *found =
        BinaryTree<Key, char, Compare, Allocator>::find_upper_bound(key);
    return found ? Iterator(ref_, found) : end();
  }

  std::pair<Iterator, Iterator> equal_range(const key_type &key) {
    return std::make_pair(lower_bound(key), upper_bound(key));
  }

  iterator begin() const {
    return Iterator(ref_,
                    BinaryTree<Key, char, Compare, Allocator>::get_begin());
  }
  iterator end() const { return Iterator(ref_, this->fake_); }

  const_iterator cbegin() const {
    return const_iterator(
        ref_, BinaryTree<Key, char, Compare, Allocator>::get_begin());
  }

  const_iterator cend() const { return const_iterator(ref_, this->fake_); }

  template <typename... Args>
  s21::vector<std::pair<Iterator, bool>> insert_many(Args &&...args) {
    s21::vector<std::pair<iterator, bool>> result;
    (result.push_back({insert_pairs(args)}), ...);
    return result;
  }

  class Iterator {
    multiset &multiset_;
    Node *elem_;
    int count_;

   public:
    using difference_type = std::ptrdiff_t;
    using value_type = Key;
    using pointer = const Key *;
    using reference = Key &;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit Iterator(multiset &multiset, Node *elem = nullptr, int count = 1)
        : multiset_{multiset}, elem_{elem}, count_{count} {}

    Iterator(const Iterator &other)
        : Iterator(other.multiset_, other.elem_, other.count_) {}

    Iterator(Iterator &&other) noexcept
        : multiset_{other.multiset_}, elem_{other.elem_}, count_{other.count_} {
      other.count_ = 1;
      other.elem_ = nullptr;
    }

    Iterator &operator=(const Iterator &other) {
      if (this != &other) {
        elem_ = other.elem_;
        count_ = other.count_;
      }
      return *this;
    }

    Iterator &operator=(Iterator &&other) noexcept {
      if (this != &other) {
        elem_ = other.elem_;
        count_ = other.count_;
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
      if (count_ == elem_->value) {
        elem_ = multiset_.increment_node(elem_);
        count_ = 1;
      } else {
        ++count_;
      }
      return *this;
    }

    Iterator &operator--() {
      if (count_ == 1) {
        elem_ = multiset_.decrement_node(elem_);
        count_ = elem_->value;
      } else {
        --count_;
      }
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

 private:
  std::pair<Iterator, bool> insert_pairs(const key_type &value) {
    auto it = insert(value);
    return std::make_pair(it, (it == this->end()) ? false : true);
  }
};
}  // namespace s21

#endif