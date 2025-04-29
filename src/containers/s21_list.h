#ifndef S21_LIST_H_
#define S21_LIST_H_

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <memory>

namespace s21 {

template <typename T, typename Allocator = std::allocator<T>>
class list {
 protected:
  struct BaseNode;
  struct ListNode;

  template <bool is_const = false>
  class ListIterator;

 public:
  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<allocator_type>;
  using node_allocator_type =
      typename allocator_traits::template rebind_alloc<ListNode>;
  using node_allocator_traits = std::allocator_traits<node_allocator_type>;

  using iterator = ListIterator<false>;
  using const_iterator = ListIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using value_type = T;
  using size_type = size_t;
  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;
  using difference_type = ptrdiff_t;

 protected:
  BaseNode sentinelNode_;
  size_t size_{0};
  node_allocator_type node_allocator_;

 public:
  list()
      : sentinelNode_([] {
          BaseNode node;
          node.next = &node;
          node.prev = &node;
          node.is_sentinel = true;
          return node;
        }()),
        size_(0),
        node_allocator_() {}

  list(const std::initializer_list<T> &items) : list() {
    for (const auto &item : items) {
      push_back(item);
    }
  }

  list(const list &other) : list() {
    if (allocator_traits::propagate_on_container_copy_assignment::value) {
      node_allocator_ = other.node_allocator_;
    }
    for (const auto &item : other) {
      push_back(item);
    }
  }

  list(list &&other) noexcept : list() { swap(other); }

  list(const size_type count, const T &value = T()) : list() {
    for (size_t i = 0; i < count; ++i) {
      push_back(value);
    }
  }

  ~list() { clear(); }

  list &operator=(const list &other) {
    if (this == &other) return *this;

    clear();
    if (other.size_ > 0) {
      BaseNode *current_other = other.sentinelNode_.next;
      while (current_other != &other.sentinelNode_) {
        push_back(static_cast<ListNode *>(current_other)->data);
        current_other = current_other->next;
      }
    }
    this->size_ = other.size_;
    return *this;
  }

  list &operator=(list &&other) noexcept {
    if (this != &other) {
      swap(other);
      other.clear();
    }
    return *this;
  }

  size_t size() const { return size_; }

  constexpr size_type max_size() const {
    return std::numeric_limits<difference_type>::max() / (3 * sizeof(void *));
  }

  void resize(const size_type count, const T &value = T()) {
    if (count < size_) {
      size_t diff = size_ - count;
      for (size_t i = 0; i < diff; ++i) {
        pop_back();
      }
    } else if (count > size_) {
      size_t diff = count - size_;
      for (size_t i = 0; i < diff; ++i) {
        push_back(value);
      }
    }
  }

  [[nodiscard]] constexpr bool empty() const { return size_ == 0; }

  iterator begin() { return iterator(sentinelNode_.next); }
  iterator end() { return iterator(&sentinelNode_); }

  const_iterator begin() const { return const_iterator(sentinelNode_.next); }
  const_iterator end() const { return const_iterator(&sentinelNode_); }

  const_iterator cbegin() const { return begin(); }
  const_iterator cend() const { return end(); }

  template <typename U = T>
  void push_back(U &&value) {
    auto newNode = create_node(std::forward<U>(value));
    insert_(end(), newNode);
  }

  template <typename U = T>
  void push_front(U &&value) {
    auto newNode = create_node(std::forward<U>(value));
    insert_(begin(), newNode);
  }

  void pop_back() {
    if (size_ == 0) return;

    if (size_ == 1) {
      destroy_node(static_cast<ListNode *>(sentinelNode_.prev));
      sentinelNode_.next = &sentinelNode_;
      sentinelNode_.prev = &sentinelNode_;
    } else {
      auto *lastNode = static_cast<ListNode *>(sentinelNode_.prev);
      sentinelNode_.prev = sentinelNode_.prev->prev;
      sentinelNode_.prev->next = &sentinelNode_;
      destroy_node(lastNode);
    }
    --size_;
  }

  void pop_front() {
    if (size_ == 0) return;

    auto *firstNode = static_cast<ListNode *>(sentinelNode_.next);
    sentinelNode_.next = sentinelNode_.next->next;
    sentinelNode_.next->prev = &sentinelNode_;
    destroy_node(firstNode);
    --size_;
  }

  template <typename U = T>
  iterator insert(iterator pos, U &&value) {
    auto newNode = create_node(std::forward<U>(value));
    return insert_(pos, newNode);
  }

  template <typename U = T>
  iterator insert(iterator pos, size_type count, U &&value) {
    for (size_t i = 0; i < count; ++i) {
      insert(pos, std::forward<U>(value));
    }
    return iterator(pos.node_);
  }

  iterator insert(iterator pos, iterator first, iterator last) {
    iterator ret(pos.node_);
    bool first_insert = true;

    for (auto it = first; it != last; ++it) {
      pos = insert(pos, *it);
      if (first_insert) {
        ret = pos;
        first_insert = false;
      }
      ++pos;
    }
    return ret;
  }

  template <typename... Args>
  iterator insert_many(iterator pos, Args &&...args) {
    iterator it = iterator(pos);
    ((insert(it, std::forward<Args>(args))), ...);
    return it;
  }

  template <typename... Args>
  void insert_many_back(Args &&...args) {
    ((push_back(std::forward<Args>(args))), ...);
  }

  template <typename... Args>
  void insert_many_front(Args &&...args) {
    ((push_front(std::forward<Args>(args))), ...);
  }

  iterator erase(iterator pos) {
    if (pos == end()) return end();

    auto *nodeToDelete = static_cast<ListNode *>(pos.node_);
    BaseNode *nextNode = nodeToDelete->next;

    nodeToDelete->prev->next = nodeToDelete->next;
    nodeToDelete->next->prev = nodeToDelete->prev;
    destroy_node(nodeToDelete);
    --size_;
    return iterator(nextNode);
  }

  iterator erase(iterator first, iterator last) {
    while (first != last) {
      first = erase(first);
    }
    return last;
  }

  constexpr const T &front() const {
    if (size_ == 0) {
      throw std::out_of_range("List is empty");
    }
    return static_cast<ListNode *>(sentinelNode_.next)->data;
  }

  constexpr T &front() {
    return const_cast<T &>(static_cast<const list *>(this)->front());
  }

  constexpr const T &back() const {
    if (size_ == 0) {
      throw std::out_of_range("List is empty");
    }
    return static_cast<ListNode *>(sentinelNode_.prev)->data;
  }

  constexpr T &back() {
    return const_cast<T &>(static_cast<const list *>(this)->back());
  }

  constexpr void clear() {
    while (size_ > 0) {
      pop_back();
    }
  }

  template <typename U = T>
  void swap(U &&other) {
    std::swap(sentinelNode_, other.sentinelNode_);
    std::swap(size_, other.size_);
    std::swap(node_allocator_, other.node_allocator_);
  }

  constexpr void merge(list &other) {
    if (this == &other || other.empty()) {
      return;
    }
    sentinelNode_.prev->next = other.sentinelNode_.next;
    other.sentinelNode_.next->prev = sentinelNode_.prev;

    sentinelNode_.prev = other.sentinelNode_.prev;
    other.sentinelNode_.prev->next = &sentinelNode_;

    size_ += other.size_;

    other.sentinelNode_.next = &other.sentinelNode_;
    other.sentinelNode_.prev = &other.sentinelNode_;
    other.size_ = 0;
  }

  constexpr void splice(const_iterator pos, list &other) {
    if (this == &other || other.empty()) {
      return;
    }
    pos.node_->prev->next = other.sentinelNode_.next;
    other.sentinelNode_.next->prev = pos.node_->prev;
    const_cast<BaseNode *>(pos.node_)->prev = other.sentinelNode_.prev;
    other.sentinelNode_.prev->next = const_cast<BaseNode *>(pos.node_);
    size_ += other.size_;

    other.sentinelNode_.next = &other.sentinelNode_;
    other.sentinelNode_.prev = &other.sentinelNode_;
    other.size_ = 0;
  }

  constexpr void reverse() {
    if (size_ <= 1) return;

    BaseNode *current = sentinelNode_.next;
    BaseNode *temp = nullptr;

    while (current != &sentinelNode_) {
      temp = current->next;
      current->next = current->prev;
      current->prev = temp;
      current = temp;
    }
    std::swap(sentinelNode_.next, sentinelNode_.prev);
  }

  constexpr void unique() {
    if (empty() || size_ == 1) {
      return;
    }

    BaseNode *current = sentinelNode_.next;
    while (current->next != &sentinelNode_) {
      BaseNode *nextNode = current->next;

      if (static_cast<ListNode *>(current)->data ==
          static_cast<ListNode *>(nextNode)->data) {
        current->next = nextNode->next;
        nextNode->next->prev = current;
        delete nextNode;
        --size_;
      } else {
        current = nextNode;
      }
    }
  }

  void sort() {
    if (size_ <= 1)
      return;  // No need to sort if the list is empty or has one element
    sentinelNode_.next = mergeSort(static_cast<ListNode *>(sentinelNode_.next));

    // Update the last node to point back to the sentinelNode_
    BaseNode *current = sentinelNode_.next;
    while (current->next != &sentinelNode_) {
      current = current->next;
    }
    sentinelNode_.prev = current;
    sentinelNode_.prev->next = &sentinelNode_;
    sentinelNode_.next->prev = &sentinelNode_;
  }

 private:
  ListNode *mergeSort(ListNode *head) {
    if (!head || head->next == &sentinelNode_) return head;

    ListNode *mid = getMiddle(head);
    ListNode *half = static_cast<ListNode *>(mid->next);
    mid->next = &sentinelNode_;

    ListNode *left = mergeSort(head);
    ListNode *right = mergeSort(half);

    return merge(left, right);
  }

  ListNode *getMiddle(ListNode *head) {
    if (!head) return head;

    ListNode *slow = head;
    ListNode *fast = head;

    while (fast->next != &sentinelNode_ && fast->next->next != &sentinelNode_) {
      slow = static_cast<ListNode *>(slow->next);
      fast = static_cast<ListNode *>(fast->next->next);
    }
    return slow;
  }

  ListNode *merge(ListNode *left, ListNode *right) {
    ListNode dummy(T{});
    ListNode *tail = &dummy;

    while (left != &sentinelNode_ && right != &sentinelNode_) {
      if (left->data < right->data) {
        tail->next = left;
        left->prev = tail;
        left = static_cast<ListNode *>(left->next);
      } else {
        tail->next = right;
        right->prev = tail;
        right = static_cast<ListNode *>(right->next);
      }
      tail = static_cast<ListNode *>(tail->next);
    }

    tail->next = (left != &sentinelNode_) ? left : right;
    if (tail->next != &sentinelNode_) tail->next->prev = tail;

    ListNode *head = static_cast<ListNode *>(dummy.next);
    while (tail->next != &sentinelNode_) {
      tail = static_cast<ListNode *>(tail->next);
    }
    tail->next = &sentinelNode_;
    sentinelNode_.prev = tail;

    return head;
  }

 private:
  template <typename U = T>
  ListNode *create_node(U &&value) {
    ListNode *node = node_allocator_traits::allocate(node_allocator_, 1);
    node_allocator_traits::construct(node_allocator_, node,
                                     std::forward<U>(value));
    return node;
  }

  void destroy_node(ListNode *node) {
    node_allocator_traits::destroy(node_allocator_, node);
    node_allocator_traits::deallocate(node_allocator_, node, 1);
  }

  iterator insert_(const_iterator pos, ListNode *newNode) {
    auto *pos_node = const_cast<BaseNode *>(pos.node_);

    newNode->next = pos_node;
    newNode->prev = pos_node->prev;
    pos_node->prev->next = newNode;
    pos_node->prev = newNode;

    ++size_;

    return iterator(newNode);
  }

 protected:
  struct BaseNode {
    BaseNode *next = nullptr;
    BaseNode *prev = nullptr;
    bool is_sentinel = false;
  };

  struct ListNode : public BaseNode {
    T data;
    ListNode(const T &value) : data(value) {}
    ListNode(T &&value) : data(std::move(value)) {}
    ListNode &operator=(const ListNode &other) {
      if (this != &other) {
        BaseNode::next = other.next;
        BaseNode::prev = other.prev;
        delete data;
        data = other.data;
      }
      return *this;
    };
    ListNode &operator=(ListNode &&other) noexcept {
      BaseNode::next = other.next;
      BaseNode::prev = other.prev;
      data = std::move(other.data);
      return *this;
    }
  };

  template <bool is_const>
  class ListIterator {
    friend class list;

    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_type =
        std::conditional_t<is_const, const ListNode *, ListNode *>;
    using value_type = std::conditional_t<is_const, const T, T>;
    using pointer_type = std::conditional_t<is_const, const T *, T *>;
    using reference_type = std::conditional_t<is_const, const T &, T &>;

    using node_pointer =
        std::conditional_t<is_const, const BaseNode *, BaseNode *>;
    node_pointer node_;

   public:
    ListIterator(node_pointer node) : node_(node) {}
    ListIterator(const ListIterator &other) = default;
    ListIterator &operator=(const ListIterator &other) = default;

    ListIterator &operator++() {
      node_ = node_->next;
      return *this;
    }

    ListIterator operator++(int) {
      ListIterator tmp(*this);
      node_ = node_->next;
      return tmp;
    }

    ListIterator &operator--() {
      node_ = node_->prev;
      return *this;
    }

    ListIterator operator--(int) {
      ListIterator tmp(*this);
      node_ = node_->prev;
      return tmp;
    }

    reference_type operator*() const {
      if (node_->is_sentinel) {
        throw std::out_of_range("Iterator out of range");
      }
      return static_cast<ListNode *>(const_cast<BaseNode *>(node_))->data;
    }

    pointer_type operator->() const {
      if (node_->is_sentinel) {
        throw std::out_of_range("Iterator out of range");
      }
      return &static_cast<ListNode *>(const_cast<BaseNode *>(node_))->data;
    }

    bool operator==(const ListIterator &other) const {
      return node_ == other.node_;
    }
    bool operator!=(const ListIterator &other) const {
      return node_ != other.node_;
    }

    operator ListIterator<true>() const { return ListIterator<true>(node_); }
  };
};

}  // namespace s21

#endif
