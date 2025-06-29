#include <iostream>
#include <chrono>
#include <random>
#include "attachment.hpp"
#include "attachment_any.hpp"

using namespace std;
using namespace ken;
using namespace std::chrono;

constexpr int TEST_SIZE = 10000;
constexpr int LOOP_COUNT = 10;

template<typename T>
void run_performance_test(const string& name) {
    T container;
    vector<string> names;
    vector<int> values;

    // Generate test data
    mt19937 gen(42);
    uniform_int_distribution<int> dist(1, 1000);
    for (int i = 0; i < TEST_SIZE; ++i) {
        names.push_back("item_" + to_string(i));
        values.push_back(dist(gen));
    }

    // Insertion test
    auto start = high_resolution_clock::now();
    for (int i = 0; i < TEST_SIZE; ++i) {
        container.set(names[i], make_shared<int>(values[i]));
    }
    auto end = high_resolution_clock::now();
    auto insert_time = duration_cast<microseconds>(end - start).count();

    // Lookup test
    start = high_resolution_clock::now();
    for (int l = 0; l < LOOP_COUNT; ++l) {
        for (int i = 0; i < TEST_SIZE; ++i) {
            auto ptr = container.template get<int>(names[i]);
            if (ptr && *ptr != values[i]) {
                cerr << "Data mismatch! *ptr:" << *ptr << " values[i]:" << values[i] << endl;
            }
        }
    }
    end = high_resolution_clock::now();
    auto lookup_time = duration_cast<microseconds>(end - start).count();

    // Type check test
    start = high_resolution_clock::now();
    for (int l = 0; l < LOOP_COUNT; ++l) {
        for (int i = 0; i < TEST_SIZE; ++i) {
            if (!container.template is<int>(names[i])) {
                cerr << "Type check failed!" << endl;
            }
        }
    }
    end = high_resolution_clock::now();
    auto type_check_time = duration_cast<microseconds>(end - start).count();

    cout << name << " performance results:" << endl;
    cout << "  Insertion: " << insert_time << " μs" << endl;
    cout << "  Lookup: " << lookup_time / LOOP_COUNT << " μs per iteration" << endl;
    cout << "  Type check: " << type_check_time / LOOP_COUNT << " μs per iteration" << endl;
    cout << endl;
}

int main() {
    run_performance_test<AttachmentSet>("AttachmentSet");
    run_performance_test<AnyAttachmentSet>("AnyAttachmentSet");
    return 0;
}

// AttachmentSet performance results:
//   Insertion: 1585972 μs
//   Lookup: 1586835 μs per iteration
//   Type check: 1589185 μs per iteration

// AnyAttachmentSet performance results:
//   Insertion: 1595721 μs
//   Lookup: 1585682 μs per iteration
//   Type check: 1591934 μs per iteration