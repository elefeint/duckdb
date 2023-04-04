#include "duckdb/function/scalar/compressed_materialization_functions.hpp"

namespace duckdb {

void BuiltinFunctions::RegisterCompressedMaterializationFunctions() {
	Register<CompressedMaterializationIntegralCompressFun>();
	Register<CompressedMaterializationIntegralDecompressFun>();
	Register<CompressedMaterializationStringFun>();
}

} // namespace duckdb
