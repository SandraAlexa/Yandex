//
// Created by eugene on 11.10.2017
//
#pragma once

#include <thread>
#include <iostream>
#include <memory>
#include <functional>
#include "thread_pool.h"
#include "promise.h"
#include "future.h"

template<typename T, typename F>
Future<typename std::result_of<F(T)>::type> Map(thread_pool *pool, Future<T> &&f, const F &func) {
    using K = typename std::result_of<F(T)>::type;
    std::shared_ptr<Promise<K>> p = std::shared_ptr<Promise<K>>(new Promise<K>());
    if (pool != nullptr) {
        pool->execute([&f, &func, p] {
            p->Set(std::move(func(f.Get())));
        });
    } else {
        std::thread([&f, &func, p] {
            p->Set(std::move(func(f.Get())));
        }).detach();
    }
    return std::move(p->GetFuture());
}


template<typename T, typename F>
Future<typename std::result_of<F(T)>::type> Map(Future<T> &&f, const F &func) {
    thread_pool *pool;
    if (f.GetPool() != nullptr) {
        pool = f.GetPool();
    } else if (thread_pool::curPool != nullptr) {
        pool = thread_pool::curPool;

    } else {
        pool = nullptr;
    }
    std::cerr << (pool == nullptr) << std::endl;
    return std::move(Map(pool, std::move(f), func));
}



