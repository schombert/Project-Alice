extern "C" {
#define XXH_NAMESPACE ZSTD_
#define ZSTD_DISABLE_ASM

#include "zstd/common/xxhash.c"
#include "zstd/decompress/zstd_decompress_block.c"
#include "zstd/decompress/zstd_ddict.c"
#include "zstd/compress/huf_compress.c"
#include "zstd/compress/fse_compress.c"
#include "zstd/decompress/huf_decompress.c"
#include "zstd/common/zstd_common.c"
#include "zstd/common/entropy_common.c"
#include "zstd/common/fse_decompress.c"
#include "zstd/compress/hist.c"
#include "zstd/compress/zstd_compress_superblock.c"
#include "zstd/compress/zstd_ldm.c"
#include "zstd/compress/zstd_opt.c"
#include "zstd/compress/zstd_lazy.c"
#include "zstd/compress/zstd_double_fast.c"
#include "zstd/compress/zstd_fast.c"
#include "zstd/compress/zstd_compress_literals.c"
#include "zstd/compress/zstd_compress_sequences.c"
#include "zstd/common/error_private.c"
#include "zstd/decompress/zstd_decompress.c"
#include "zstd/compress/zstd_compress.c"
};
