#ifndef CLASSES_CODECS_ZLIB
#define CLASSES_CODECS_ZLIB
#include "zlib.h"
//#define ZLIB_CHUNK_SIZE 0x4000

/*! @brief Zlib compressor / decompressor.
 * Compresses or decompresses char arrays.
 */
class Zlib{
	public:
		
		static char*	output;
		static size_t	output_size;
		static void clear(bool keep_mt_lock = false);
		static char *decompress(char *input, size_t size);
		static char *compress(char *input, size_t size);
		static bool test(char *testinput, size_t size);
};

#endif
