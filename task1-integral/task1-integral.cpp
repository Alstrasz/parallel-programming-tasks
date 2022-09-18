#include <chrono>
#include <iostream>
#include <deque>
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <iomanip>

using namespace std;

void count_integral ( double from, double to, double step, function<double(double)> func, double *ret, mutex *ret_edit_mutex ) {
    double acc = 0;
    for ( ; from < to; from += step ) {
        acc += func( from ) * step;
    }
    ret_edit_mutex->lock();
    *ret += acc;
    ret_edit_mutex->unlock();
}

double integral_func ( double x ) {
    return x * x;
}

double spawn_threads ( int32_t number_of_threads, int32_t number_of_steps ) {
    deque<thread *> threads;

    double ret = 0;
    mutex ret_edit_mutex;

    for ( int32_t i = 0; i < number_of_threads; i ++ ) {
        threads.push_back(
            new thread( 
                count_integral, 
                1.0 * i / number_of_threads, 
                1.0 * ( i + 1 ) / number_of_threads,
                1.0 / number_of_steps, 
                integral_func,
                &ret,
                &ret_edit_mutex
            ) 
        );
    }

    while ( threads.size() ) {
        thread * thread_ptr = threads.front();
        threads.pop_front();
        thread_ptr->join();
        delete thread_ptr;
    }

    return ret;
}

void benchmark ( vector<int32_t> & number_of_threads, int32_t number_of_steps, int32_t runs_per_num ) {
    cout << std::setfill (' ') << std::setw(12) << "Thread count";
    cout << std::resetiosflags(std::_S_ios_fmtflags_max);
    cout << " | ";

    cout << std::setfill (' ') << std::setw(12) << "Value";
    cout << std::resetiosflags(std::_S_ios_fmtflags_max);
    cout << " | ";

    cout << std::setfill (' ') << std::setw(12) << "Duration ms";
    cout << std::resetiosflags(std::_S_ios_fmtflags_max);
    cout << '\n';

    for ( int32_t num: number_of_threads ) {
        double time_diff = 0;
        double value;

        for ( int32_t i = 0; i < runs_per_num; i++ ) {
            auto start = chrono::steady_clock::now();

            value = spawn_threads( num, number_of_steps );

            auto end = chrono::steady_clock::now();

            time_diff += chrono::duration<double, milli> ( end - start ).count();
        }
        time_diff /= runs_per_num;

        cout << std::setfill (' ') << std::setw(12) << num;
        cout << std::resetiosflags(std::_S_ios_fmtflags_max);
        cout << " | ";

        cout << std::setfill (' ') << std::setw(12) << value;
        cout << std::resetiosflags(std::_S_ios_fmtflags_max);
        cout << " | ";

        cout << std::setfill (' ') << std::setw(12) << time_diff;
        cout << std::resetiosflags(std::_S_ios_fmtflags_max);
        cout << '\n';
    }
}

int main () {
    vector<int32_t> count { 1, 2, 4, 8, 12, 16, 32, 64, 128 };

    benchmark(count, 1000000000, 10);

    return 0;
}
