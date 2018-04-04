//
// Created by eugene on 28.9.2017
//
#ifdef _GTEST

#include "promise.h"
#include "future.h"

#include <thread>
#include <gtest/gtest.h>

TEST(promise, IsReady) {
    Promise<int> promise;
    Future<int> f = promise.GetFuture();


    ASSERT_FALSE(f.IsReady());

    int const x = 10;
    promise.Set(x);
    ASSERT_TRUE(f.IsReady());
}

TEST(promise, Set_int) {
    Promise<int> promise;
    Future<int> f = promise.GetFuture();

    int const x = 10;
    promise.Set(x);
    ASSERT_EQ(f.Get(), x);
}

TEST(promise, Set_int_ampersand) {
    Promise<int &> promise;
    Future<int &> f = promise.GetFuture();

    int const y = 100;
    int x = 10;

    promise.Set(x);

    f.Get() = y;

    ASSERT_EQ(x, y);
}

TEST(promise, Set_void) {
    Promise<void> promise;
    Future<void> f = promise.GetFuture();

    ASSERT_FALSE(f.IsReady());

    promise.Set();

    f.Get();

    ASSERT_TRUE(f.IsReady());
}

TEST(promise, two_threads_get_set) {
    Promise<int> promise;
    Future<int> future = promise.GetFuture();

    int x = 777;

    std::thread t(
            [x] (Promise<int> promise) {
                std::this_thread::sleep_for(std::chrono::seconds(1));

                promise.Set(x);
            }, std::move(promise)
    );

    for (int i = 0; !future.IsReady(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_EQ(future.Get(), x);

    t.join();
}

TEST(promise, two_threads_get_throw_exception) {
    Promise<int> promise;
    Future<int> future = promise.GetFuture();

    int x = 777;

    std::thread t(
            [x] (Promise<int> promise) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }, std::move(promise)
    );

    ASSERT_ANY_THROW(future.Get());

    t.join();
}
#endif // _GTEST