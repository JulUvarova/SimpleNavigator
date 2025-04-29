#ifndef S21_BINARY_TREE_H_
#define S21_BINARY_TREE_H_

#include <limits>

namespace s21 {
template <typename Key, typename Value, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
class BinaryTree {
 protected:
  struct Node;

  using key_type = Key;
  using value_type = Value;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<allocator_type>;
  using node_allocator_type =
      typename allocator_traits::template rebind_alloc<Node>;
  using node_allocator_traits = std::allocator_traits<node_allocator_type>;

  size_type size_ = 0;
  Node *root_ = nullptr;
  Node *fake_ = new Node();
  node_allocator_type alloc_;

 public:
  BinaryTree() { fake_->parent = fake_; }

  BinaryTree(const BinaryTree &bt) : BinaryTree() { copy_node(bt.root_); }

  BinaryTree(BinaryTree &&bt) noexcept : BinaryTree() {
    std::swap(this->size_, bt.size_);
    std::swap(this->root_, bt.root_);
    std::swap(this->fake_, bt.fake_);
    std::swap(this->alloc_, bt.alloc_);
  }

  ~BinaryTree() noexcept {
    clear();
    destroy_node(fake_);
  }

  BinaryTree &operator=(BinaryTree &&bt) noexcept {
    if (this != &bt) {
      std::swap(this->size_, bt.size_);
      std::swap(this->root_, bt.root_);
      std::swap(this->fake_, bt.fake_);
      std::swap(this->alloc_, bt.alloc_);
      bt.clear();
    }
    return *this;
  }

  BinaryTree &operator=(const BinaryTree &bt) {
    if (this != &bt) {
      clear();
      copy_node(bt.root_);
    }
    return *this;
  }

 private:
  unsigned char get_height(Node *elem) const noexcept {
    return elem ? elem->height : 0;
  }

  int balance_factor(Node *elem) const noexcept {
    return get_height(elem->right) - get_height(elem->left);
  }

  void set_height(Node *elem) noexcept {
    unsigned char left = get_height(elem->left);
    unsigned char right = get_height(elem->right);
    elem->height = (left > right ? left : right) + 1;
  }

  Node *right_rotate(Node *elem) noexcept {
    Node *l = elem->left->left;
    Node *r_r = elem->right;
    Node *r_l = elem->left->right;
    std::swap(elem->key, elem->left->key);
    std::swap(elem->value, elem->left->value);
    elem->right = elem->left;
    elem->left = l;
    if (elem->left) elem->left->parent = elem;
    elem->right->left = r_l;
    if (elem->right->left) elem->right->left->parent = elem->right;
    elem->right->right = r_r;
    if (elem->right->right) elem->right->right->parent = elem->right;

    set_height(elem->right);
    set_height(elem);
    return elem;
  }

  Node *left_rotate(Node *elem) noexcept {
    Node *r = elem->right->right;
    Node *l_l = elem->left;
    Node *l_r = elem->right->left;
    std::swap(elem->key, elem->right->key);
    std::swap(elem->value, elem->right->value);
    elem->left = elem->right;
    elem->right = r;
    if (elem->right) elem->right->parent = elem;
    elem->left->right = l_r;
    if (elem->left->right) elem->left->right->parent = elem->left;
    elem->left->left = l_l;
    if (elem->left->left) elem->left->left->parent = elem->left;

    set_height(elem);
    set_height(elem->parent);
    return elem;
  }

  void balance(Node *elem) {
    set_height(elem);
    if (balance_factor(elem) == 2) {
      if (balance_factor(elem->right) < 0)
        elem->right = right_rotate(elem->right);
      left_rotate(elem);
    }
    if (balance_factor(elem) == -2) {
      if (balance_factor(elem->left) > 0) elem->left = left_rotate(elem->left);
      right_rotate(elem);
    }
  }

  Node *get_min(Node *elem) const {
    return (elem->left) ? get_min(elem->left) : elem;
  }

  Node *get_max(Node *elem) const {
    return elem->right ? get_max(elem->right) : elem;
  }

  Node *find_node(Node *elem, const key_type &key) const {
    if (!elem || key == elem->key) return elem;
    if (Compare()(key, elem->key)) {
      return find_node(elem->left, key);
    } else {
      return find_node(elem->right, key);
    }
  }

  Node *insert_node(Node *elem, key_type &key, value_type &value,
                    Node *parent) {
    if (!elem) {
      ++size_;
      elem = create_node(key, value);
      elem->parent = parent;
      if (!root_) {
        root_ = elem;
        fake_->left = root_;
        return root_;
      }
      (Compare()(key, parent->key)) ? parent->left = elem
                                    : parent->right = elem;
      return elem;
    }
    Node *result = nullptr;
    if (Compare()(key, elem->key)) {
      result = insert_node(elem->left, key, value, elem);
    } else {
      result = insert_node(elem->right, key, value, elem);
    }
    balance(elem);
    return result;
  }

  Node *delete_min(Node *elem) {
    if (!elem->left) return elem->right;
    elem->left = delete_min(elem->left);
    balance(elem);
    return elem;
  }

  void copy_node(Node *elem) {
    if (elem) {
      copy_node(elem->left);
      insert_by_key(elem->key, elem->value);
      copy_node(elem->right);
    }
  }

  void clean_node(Node *elem) {
    if (elem) {
      clean_node(elem->left);
      clean_node(elem->right);
      destroy_node(elem);
      --size_;
    }
  }

  Node *create_node(key_type key, value_type value) {
    Node *node = node_allocator_traits::allocate(alloc_, 1);
    node_allocator_traits::construct(alloc_, node, key, value);
    return node;
  }

  void destroy_node(Node *elem) {
    node_allocator_traits::destroy(alloc_, elem);
    node_allocator_traits::deallocate(alloc_, elem, 1);
  }

  // вспомогательные функции для функций наследников и их итераторов
 protected:
  Node *find_by_key(const key_type &key) const {
    return find_node(this->root_, key);
  }

  Node *insert_by_key(key_type key, value_type value) {
    Node *created;
    try {
      created = insert_node(root_, key, value, fake_);
      // LCOV_EXCL_START
    } catch (...) {
      created = nullptr;
    }
    // LCOV_EXCL_STOP
    return created;
  }

  void remove_node(Node *elem) {
    Node *left = elem->left;
    Node *right = elem->right;
    Node *parent = elem->parent;
    (Compare()(elem->key, parent->key) || parent == fake_)
        ? parent->left = nullptr
        : parent->right = nullptr;
    if (parent == fake_) root_ = nullptr;
    destroy_node(elem);
    --size_;
    if (!right && left) {
      left->parent = parent;
      parent->left = left;
      if (parent == fake_) root_ = left;
    } else if (!left && right) {
      right->parent = parent;
      parent->right = right;
      if (parent == fake_) root_ = right;
    } else if (left && right) {
      Node *min = get_min(right);
      min->right = delete_min(right);
      min->left = left;
      min->left->parent = min;
      min->parent = parent;
      if (parent == fake_)
        root_ = min;
      else
        (Compare()(min->key, parent->key)) ? parent->left = min
                                           : parent->right = min;
      balance(min);
    }
  }

  Node *increment_node(Node *elem) {
    if (elem->right) {
      return get_min(elem->right);
    } else {
      if (elem->parent->left == elem) return elem->parent;
      if (elem->parent->parent && elem->key < elem->parent->parent->key)
        return elem->parent->parent;
      return fake_;
    }
  }

  Node *decrement_node(Node *elem) {
    if (elem->left) return get_max(elem->left);
    if (elem->parent->right == elem) return elem->parent;
    if (elem->parent->parent && elem->key > elem->parent->parent->key)
      return elem->parent->parent;
    return fake_;
  }

  Node *find_lower_bound(const key_type &key) {
    if (!root_ || key > get_max(root_)->key ||
        (size_ == 1 && Compare()(root_->key, key)))
      return nullptr;
    if (key == root_->key || (size_ == 1 && Compare()(key, root_->key)))
      return root_;
    Node *found =
        get_min((Compare()(key, root_->key)) ? root_->left : root_->right);
    while (found && Compare()(found->key, key)) {
      found = increment_node(found);
    }
    return found;
  }

  Node *find_upper_bound(const key_type &key) {
    Node *found = find_lower_bound(key);
    if (found && found->key == key) return increment_node(found);
    return found;
  }

  Node *get_begin() const { return (root_) ? get_min(root_) : fake_; }

  // Общие функции для наследников
 public:
  inline bool empty() const noexcept { return size_ == 0 ? true : false; }

  inline size_type size() const noexcept { return size_; }

  constexpr size_type max_size() const {
    return std::numeric_limits<difference_type>::max() / (3 * sizeof(void *));
  }

  void clear() noexcept {
    if (size_ && root_) clean_node(root_);
    size_ = 0;
    root_ = nullptr;
  }

  virtual size_type count(const key_type &key) const {
    return contains(key) ? 1 : 0;
  }

  bool contains(const key_type &key) const {
    const Node *found = find_node(this->root_, key);
    return (found == nullptr) ? false : true;
  }

  virtual size_type erase(const key_type &key) {
    Node *found = find_node(root_, key);
    if (found != nullptr) {
      remove_node(found);
      return 1;
    } else
      return 0;
  }

  void swap(BinaryTree &other) noexcept {
    std::swap(this->size_, other.size_);
    std::swap(this->root_, other.root_);
    std::swap(this->fake_, other.fake_);
  }

 protected:
  struct Node {
    key_type key;
    value_type value;
    Node *parent, *left, *right;
    unsigned char height;

    explicit Node(const key_type &k = key_type(), value_type v = value_type(),
                  Node *p = nullptr, Node *l = nullptr, Node *r = nullptr)
        : key{k}, value{v}, parent{p}, left{l}, right{r}, height{1} {}
  };
};
}  // namespace s21

#endif