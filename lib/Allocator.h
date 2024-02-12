#pragma once

#include "iostream"
#include "vector"
#include "queue"

class Pool {
    void* pool_;
    void* first_free_chunk_ = nullptr;
    std::queue<void*> free_chunks_;
    size_t chunk_size_;
    size_t amount_of_chunk_;

    [[nodiscard]] char* getStartOfPool() const {
        return static_cast<char*>(pool_);
    }

    [[nodiscard]] char* getEndOfPool() const {
        return static_cast<char*>(pool_) + chunk_size_ * amount_of_chunk_;
    }

public:
    Pool(size_t amount_chunk, size_t chunk_size) {
        if (chunk_size <= 0 || amount_chunk <= 0) {
            throw std::bad_alloc();
        }

        pool_ = first_free_chunk_ = malloc(amount_chunk * chunk_size);
        chunk_size_ = chunk_size;
        amount_of_chunk_ = amount_chunk;
    }

    ~Pool() {
        if (pool_ != nullptr) {
            free(pool_);
        }
    }

    [[nodiscard]] size_t GetChunkSize() const {
        return chunk_size_;
    }

    void* getChunk() {
        char* temp_pointer = static_cast<char*>(first_free_chunk_) + chunk_size_;

        if (!free_chunks_.empty()) {
            void* temp = free_chunks_.front();
            free_chunks_.pop();

            return temp;
        } else if (temp_pointer <= getEndOfPool()) {
            void* return_pointer = first_free_chunk_;
            first_free_chunk_ = static_cast<void*>(temp_pointer);

            return return_pointer;
        } else {
            // if pool haven't free memory
            return nullptr;
        }
    }

    bool deleteChunk(void* pointer) {
        char* this_pointer = static_cast<char*>(pointer);

        if (getStartOfPool() <= this_pointer && this_pointer <= getEndOfPool()) {
            // if pointer isn't a start of chunk
            if ((this_pointer - getStartOfPool()) % chunk_size_ != 0) {
                throw std::bad_alloc();
            }

            free_chunks_.push(pointer);

            return true;
        }

        return false;
    }
};

bool gradle (const Pool* lhs, const Pool* rhs) {
    return lhs->GetChunkSize() < rhs->GetChunkSize();
}

template<typename T>
class Allocator {
    std::vector<Pool*>& pools_;

    size_t findPoolBySize(size_t size) {
        if (size <= pools_[0]->GetChunkSize()) {
            return 0;
        }

        size_t left = 0;
        size_t right = pools_.size() - 1;
        size_t pool_for_allocate = -1;

        while (left <= right) {
            size_t mid = left + (right - left) / 2;

            if (pools_[mid]->GetChunkSize() >= size) {
                pool_for_allocate = mid;
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        if (pool_for_allocate == -1) {
            throw std::bad_alloc();
        }

        return pool_for_allocate;
    }
public:
    using value_type = T;

    explicit Allocator(std::vector<Pool*>& pools) : pools_(pools) {
        if (pools_.empty()) {
            throw std::bad_alloc();
        }

        std::sort(pools_.begin(), pools_.end(), gradle);
    }

    Allocator(const Allocator<T>& other) : pools_(other.pools_) {}

    bool operator== (const Allocator& rhs) {
        return std::equal(pools_.begin(), pools_.end(),
                          rhs.pools_.begin(), rhs.pools_.end());
    }

    bool operator!= (const Allocator& rhs) {
        return !std::equal(pools_.begin(), pools_.end(),
                           rhs.pools_.begin(), rhs.pools_.end());
    }

    T* allocate(size_t n) {
        if (pools_.empty()) {
            throw std::bad_alloc();
        }

        size_t pool_for_allocate = findPoolBySize(n * sizeof(T));
        void* pointer = nullptr;

        for (size_t i = pool_for_allocate; i < pools_.size(); i++) {
            pointer = pools_[i]->getChunk();

            if (pointer != nullptr) {
                break;
            }
        }

        // allocator don't have free memory
        if (pointer == nullptr) {
            throw std::bad_alloc();
        }

        return static_cast<T*>(pointer);
    }

    void deallocate(void* pointer, size_t size) {
        size_t pool_for_deallocate = findPoolBySize(size);

        for (size_t i = pool_for_deallocate; i < pools_.size(); i++) {
            if (pools_[i]->deleteChunk(pointer)) {
                return;
            }
        }

        throw std::bad_alloc();
    }
};

