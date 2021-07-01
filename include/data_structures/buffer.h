#ifndef DATA_STRUCTURES_BUFFER_H
#define DATA_STRUCTURES_BUFFER_H

#include <array>

template <typename T, int N>
struct Buffer {
    std::array<T, N> data;
    int tail;

    T &operator[](int index){ return data[index]; }
    const T &operator[](int index)const{ return data[index]; }

    bool append(T element) {
        if (tail == N) return false;
        data[tail] = element;
        tail++;
        return true;
    }
    bool reserve(int num) {
        if (tail + num >= N) return false;
        tail+=num;
        return true;
    }
    void remove(int index) {
        tail--;
        if (index != tail) {
            data[index] = data[tail];
        }
    }
    bool full(){ return tail == N; }

    Buffer(): tail(0) {}
};

#endif
