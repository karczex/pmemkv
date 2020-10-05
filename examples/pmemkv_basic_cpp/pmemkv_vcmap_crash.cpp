// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2019-2020, Intel Corporation */

/*
 * pmemkv_basic.cpp -- example usage of pmemkv.
 */

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <libpmemkv.hpp>
#include <sstream>

#include <numeric>
#include <thread>
#include <vector>

#define LOG(msg) std::cout << msg << std::endl

using namespace pmem::kv;

static inline pmem::kv::string_view uint64_to_strv(uint64_t &key)
{
	return pmem::kv::string_view((char *)&key, sizeof(uint64_t));
}

template <typename Function>
void parallel_exec(size_t threads_number, Function f)
{
	std::vector<std::thread> threads;
	threads.reserve(threads_number);

	for (size_t i = 0; i < threads_number; ++i) {
		threads.emplace_back(f, i);
	}

	for (auto &t : threads) {
		t.join();
	}
}

const uint64_t SIZE_16M = 16UL * 1024UL * 1024UL;
/*
 * /opt/workspace/pmemkv_upstream/build/tests/concurrent_put_get_remove_single_op_params
 * vcmap;{"path":"/dev/shm/vcmap__concurrent_put_get_remove_single_op_params__default_1000_0_none","size":104857600};1000
 */
int main(int argc, char *argv[])
{

//	LOG("Creating config");
	config cfg;

	status s = cfg.put_string("path", "/dev/shm");
	assert(s == status::OK);
	s = cfg.put_uint64("size", SIZE_16M);
	assert(s == status::OK);

	const int threads_number = 16;

//	LOG("test with " << threads_number << " threads");

//	LOG("Opening pmemkv database with 'vcmap' engine");
	db *kv = new db();
	assert(kv != nullptr);
	s = kv->open("vcmap", std::move(cfg));
	assert(s == status::OK);

	std::vector<uint64_t> keys(threads_number, 0);
	std::iota(keys.begin(), keys.end(), 0);

	parallel_exec(threads_number, [&](size_t thread_id) {
		auto s = kv->put(uint64_to_strv(keys[thread_id]),
				 uint64_to_strv(keys[thread_id]));
		if (static_cast<int>(s) != 0) {
			LOG("status: " << static_cast<int>(s) << " ERROR occured");
			exit(1);
		}
	});

//	LOG("Closing database");
	delete kv;

	return 0;
}
