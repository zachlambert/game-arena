#include "data_structures/heap.h"

#include <iostream>

int main()
{
    Heap<int> heap;
    int a;

    // Allocate some things
    heap.allocate(1, a);
    std::cout << "a = " << a << std::endl;
    heap.allocate(2, a);
    std::cout << "a = " << a << std::endl;
    heap.allocate(1, a);
    std::cout << "a = " << a << std::endl;
    heap.allocate(2, a);
    std::cout << "a = " << a << std::endl;

    // Deallocate some things
    heap.deallocate(1, 0);
    heap.allocate(1, a);
    std::cout << "a = " << a << std::endl;

    heap.deallocate(1, 0);
    heap.deallocate(1, 1);
    heap.deallocate(2, 2);
    heap.allocate(4, a);
    std::cout << "a = " << a << std::endl;

    return 0;
}
