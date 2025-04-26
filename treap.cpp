#include <iostream>
#include <random>


template <typename T>
class Treap {
    struct Node {
        T value;
        int priority;
        int count;
        Node *left;
        Node *right;

        Node(const T &value, int priority) : value(value), priority(priority), count(1), left(nullptr), right(nullptr) {}
    };

    Node *root;
    unsigned size_;
    std::mt19937 rng;

    std::pair<Node*, Node*> split(Node *node, const T &value) {
        if (!node) return {nullptr, nullptr};

        if (node->value < value) {
            auto [right_left, right_right] = split(node->right, value);
            node->right = right_left;
            return {node, right_right};
        } else {
            auto [left_left, left_right] = split(node->left, value);
            node->left = left_right;
            return {left_left, node};
        }
    }

    Node *merge(Node *left, Node *right) {
        if (!left) return right;
        if (!right) return left;

        if (left->priority > right->priority) {
            left->right = merge(left->right, right);
            return left;
        } else {
            right->left = merge(left, right->left);
            return right;
        }
    }

    int generate_priority() {
        std::uniform_int_distribution<int> dist(0, INT_MAX);
        return dist(rng);
    }

    void print_(const Node *node, int indent = 0) const {
        if (node) {
            print_(node->right, indent + 4);
            std::cout << std::string(indent, ' ') << node->value << " (" << node->priority << ", count: " << node->count << ")" << std::endl;
            print_(node->left, indent + 4);
        }
    }

    void destroy(Node *node) {
        if (node) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

public:
    Treap() : root(nullptr), size_(0), rng(std::random_device{}()) {}

    ~Treap() {
        destroy(root);
    }

    void insert(const T &value) {
        auto [less, greater_equal] = split(root, value);

        if (greater_equal && greater_equal->value == value) {
            greater_equal->count++;
            root = merge(less, greater_equal);
        } else {
            Node *new_node = new Node(value, generate_priority());
            root = merge(merge(less, new_node), greater_equal);
        }

        ++size_;
    }

    void remove(const T &value) {
        auto [less, greater_equal] = split(root, value);

        if (greater_equal && greater_equal->value == value) {
            if (greater_equal->count > 1) {
                greater_equal->count--;
                root = merge(less, greater_equal);
            } else {
                auto [equal, greater] = split(greater_equal, value + static_cast<T>(1));
                delete equal;
                root = merge(less, greater);
            }
        } else {
            root = merge(less, greater_equal);
        }

        --size_;
    }

    int count(const T &value) const {
        Node *current = root;
        while (current) {
            if (value < current->value) {
                current = current->left;
            } else if (value > current->value) {
                current = current->right;
            } else {
                return current->count;
            }
        }
        return 0;
    }

    int size() const {
        return size_;
    }

    void print() const {
        print_(root);
        std::cout << std::endl;
    }
};

int main() {
    Treap<int> tree;

    tree.insert(5);
    tree.insert(2);
    tree.insert(7);
    tree.insert(5);
    tree.insert(1);
    tree.insert(5);
    tree.insert(8);

    tree.print();

    std::cout << "\nCount of 5: " << tree.count(5) << std::endl;
    std::cout << "Size: " << tree.size() << std::endl;

    tree.remove(5);
    tree.print();
    std::cout << "\nCount of 5: " << tree.count(5) << std::endl;
    std::cout << "Size: " << tree.size() << std::endl;

    tree.remove(5);
    tree.remove(5);
    tree.print();

    std::cout << "\nCount of 5: " << tree.count(5) << std::endl;
    std::cout << "Size of the tree: " << tree.size() << std::endl;

    return 0;
}