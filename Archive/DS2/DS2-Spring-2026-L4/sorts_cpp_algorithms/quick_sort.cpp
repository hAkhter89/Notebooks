#include "sorts.h"
#include <algorithm>

static void quick_sort_impl(std::vector<int>& v, int left, int right) {
    //add quick sort code here
}

void quick_sort(std::vector<int>& v) {
    if (!v.empty()) {
        quick_sort_impl(v, 0, static_cast<int>(v.size() - 1));
    }
}
