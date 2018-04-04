#include <iostream>
#include "map.h"
#include "flatten.h"
#include "promise.h"
#include "future.h"

using namespace std;

int main() {
    thread_pool pool(10);
    Promise<int> p;
    pool.execute([&p]() {
        p.Set(0);
    });
    Future<long> futa = Map(&pool, std::move(p.GetFuture()), [](int ii) {
        return (long) (ii + 1);
    });
    cout << (futa.Get() == 1) ? "OK"
                              : "NEOK";
}
