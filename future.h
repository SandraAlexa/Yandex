#pragma once
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <atomic>
#include <memory>
#include "thread_pool.h"
#include "shared_state.h"

template<typename T>
class Future {
public:
    Future(Future<T> && f) {
        state = f.state;
    }
    Future& operator=(Future<T> && f) {
        state = f.state;
        return *this;
    }
    /*
    Future operator=(const Future<T> & f) {
        state = f.state;
    }*/

    thread_pool * GetPool() {
        return state->pool;
    }

    const T& Get() const {
        Wait();
        if (state->isException) {
            std::rethrow_exception(state->exc);
        }
        return state->result;
    }

    bool IsReady() const {
        return state->isSet;
    }
    void Wait() const{
        if (state->isSet) {
            return;
        }
        std::unique_lock<std::mutex> l(state->fMutex);
        while (!state->isSet && !state->promiseDestroyed) {
            state->cVar.wait(l);
        }
        if (state->promiseDestroyed && !state->isSet) {
            throw std::runtime_error("Can't wait for value any longer - promise has been destroyed!");
        }
    }
    std::shared_ptr<shared_state<T>> state;

    Future(std::shared_ptr<shared_state<T>> s) {
        state = s;
    }
    Future() = default;
};

template<>
class Future<void> {
public:
    Future(Future<void> && f) {
        state = std::move(f.state);
    }

    void Get() const {
        Wait();
        if (state->isException) {
            std::rethrow_exception(state->exc);
        }
    }

    bool IsReady() const {
        return state->isSet;
    }

    void Wait() const {
        if (state->isSet) {
            return;
        }
        std::unique_lock<std::mutex> l(state->fMutex);
        while (!state->isSet && !state->promiseDestroyed) {
            state->cVar.wait(l);
        }
        if (state->promiseDestroyed && !state->isSet) {
            throw std::runtime_error("Can't wait for value any longer - promise has been destroyed!");
        }
    }

    std::shared_ptr<shared_state<void>> state;

    Future(std::shared_ptr<shared_state<void>> s) {
        state = s;
    }
};

template<typename T>
class Future<T&> {
public:
    Future(Future<T&> && f) {
        state = std::move(f.state);
    }

    T& Get(){
        Wait();
        if (state->isException) {
            std::rethrow_exception(state->exc);
        }
        return *state->result;
    }

    bool IsReady() const {
        return state->isSet;
    }
    void Wait() const{
        if (state->isSet) {
            return;
        }
        std::unique_lock<std::mutex> l(state->fMutex);
        while (!state->isSet && !state->promiseDestroyed) {
            state->cVar.wait(l);
        }
        if (state->promiseDestroyed && !state->isSet) {
            throw std::runtime_error("Can't wait for value any longer - promise has been destroyed!");
        }
    }
    std::shared_ptr<shared_state<T&>> state;

    Future(std::shared_ptr<shared_state<T&>> s) {
        state = s;
    }
};