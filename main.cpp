#include "lib/Allocator.h"

#include <chrono>

int main() {
    auto pool1 = Pool(2, 8);
    auto pool2 = Pool(2, 60);
    auto pool3 = Pool(2, 240);
    auto pool4 = Pool(2, 2000);
    auto pool5 = Pool(2, 20000);
    auto pool6 = Pool(2, 100000);
    auto pool7 = Pool(2, 500000);
    auto pool8 = Pool(2, 5000000);
    auto pool9 = Pool(2, 20000000);

    std::vector<Pool*> pools = {&pool1, &pool2, &pool3, &pool4, &pool5, &pool6, &pool7, &pool8, &pool9};
    Allocator<int> allocator(pools);

    std::vector<int, Allocator<int>> my_vector(allocator);
    std::vector<int> stl_vector;

    const int k_max_elements = 1000000;
    const int teg = 10;

    for (int i = 1; i < k_max_elements; i *= 2) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int k = 0; k < i; k++) {
            my_vector.push_back(teg);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto my_allocator_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (int k = 0; k < i; k++) {
            stl_vector.push_back(teg);
        }
        end = std::chrono::high_resolution_clock::now();

        auto stl_allocator_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        std::cout << i << ' ' << my_allocator_time << ' ' << stl_allocator_time << '\n';

        my_vector.clear();
        stl_vector.clear();
    }

    return 0;
}
