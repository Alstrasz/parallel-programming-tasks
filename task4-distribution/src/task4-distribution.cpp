#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <deque>
#include <string>
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <iomanip>
#include <memory>
#include <cmath>
#include <openssl/md5.h>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

using namespace std;
 
void simulate(int64_t iterations, vector<int64_t> * const ret, mutex * const ret_edit_mutex) {
    // cout << iterations << '\n';
    boost::random::uniform_int_distribution dist(0, 1);
    boost::random::mt19937 gen(random());
    
    int32_t n = ret->size();
    vector<int64_t> acc(n, 0);

    for ( int32_t i = 0; i < iterations; i++ ) {
        int32_t pos = 0;
        for ( int32_t j = 0; j < n - 1; j++ ) {
            if ( dist(gen) < 1 ) {
                pos ++;
            }
        }
        acc[pos]++;
    }

    ret_edit_mutex->lock();
    for ( int32_t i = 0; i < n; i++ ) {
        (*ret)[i] += acc[i];
    }
    ret_edit_mutex->unlock();
}
 
vector<int64_t> spawn_threads(int32_t number_of_threads, int32_t size, int64_t iterations ) {
    deque<thread*> threads;
 
    vector<int64_t> ret(size, 0);
    mutex ret_edit_mutex;
    
    int64_t max_code = 1;
 
    for (int64_t i = 0; i < number_of_threads; i++) {
        
        threads.push_back(
            new thread(
                simulate,
                iterations / number_of_threads,
                &ret,
                &ret_edit_mutex
            )
        );
    }
 
    while (threads.size()) {
        thread* thread_ptr = threads.front();
        threads.pop_front();
        thread_ptr->join();
        delete thread_ptr;
    }
 
    return ret;
}
 
void benchmark(vector<int32_t>& number_of_threads, int32_t size, int64_t iterations, int32_t runs_per_num) {
    cout << setfill(' ') << setw(12) << "Thread count";
    cout << resetiosflags(ios_base::basefield);
    cout << " | ";
 
    cout << setfill(' ') << setw(12) << "Duration ms";
    cout << resetiosflags(ios_base::basefield);
    cout << '\n';

    vector<int64_t> value;
 
    for (int32_t num : number_of_threads) {
        double time_diff = 0;
 
        for (int32_t i = 0; i < runs_per_num; i++) {
            auto start = chrono::steady_clock::now();
 
            value = spawn_threads(num, size, iterations);
 
            auto end = chrono::steady_clock::now();
 
            time_diff += chrono::duration<double, milli>(end - start).count();
        }
        time_diff /= runs_per_num;
 
        cout << setfill(' ') << setw(12) << num;
        cout << resetiosflags(ios_base::basefield);
        cout << " | ";

        cout << setfill(' ') << setw(12) << time_diff;
        cout << resetiosflags(ios_base::basefield);
        cout << '\n';
    }

    int64_t max = 0;
    for ( int32_t i = 0; i < value.size(); i++ ) {
        max = max > value[i] ? max : value[i];
    }

    max /= 40;

    for (int32_t i = 0; i < value.size(); i++ ) {
        for (int32_t j = 0; j < value[i] / max; j++ ) {
            cout << '#';
        }
        cout << '\n';
    }
}
 
int main( int argc, char **argv ) {
    vector<int32_t> count{ 1, 2, 4, 8, 12, 16, 32, 64 };

    string hash;

    if ( argc > 1 ) {
        hash = argv[1];
    } else {
        hash = "abcd";
    }
 
    vector<char> alphabet;
 
    for (char i = 'a'; i <= 'z'; i++) {
        alphabet.push_back(i);
    }

    int32_t str_max_len = 5;
    
    benchmark(count, 11, 100000000, 1);

    return 0;
}
