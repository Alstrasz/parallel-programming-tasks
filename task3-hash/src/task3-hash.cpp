#include <chrono>
#include <cstdio>
#include <iostream>
#include <deque>
#include <string>
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <iomanip>
#include <memory>
#include <openssl/md5.h>
 
using namespace std;
 
string apply_hash ( const string& data ) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    char ret[MD5_DIGEST_LENGTH * 2 + 1];
    MD5( (const unsigned char *) data.c_str(), data.size(), (unsigned char *) hash );

    for ( int32_t i = 0; i < MD5_DIGEST_LENGTH; i++ ) {
        sprintf(ret + i * 2, "%02x", hash[i]);
    }
    ret[MD5_DIGEST_LENGTH * 2] = 0;

    return (char*)ret;
}

double check_hash(string code, const string* const hash) {
    return apply_hash(code) == *hash;
}
 
string construct_string(int64_t code, const vector<char>* const alphabet) {
    string acc;
    while (code > 0) {
        if (code % (alphabet->size() + 1) == 0) {
            break;
        }
 
        acc.push_back((*alphabet)[(code % (alphabet->size() + 1)) - 1]);
 
        code /= alphabet->size() + 1;
    }
    return acc;
}
 
void reverse_hash(int64_t from, int64_t to, const string * const hash, const vector<char> * const alphabet, int64_t * const ret, mutex * const ret_edit_mutex) {
    for (; from < to && !*ret; from++) {
        if (check_hash(construct_string(from, alphabet), hash)) {
            ret_edit_mutex->lock();
            *ret = from;
            ret_edit_mutex->unlock();
        }
    }
}
 
int64_t spawn_threads(int32_t number_of_threads, const vector<char> &alphabet, int32_t str_max_len, const string& hash ) {
    deque<thread*> threads;
 
    int64_t ret = 0;
    mutex ret_edit_mutex;
    
    int64_t max_code = 1;
 
    for (int32_t i = 0; i < str_max_len; i++) {
        max_code *= alphabet.size() + 1;
    }
    // cout << "-----" << number_of_threads << "-----\n"; 
    for (int64_t i = 0; i < number_of_threads; i++) {
        int32_t from = max_code * i / number_of_threads;
        int32_t to = max_code * (i + 1L) / number_of_threads;
        // cout << from << ' ' << to << '\n';
        threads.push_back(
            new thread(
                reverse_hash,
                max_code * i / number_of_threads,
                max_code * (i + 1L) / number_of_threads,
                &hash,
                &alphabet,
                &ret,
                &ret_edit_mutex
            )
        );
    }
    // cout << "---------";
 
    while (threads.size()) {
        thread* thread_ptr = threads.front();
        threads.pop_front();
        thread_ptr->join();
        delete thread_ptr;
    }
 
    return ret;
}
 
void benchmark(vector<int32_t>& number_of_threads, const vector<char>& alphabet, int32_t str_max_len, const string& hash, int32_t runs_per_num) {
    cout << setfill(' ') << setw(12) << "Thread count";
    cout << resetiosflags(ios_base::basefield);
    cout << " | ";
 
    cout << setfill(' ') << setw(12) << "Value";
    cout << resetiosflags(ios_base::basefield);
    cout << " | ";
 
    cout << setfill(' ') << setw(12) << "Duration ms";
    cout << resetiosflags(ios_base::basefield);
    cout << '\n';
 
    for (int32_t num : number_of_threads) {
        double time_diff = 0;
        int64_t value = 0;
 
        for (int32_t i = 0; i < runs_per_num; i++) {
            auto start = chrono::steady_clock::now();
 
            value = spawn_threads(num, alphabet, str_max_len, hash);
 
            auto end = chrono::steady_clock::now();
 
            time_diff += chrono::duration<double, milli>(end - start).count();
        }
        time_diff /= runs_per_num;
 
        cout << setfill(' ') << setw(12) << num;
        cout << resetiosflags(ios_base::basefield);
        cout << " | ";
 
        cout << setfill(' ') << setw(12) << construct_string(value, &alphabet);
        cout << resetiosflags(ios_base::basefield);
        cout << " | ";
 
        cout << setfill(' ') << setw(12) << time_diff;
        cout << resetiosflags(ios_base::basefield);
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
    
    benchmark(count, alphabet, str_max_len, hash, 1);

    return 0;
}
