#include <iostream>
#include <algorithm>
#include <cmath>
#include <climits>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <memory>
#include <fstream>
#include <sstream>
#include "prime.h"

using namespace std;

int32_t get_value ( shared_ptr<int32_t> inc, int32_t max_inc, shared_ptr<mutex> inc_mutex ) {
    int32_t val;
    inc_mutex->lock();
    if ( *inc < max_inc ) {
        
        *inc += 6 - ((*inc) % 3) * 2;
    }
    val = *inc;
    inc_mutex->unlock();

    return val;
}

bool output_value (
    int32_t val,
    bool is_final,
    int32_t id,
    shared_ptr<vector<int32_t>> entries_count, 
    shared_ptr<priority_queue<pair<int32_t, int32_t>>> output_etries, 
    shared_ptr<ostream> result_stream,
    ostream & thread_stream,
    shared_ptr<mutex> output_mutex
) {
    output_mutex->lock();

    if ( is_final ) {
        (*entries_count)[id] = INT_MAX;
    } else {
        (*entries_count)[id]++;
        output_etries->push({-val, id});
    }

    bool check = true;

    for (auto v: *entries_count) {
        if ( v == 0 ) {
            check = false;
        }
    }

    if ( check ) {
        while ( output_etries->size() ) {
            auto top = output_etries->top();
            output_etries->pop();

            (*result_stream) << -top.first << ' ';
            thread_stream << -top.first << ' ';

            (*entries_count)[top.second] --;
            if ( (*entries_count)[top.second] == 0 ) {
                break;
            }
        }
    }

    check = output_etries->empty();
    output_mutex->unlock();
    return check;
}

void thread_func(
    int32_t max_inc,
    shared_ptr<int32_t> inc,
    shared_ptr<vector<int32_t>> entries_count, 
    shared_ptr<priority_queue<pair<int32_t, int32_t>>> output_etries, 
    shared_ptr<ostream> result_stream,
    shared_ptr<mutex> inc_mutex,
    shared_ptr<mutex> output_mutex
) {
    output_mutex->lock();
    int32_t id = entries_count->size();
    entries_count->push_back(0);
    output_mutex->unlock();

    stringstream str;
    str << "thread" << id << ".txt";
    ofstream fout(str.str(), ofstream::ate);

    if (id == 0) {
        output_value(2, false, id, entries_count, output_etries, result_stream, fout, output_mutex);
        output_value(3, false, id, entries_count, output_etries, result_stream, fout, output_mutex);
    }


    while ( true ) {
        int32_t val = get_value(inc, max_inc, inc_mutex);
        if ( val > max_inc ) {
            bool check = output_value(0, true, id, entries_count, output_etries, result_stream, fout, output_mutex);
            if ( check ) {
                break;
            }
        }

        if ( is_prime( val ) ) {
            output_value(val, false, id, entries_count, output_etries, result_stream, fout, output_mutex);
        }
    }
}

void spawn_threads(int32_t thread_count, int32_t max_inc) {
    shared_ptr<int32_t> inc(new int);
    *inc = 1;
    shared_ptr<vector<int32_t>> entries_count(new vector<int32_t>);
    shared_ptr<priority_queue<pair<int32_t, int32_t>>> output_etries(new priority_queue<pair<int32_t, int32_t>>); 
    shared_ptr<mutex> inc_mutex(new mutex);
    shared_ptr<mutex> output_mutex(new mutex);
    shared_ptr<ostream> result_stream(new ofstream("result.txt"));

    deque<thread *> threads;

    for (int32_t i = 0; i < thread_count; i++) {
        auto inc_m = inc;
        auto entries_count_m = entries_count;
        auto output_etries_m = output_etries;
        auto result_stream_m = result_stream;
        auto inc_mutex_m = inc_mutex;
        auto output_mutex_m = output_mutex;
        
        threads.push_back( new thread (
            thread_func,
            max_inc,
            move(inc_m),
            move(entries_count_m),
            move(output_etries_m),
            move(result_stream_m),
            move(inc_mutex_m),
            move(output_mutex_m)
        ) );
    }

    while ( threads.size() ) {
        thread * thread_ptr = threads.front();
        threads.pop_front();
        thread_ptr->join();
        delete thread_ptr;
    }
}

int32_t main( int argc, char **argv ) {
    int32_t thread_count = 2;
    int32_t max_inc = 1000000;

    if ( argc >= 2 ) {
        int32_t num = atoi(argv[1]);
        if ( num > 0 ) {
            thread_count = num;
        }
    }

    if ( argc >= 3 ) {
        int32_t num = atoi(argv[2]);
        if ( num > 0 ) {
            max_inc = num;
        }
    }

    cout << "Thread count: " << thread_count << '\n';

    cout << "Max value: " << max_inc << '\n';

    auto start = chrono::steady_clock::now();

    spawn_threads( thread_count, max_inc );

    auto end = chrono::steady_clock::now();

    double time_diff = chrono::duration<double, milli> ( end - start ).count();

    cout << "Execution time: " << time_diff << '\n';
}
