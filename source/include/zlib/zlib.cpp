#include "zlib.hpp"
#include <stdexcept>
char*	Zlib::output = nullptr;
size_t	Zlib::output_size = 0;
char	outbuffer[65535];

#include <string>

void Zlib::clear(bool keep_mt_lock){
	if (output) free(output);
	output = nullptr;
	output_size = 0;
}

bool Zlib::test(char *testinput, size_t size){
	
	Zlib::compress(testinput, size);
	size_t outsize = Zlib::output_size;
	char *out = (char*)malloc(outsize);
	if (!out) {
		printf("Out of Memory");
		exit(-1);
	}
	memcpy(out, Zlib::output, outsize);
	Zlib::clear();
	
	std::string decompressed = Zlib::decompress(out, outsize);
	free(out); 
	outsize = Zlib::output_size;
	out = (char*)malloc(outsize);
	if (out) {
		memcpy(out, Zlib::output, outsize);
		Zlib::clear();
		free(out);
		out = nullptr;
	}
	
	// make sure uncompressed is exactly equal to original.
	if (outsize != size)return false;	
	return true;
}

char *Zlib::decompress(char *str, size_t size)
{
	
	z_stream zs;                        // z_stream is zlib's control structure
	memset(&zs, 0, sizeof(zs));

	if (inflateInit(&zs) != Z_OK)
		throw(std::runtime_error("inflateInit failed while decompressing."));

	zs.next_in = (Bytef*)str;
	zs.avail_in = (uInt)size;

	int ret;
	std::string outstring;

	// get the decompressed bytes blockwise using repeated calls to inflate
	do {
		zs.next_out = (Bytef*)(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = inflate(&zs, 0);

		if (outstring.size() < zs.total_out) {
			outstring.append(outbuffer,
				zs.total_out - outstring.size());
		}

	} while (ret == Z_OK);

	clear(true);
	output_size = outstring.size();
	output = (char*)malloc(output_size);
	if (!output) {
		printf("Out of Memory");
		exit(-1);
	}
	memcpy((void*)output, (Bytef*)outstring.data(), output_size);

	inflateEnd(&zs);
	if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
		printf("Compression error: code %d", ret);
	}
	
	return output;
}

char *Zlib::compress(char *str, size_t size){
	int compressionlevel = Z_BEST_COMPRESSION;
	z_stream zs;                        // z_stream is zlib's control structure
	memset(&zs, 0, sizeof(zs));

	if (deflateInit(&zs, compressionlevel) != Z_OK)
		throw(std::runtime_error("deflateInit failed while compressing."));

	zs.next_in = (Bytef*)str;
	zs.avail_in = (uInt)size;           // set the z_stream's input

	int ret;
	std::string outstring;

	// retrieve the compressed bytes blockwise
	do {
		zs.next_out = (Bytef*)(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = deflate(&zs, Z_FINISH);

		if (outstring.size() < zs.total_out) {
			// append the block to the output string
			outstring.append(outbuffer,
				zs.total_out - outstring.size());
		}
	} while (ret == Z_OK);

	clear(true);
	output_size = outstring.size();
	output = (char*)malloc(output_size);
	if (!output) {
		printf("Out of Memory");
		exit(-1); 
	}
	memcpy((void*)output, (Bytef*)outstring.data(), output_size);

	deflateEnd(&zs);
	if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
		printf("Compression error: code %d", ret);
	}
	return output;
}
