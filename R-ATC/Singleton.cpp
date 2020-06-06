// this code is from google/mozc (modified)
// https://github.com/google/mozc/blob/master/src/base/singleton.cc
// under BSD-3-Clause license

#include "singleton.h"

namespace {

    constexpr int kMaxFinalizersSize = 256;
    std::mutex gMutex;
    int gNumFinalizersSize = 0;
    SingletonFinalizer::FinalizerFunc gFinalizers[kMaxFinalizersSize];

}

void SingletonFinalizer::addFinalizer(FinalizerFunc func) {
    std::lock_guard<std::mutex> lock(gMutex);
    assert(gNumFinalizersSize < kMaxFinalizersSize);
    gFinalizers[gNumFinalizersSize++] = func;
}

void SingletonFinalizer::finalize() {
    std::lock_guard<std::mutex> lock(gMutex);
    for (int i = gNumFinalizersSize - 1; i >= 0; --i) {
        (*gFinalizers[i])();
    }
    gNumFinalizersSize = 0;
}

/* ----- how to use ----- */
// this code from mozc
/*
void main() {
    Foo& gFoo = singleton<Foo>::get_instance(); // get instance
    Bar& gBar = singleton<Bar>::get_instance(); // get instance
    SingletonFinalizer::finalize(); // delete instance
}
*/
