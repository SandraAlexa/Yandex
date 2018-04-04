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
#include "future.h"

template<typename T>
class Promise {
public:
	Promise() {
		state = std::shared_ptr<shared_state<T>>(new shared_state<T>());
	}
	~Promise() {
		state->onPromiseDestroyed();
	}
	Promise(Promise<T> && p) {
		state = p.state;
	}
	Future<T> GetFuture() {
		return Future<T>(state);
	}
	void SetPool(thread_pool * pool) {
		state->pool = pool;
	}
	void Set(const T& o) {
		std::unique_lock<std::mutex> l(state->fMutex);
		checkError();
		state->set(o);
		std::cerr.flush();
	}
	void Set(T&& o) {
		std::unique_lock<std::mutex> l(state->fMutex);
		checkError();
		state->set(std::move(o));
	}
	void SetException(const std::exception_ptr & o) {
		std::unique_lock<std::mutex> l(state->fMutex);
		checkError();
		state->setError(o);
	}
private:
	std::shared_ptr<shared_state<T>> state;
	bool checkError() {
		if (state->isSet) {
			throw std::runtime_error("Attempting to call promise's set method multiple times!");
		}
	}
};

template<>
class Promise<void> {
public:
	Promise() {
		state = std::shared_ptr<shared_state<void>>(new shared_state<void>());
	}
	~Promise() {
		state->onPromiseDestroyed();
	}
	Promise(Promise<void> && p) {
		state = p.state;
	}
	Future<void> GetFuture() {
		return Future<void>(state);
	}
	void SetPool(thread_pool * pool) {
		state->pool = pool;
	}
	void Set() {
		std::unique_lock<std::mutex> l(state->fMutex);
		checkError();
		state->set();
	}
	void SetException(const std::exception_ptr & o) {
		std::unique_lock<std::mutex> l(state->fMutex);
		checkError();
		state->setError(o);
	}
private:
	std::shared_ptr<shared_state<void>> state;
	bool checkError() {
		if (state->isSet) {
			throw std::runtime_error("Attempting to call promise's set method multiple times!");
		}
	}
};

template<typename T>
class Promise<T&> {
public:
	Promise() {
		state = std::shared_ptr<shared_state<T&>>(new shared_state<T&>());
	}
	~Promise() {
		state->onPromiseDestroyed();
	}
	Promise(Promise<T&> && p) {
		state = p.state;
	}
	Future<T&> GetFuture() {
		return Future<T&>(state);
	}
	void SetPool(thread_pool * pool) {
		state->pool = pool;
	}
	void Set(T& o) {
		std::unique_lock<std::mutex> l(state->fMutex);
		checkError();
		state->set(o);
	}
	void Set(T&& o) {
		std::unique_lock<std::mutex> l(state->fMutex);
		checkError();
		state->set(o);
	}
	void SetException(const std::exception_ptr & o) {
		std::unique_lock<std::mutex> l(state->fMutex);
		checkError();
		state->setError(o);
	}
private:
	std::shared_ptr<shared_state<T&>> state;
	bool checkError() {
		if (state->isSet) {
			throw std::runtime_error("Attempting to call promise's set method multiple times!");
		}
	}
};