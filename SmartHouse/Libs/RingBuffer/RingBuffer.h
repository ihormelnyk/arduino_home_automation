#ifndef RINGBUFFER_h
#define RINGBUFFER_h

#include <stdlib.h>
#include <inttypes.h>

template <typename T, uint16_t Size>
class RingBuffer {
    private:
        const uint16_t size;
        uint16_t count;
        T buf[Size];
        T* write_p;
        T* read_p;
        T* end_p;
    public:
        RingBuffer(): size(Size), write_p(buf), read_p(buf), end_p(buf + Size), count(0) {}

        T* push() {
            if (count >= size) {
                return NULL;
            }
            T* result = write_p++;
            count++;
            if (write_p == end_p) write_p = buf;
            return result;
        }

        T* pop() {
            if (count <= 0) {
                return NULL;
            }
            T* result = read_p++;
            count--;
            if (read_p == end_p) read_p = buf;
            return result;
        }

        uint16_t getCount() const {
            return count;
        }

        uint16_t getSize() const {
            return size;
        }

        bool isEmpty() const {
            return count <= 0;
        }

        bool isFull() const {
            return count >= size;
        }
};

#endif
