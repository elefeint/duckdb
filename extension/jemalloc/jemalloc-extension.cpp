#define DUCKDB_EXTENSION_MAIN
#include "jemalloc-extension.hpp"

#include "duckdb/common/allocator.hpp"
#include "jemalloc/jemalloc.h"

namespace duckdb {

class JEMallocWrapper {
public:
	static inline data_ptr_t Allocate(PrivateAllocatorData *private_data, idx_t size) {
		return (data_ptr_t)duckdb_jemalloc::je_malloc(size);
	};

	static inline void Free(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size) {
		duckdb_jemalloc::je_free(pointer);
	};

	static inline data_ptr_t ReAllocate(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t old_size,
	                                    idx_t size) {
		return (data_ptr_t)duckdb_jemalloc::je_realloc(pointer, size);
	};
};

void JEMallocExtension::Load(DuckDB &db) {
	// Initialize an Allocator that uses JEMalloc
	auto jemallocator =
	    make_unique<Allocator>(JEMallocWrapper::Allocate, JEMallocWrapper::Free, JEMallocWrapper::ReAllocate, nullptr);

	// Move any of the owned data to the new allocator
	db.instance->config.allocator->TransferPrivateData(*jemallocator);

	// Override the DB instance's allocator
	db.instance->config.allocator = move(jemallocator);
}

std::string JEMallocExtension::Name() {
	return "jemalloc";
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void jemalloc_init(duckdb::DatabaseInstance &db) {
	duckdb::DuckDB db_wrapper(db);
	db_wrapper.LoadExtension<duckdb::JEMallocExtension>();
}

DUCKDB_EXTENSION_API const char *jemalloc_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
