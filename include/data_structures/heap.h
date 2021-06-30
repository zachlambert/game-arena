#ifndef DATA_STRUCTURES_HEAP_H
#define DATA_STRUCTURES_HEAP_H

#include <vector>
#include <stack>

template <typename T>
class Heap {
public:
    struct Node {
        bool available; // Nothing allocated below it
        bool allocated;
        int capacity;
        int index;
        int left;
        int right;
        int parent;
        Node(): available(true), allocated(false), capacity(1), index(-1), left(-1), right(-1), parent(-1) {}
    };

    int root_index;
    std::vector<Node> nodes;
    std::vector<T> data;

    Heap(): root_index(0) {
        nodes.push_back({true, false, 1, 0, -1, -1, -1});
        data.resize(1);
    }

    // n must be a multiple of 2
    void allocate(int n, int &start) {
        if (n > nodes[root_index].capacity) resize(2*n);

        std::stack<int> search, path;
        search.push(nodes[root_index]);
        while (!search.empty()) {
            int current = search.top();
            path.push(current);
            search.pop();

            // Skip if allocated
            if (nodes[current].allocated) continue;

            // If correct capacity and is available, assign to this node
            if (nodes[current].capacity == n) {
                if (!nodes[current].available) continue;
                nodes[current].allocated = true;
                start = nodes[current].index;
                return;
            }

            // At this point, guaranteed to allocate below the current node
            nodes[current].available = false;

            search.push(nodes[current].right);
            search.push(nodes[current].left);
        }
        // Can't find anything, resize
        resize(nodes[root_index].capacity * 2);
        allocate(n, start);
        // Bit of recursion, but should be guaranteed to allocate on next
        // call of allocate, so seems fine
    }

    void resize(int n) {
        data.resize(n);
        while (nodes[root_index].capacity != n) {
            Node new_root;
            new_root.index = 0;
            new_root.left = root_index;
            new_root.capacity = nodes[root_index].capacity * 2;
            nodes[root_index].parent = nodes.size();
            root_index = nodes.size();
            nodes.push_back(new_root);

            // Now create all child nodes on the right
            Node root_right;
            root_right.index = nodes[root_index].capacity/2;
            root_right.capacity = nodes[root_index].capacity/2;
            root_right.parent = root_index;

            std::stack<int> pending;
            pending.push(nodes.size());
            nodes.push_back(root_right);
            int current;
            while (!pending.empty()) {
                current = pending.top();
                pending.pop();
                if (nodes[current].capacity == 1) continue;
                Node left, right;
                left.capacity = nodes[current].capacity/2;
                right.capacity = nodes[current].capacity/2;
                left.index = nodes[current].index;
                right.capacity = nodes[current].index + nodes[current].capacity/2;
                left.parent = current;
                right.parent = current;
                nodes[current].left = nodes.size();
                nodes.push_back(left);
                nodes[current].right = nodes.size();
                nodes.push_back(right);
                pending.push(nodes[current].right);
                pending.push(nodes[current].left);
            }
        }
    }

    void deallocate(int n, int start)
    {
        int current = nodes[root_index];
        while (nodes[current].capacity != n) {
            if (start < nodes[current].index + nodes[current].capacity/2) {
                current = nodes[current].left;
            } else {
                current = nodes[current].right;
            }
        }
        nodes[current].allocated = false;

        // Now need to update the available flags for parent nodes
        while (true) {
            current = nodes[current].parent;
            if (current == -1) break;
            if (!nodes[nodes[current].left].available) break;
            if (!nodes[nodes[current].right].available) break;
            nodes[current].available == true;
        }
    }
};

#endif
