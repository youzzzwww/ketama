#include <string>
#include "murmur3_hash.h"


struct Murmur3
{
	static uint32_t hash(const std::string& key)
	{
		return MurmurHash3_x86_32(key.data(), key.length());
	}
};
