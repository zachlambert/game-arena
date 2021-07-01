#ifndef DATA_STRUCTURES_BUFFER_H
#define DATA_STRUCTURES_BUFFER_H

#include <vector>

template <typename T>
struct Buffer {
    std::vector<T> data;
    int tail;

    T &operator[](int index){ return data[index]; }
    const T &operator[](int index)const{ return data[index]; }

    void append(T element) {
        if (tail == data.size()) {
            data.push_back(element);
        } else {
            data[tail] = element;
        }
        tail++;
    }
    void reserve(int num) {
        if (tail + num >= data.size()) data.resize(data.size() + num);
        tail+=num;
    }
    void remove(int index) {
        tail--;
        if (index != tail) {
            data[index] = data[tail];
        }
    }

    Buffer(): tail(0) {}
};

#endif
