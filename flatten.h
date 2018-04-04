
#pragma once

#include <thread>
#include <iostream>
#include <memory>
#include "promise.h"
#include "future.h"
#include "thread_pool.h"

template<typename T>
struct nested_type_getter;

template<typename T>
struct nested_type_getter<Future<T>> {
    typedef T type_t;
};

template<typename T>
struct nested_type_getter<Future<Future<T>>> {
    typedef typename nested_type_getter<Future<T>>::type_t type_t;
};

template<typename T>
auto FlattenImpl(const Future<Future<T>> &f) {
    return std::move(FlattenImpl(std::move(f.Get())));
}

template<typename T>
auto FlattenImpl(const Future<T> &f) {
    return std::move(f.Get());
}

template <typename T>
thread_pool* getPool(Future<T> &f){
    thread_pool* pool;
    if (f.GetPool() != nullptr){
        pool = f.GetPool();
    } else {
        if (thread_pool::curPool != nullptr){
            pool = thread_pool::curPool;
        } else {
            pool = nullptr;
        }
    }
    return pool;
}


template<typename T>
auto Flatten(thread_pool *pool, Future<Future<T>> &f) {
    using K = typename nested_type_getter<Future<T>>::type_t;
    std::shared_ptr<Promise<K>> p(new Promise<K>());
    if (pool == nullptr) {
        std::thread([p, &f]() {
            p->Set(std::move(FlattenImpl(f)));
        }).detach();
    } else {
        pool->execute([p, &f]() {
            p->Set(std::move(FlattenImpl(f)));
        });
    }
    return std::move(p->GetFuture());
}

template<typename T>
auto Flatten(Future<Future<T>> &f) {
    return std::move(Flatten(getPool(f), std::move(f)));
}

template<typename T>
auto Flatten(thread_pool *pool, Future<T> &f) {
    std::shared_ptr<Promise<T>> p(new Promise<T>());
    if (pool == nullptr) {
        std::thread([p, &f]() {
            p->Set(std::move(f.Get()));
        }).detach();
    } else {
        pool->execute([p, &f]()
        {
            p->Set(std::move(f.Get()));
        });
    }
    return std::move(p->GetFuture());
}

template<typename T>
auto Flatten(Future<T> &f) {
    return std::move(Flatten(getPool(f), std::move(f)));
}

template<template<typename, typename...> class C, typename T>
Future<C<T>> Flatten(C<Future<T>> const &col) {
    std::shared_ptr<Promise<C<T>>> p(new Promise<C<T>>());
    std::thread t([p, &col]() {
        C<T> output = C<T>();
        for (const Future<T> &f : col) {
            output.insert(std::end(output), f.Get());
        }
        p->Set(output);
    });
    t.detach();
    return std::move(p->GetFuture());
}
