#include "sorts.h"
#include <iostream>

static void print(const std::vector<int>& v) {
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";
}

int main() {
    std::vector<int> data1 = {5, 3, 8, 4, 2};
    std::vector<int> data2 = data1;
    std::vector<int> data3 = data1;

    bubble_sort(data1);
    insertion_sort(data2);
    quick_sort(data3);

    print(data1);
    print(data2);
    print(data3);

    return 0;
}
