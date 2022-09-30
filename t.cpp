#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

using namespace std;

class Tmp {
public:
    int32_t a;

    Tmp( int32_t a_v = 0 ): a{a_v} {
        cout << "Constructed tmp " << a << '\n';
    }

    ~Tmp() {
        cout << "Destructed tmp " << a << '\n';
    }
};

Tmp& tf () {
    cout << "t begin\n";
    Tmp t(0);
    shared_ptr<Tmp> spt( new Tmp(2) );
    cout << "t end\n";
    return t;
}

int main () {
    cout << "main begin\n";
    cout << "t called\n";
    Tmp& t = tf();
    cout << "t finished\n";
    cout << "main end\n";
}
