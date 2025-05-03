#include <iostream>
#include <optional>

template <typename T>
class AVLTree {
    struct Node {
        T value;
        Node *left;
        Node *right;
        int height;
        int size;

        Node(const T &value) : value(value), left(nullptr), right(nullptr), height(1), size(1) {}
    };

    Node *root;

    int height(Node *node) {
        return (node == nullptr) ? 0 : node->height;
    }

    int size(Node *node) {
        return (node == nullptr) ? 0 : node->size;
    }

    Node* right_rotate(Node *y) {
        Node *x = y->left;
        Node *T2 = x->right;

        x->right = y;
        y->left = T2;

        int left_height = height(y->left), right_height = height(y->right);
        y->height = 1 + (left_height > right_height ? left_height : right_height);
        left_height = height(x->left), right_height = height(x->right);
        x->height = 1 + (left_height > right_height ? left_height : right_height);

        y->size = size(y->left) + size(y->right) + 1;
        x->size = size(x->left) + size(x->right) + 1;

        return x;
    }

    Node* left_rotate(Node *x) {
        Node *y = x->right;
        Node *T2 = y->left;

        y->left = x;
        x->right = T2;

        int left_height = height(x->left), right_height = height(x->right);
        x->height = 1 + (left_height > right_height ? left_height : right_height);
        left_height = height(y->left), right_height = height(y->right);
        y->height = 1 + (left_height > right_height ? left_height : right_height);

        x->size = size(x->left) + size(x->right) + 1;
        y->size = size(y->left) + size(y->right) + 1;

        return y;
    }

    Node* insert_helper(Node *node, const T &value) {
        if (node == nullptr) return new Node(value);

        if (value < node->value)
            node->left = insert_helper(node->left, value);
        else if (value > node->value)
            node->right = insert_helper(node->right, value);
        else 
            return node;

        int left_height = height(node->left), right_height = height(node->right);
        node->height = 1 + (left_height > right_height ? left_height : right_height);

        node->size = size(node->left) + size(node->right) + 1;

        int balance = height(node->left) - height(node->right);


        if (balance > 1 && value < node->left->value) return right_rotate(node);

        if (balance < -1 && value > node->right->value) return left_rotate(node);

        if (balance > 1 && value > node->left->value) {
            node->left = left_rotate(node->left);
            return right_rotate(node);
        }

        if (balance < -1 && value < node->right->value) {
            node->right = right_rotate(node->right);
            return left_rotate(node);
        }

        return node;
    }

    Node* delete_helper(Node *node, const T &value) {
        if (node == nullptr) return nullptr;

        if (value < node->value)
            node->left = delete_helper(node->left, value);
        else if (value > node->value)
            node->right = delete_helper(node->right, value);
        else {
            if ((node->left == nullptr) || (node->right == nullptr)) {
                Node *temp = (node->left) ? node->left : node->right;

                if (temp == nullptr) {
                    temp = node;
                    node = nullptr;
                } else *node = *temp;

                delete temp;
            } else {
                Node *min_node = node->right;
                while (min_node->left != nullptr) min_node = min_node->left;
                node->value = min_node->value;
                node->right = delete_helper(node->right, min_node->value);
            }
        }

        if (node == nullptr) return nullptr;

        int left_height = height(node->left), right_height = height(node->right);
        node->height = 1 + (left_height > right_height ? left_height : right_height);

        node->size = size(node->left) + size(node->right) + 1;

        int balance = height(node->left) - height(node->right);

        if (balance > 1) {
            if (((node->left == nullptr) ? 0 : height(node->left->left) - height(node->left->right)) >= 0)
                return right_rotate(node);
            else {
                node->left = left_rotate(node->left);
                return right_rotate(node);
            }
        }

        if (balance < -1) {
            if (((node->left == nullptr) ? 0 : height(node->left->left) - height(node->left->right)) <= 0)
                return left_rotate(node);
            else {
                node->right = right_rotate(node->right);
                return left_rotate(node);
            }
        }

        return node;
    }

    Node* kth_order_statistic_helper(Node *node, int k) {
        if (node == nullptr) return nullptr;

        int left_size = size(node->left);

        if (k == left_size + 1)
            return node;
        else if (k < left_size + 1)
            return kth_order_statistic_helper(node->left, k);
        else
            return kth_order_statistic_helper(node->right, k - left_size - 1);
    }

    void inorder_helper(Node *node) {
        if (node != nullptr) {
            inorder_helper(node->left);
            std::cout << node->value << " ";
            inorder_helper(node->right);
        }
    }

    void destroy(Node *node) {
        if (node != nullptr) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree() {
        destroy(root);
    }

    void insert(const T &value) {
        root = insert_helper(root, value);
    }

    void remove(const T &value) {
        root = delete_helper(root, value);
    }

    std::optional<T>  kth_order_statistic(int k) {
        if (k <= 0) {
            std::cout << "k must be greater than 0" << std::endl;
            return std::nullopt;
        }

        Node *result = kth_order_statistic_helper(root, k);
        if (result == nullptr) {
            std::cout << "k is out of range" << std::endl;
            return std::nullopt;
        }

        return result->value;
    }

    void inorder_traversal() {
        inorder_helper(root);
        std::cout << std::endl;
    }
};

int main() {
    AVLTree<int> tree;

    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);
    tree.insert(25);

    std::cout << "Inorder Traversal: ";
    tree.inorder_traversal();
    std::cout << "3rd smallest element: " << tree.kth_order_statistic(3).value() << std::endl;
    std::cout << "1st smallest element: " << tree.kth_order_statistic(1).value() << std::endl;
    std::cout << "6th smallest element: " << tree.kth_order_statistic(6).value() << std::endl;

    tree.remove(20);
    std::cout << "Inorder Traversal after deleting 20: ";
    tree.inorder_traversal();
    std::cout << "3rd smallest element 20: " << tree.kth_order_statistic(3).value() << std::endl;

    return 0;
}