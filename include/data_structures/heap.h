#ifndef DATA_STRUCTURES_HEAP_H
#define DATA_STRUCTURES_HEAP_H

#include <vector>
#include <stack>

template <typename T>
class Heap {
public:
    struct Node {
        bool allocated;
        int capacity;
        int index;
        int left;
        int right;
        Node(): allocated(false), capacity(1), index(-1), left(-1), right(-1) {}
    };

    int root_index;
    std::vector<Node> nodes;
    std::vector<T> data;

    Heap(): root_index(0) {
        nodes.push_back({false, 1, 0, -1, -1});
        data.resize(1);
    }

    // n must be a multiple of 2
    void allocate(int n, int &start) {
        if (n > nodes[root_index].capacity) resize(2*n);

        std::stack<int> search;
        search.push(nodes[root_index]);
        while (!search.empty()) {
            int current = search.top();
            search.pop();
            if (nodes[current].allocated) continue;
            if (nodes[current].capacity == n) {
                nodes[current].allocated = true;
                start = nodes[current].index;
                return;
            }

            // Can't allocate to this node. Search to  lower nodes,
            // or stop this search path if at the bottom (capacity == 1)

            if (nodes[current].capacity == 1) continue;

            // If not at bottom, add new nodes if they haven't been created yet
            if (nodes[current].left == -1) {
                // Add node to the left
                Node node;
                node.index = nodes[current].index;
                node.capacity = nodes[current].capacity/2;
                nodes[current].left = nodes.size();
                nodes.push_back(node);
            }
            if (nodes[current].right == -1) {
                // Add node to the right
                Node node;
                node.index = nodes[current].index + nodes[current].capacity/2;
                node.capacity = nodes[current].capacity/2;
                nodes[current].left = nodes.size();
                nodes.push_back(node);
            }
            search.push(nodes[current].right);
            search.push(nodes[current].left);
            continue;
        }
        // Can't find anything, resize
        resize(nodes[root_index].capacity * 2);
        allocate(n, start);
        // Bit of recursion, but should be guaranteed to allocate on next
        // call of allocate, so seems fine
    }

    void resize(int n) {
        data.resize(n);
        Node new_root;
        new_root.index = 0;
        new_root.left = root_index;
        root_index = nodes.size();
        nodes.push_back(new_root);
    }
};

#endif
