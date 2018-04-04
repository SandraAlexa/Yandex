//
// Created by eugene on 11.10.2017
//
#ifdef _GTEST

#include "flatten.h"

#include <thread>
#include <vector>
#include <iostream>
#include <gtest/gtest.h>



TEST(ff, Collection1) {
	Promise<int> p1;
	Promise<int> p2;
	std::vector<Future<int>> ff;
	ff.push_back(p1.GetFuture());
	ff.push_back(p2.GetFuture());
	p1.Set(1);
	p2.Set(2);
	Future<std::vector<int>> v = Flatten(ff);
	std::vector<int> result = v.Get();
	ASSERT_EQ(result[0], 1);
	ASSERT_EQ(result[1], 2);
}

TEST(ff, NestedFuture3) {
	Promise<Future<Future<int>>> p1;
	Promise<Future<int>> p2;
	Promise<int> p3;
	p3.Set(1);
	p2.Set(std::move(p3.GetFuture()));
	p1.Set(std::move(p2.GetFuture()));
	ASSERT_EQ(Flatten(std::move(p1.GetFuture())).Get(), 1);
}

TEST(ff, SimpleFuture) {
	Promise<int> p;
	p.Set(1);
	ASSERT_EQ(Flatten(std::move(p.GetFuture())).Get(), 1);
}

#endif // _GTEST
