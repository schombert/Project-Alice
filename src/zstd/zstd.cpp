extern "C" {
#define XXH_NAMESPACE ZSTD_
#define ZSTD_DISABLE_ASM

#include "zstd/xxhash.c"
#include "zstd/zstd_decompress_block.c"
#include "zstd/zstd_ddict.c"
#include "zstd/huf_compress.c"
#include "zstd/fse_compress.c"
#include "zstd/huf_decompress.c"
#include "zstd/fse_decompress.c"
#include "zstd/zstd_common.c"
#include "zstd/entropy_common.c"
#include "zstd/hist.c"
#include "zstd/zstd_compress_superblock.c"
#include "zstd/zstd_ldm.c"
#include "zstd/zstd_opt.c"
#include "zstd/zstd_lazy.c"
#include "zstd/zstd_double_fast.c"
#include "zstd/zstd_fast.c"
#include "zstd/zstd_compress_literals.c"
#include "zstd/zstd_compress_sequences.c"
#include "zstd/error_private.c"
#include "zstd/zstd_decompress.c"
#include "zstd/zstd_compress.c"
};