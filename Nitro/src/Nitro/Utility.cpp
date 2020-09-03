#include "NtPCH.h"

namespace Nitro
{
	namespace Util
	{
		bool is_file_accessible(const std::string& file_path)
		{
			struct stat buffer;
			return (stat(file_path.c_str(), &buffer) == 0);
		}

		std::string read_file(const std::string& path)
		{
			std::ifstream vsFile(path, std::ios::in);
			if (!vsFile.is_open()) 
			{
				std::cout << "FAIL: shader file loading failed." << std::endl;
				return "";
			}
			std::string content;
			std::string line = "";
			while (!vsFile.eof()) 
			{
				std::getline(vsFile, line);
				content.append(line + "\n");
			}
			content.append("\0");
			vsFile.close();
			return content;
		}

		std::string read_file2(const std::string& filepath)
		{
			std::string result;
			std::ifstream in(filepath, std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				size_t size = in.tellg();
				if (size != -1)
				{
					result.resize(size);
					in.seekg(0, std::ios::beg);
					in.read(&result[0], size);
					in.close();
				}
				else
				{
					NT_CORE_ERROR("Could not read from file '{0}'", filepath);
				}
			}
			else
			{
				NT_CORE_ERROR("Could not open file '{0}'", filepath);
			}

			return result;
		}

		std::string file_path2name(const std::string& path, bool withExt)
		{
			size_t lastSlashPos = path.find_last_of("\\/");
			size_t fileNameStartPos = (lastSlashPos == std::string::npos) ? 0 : lastSlashPos + 1;
			if (withExt)
			{
				return path.substr(fileNameStartPos);
			}

			size_t dotPos = path.rfind('.');
			size_t filenameCharNum = (dotPos == std::string::npos) ? path.size() - fileNameStartPos : dotPos - fileNameStartPos;
			return path.substr(fileNameStartPos, filenameCharNum);
		}

		std::wstring stdstring_to_stdwstring(const std::string& str)
		{
			std::wstring wstr(str.begin(), str.end());
			return wstr;
		}

		void simd_memcpy(void* __restrict dest, const void* __restrict src, size_t sizeInDQWord)
		{
			// @ align to 16 is due to __alignment_memblk is 128-bit-aligned. (16 bytes = 128 bits)
			NT_ASSERT(isAligned(dest, 16), "simd copy failed. address's not aligned to word-length.");
			NT_ASSERT(isAligned(src, 16), "simd copy failed. address's not aligned to word-length.");

			__m128i* __restrict pDest = (__m128i * __restrict)dest;
			const __m128i* __restrict pSrc = (const __m128i * __restrict)src;

			// @ to find how many quadwords precede a cache line boundary(64-bytes).
			// @ we need to copy them separately if the mem region cross a cache line boundary.
			// @ ??? actually...it finds num of double-qwords instead of qwords
			size_t mem_size_front_m64bytes = (4 - ((size_t)pSrc >> 4) & 3) & 3;
			if (mem_size_front_m64bytes > sizeInDQWord)
			{
				mem_size_front_m64bytes = sizeInDQWord;
			}
			switch (mem_size_front_m64bytes)
			{
			case 3: _mm_stream_si128(pDest + 2, _mm_load_si128(pSrc + 2));
			case 2: _mm_stream_si128(pDest + 1, _mm_load_si128(pSrc + 1));
			case 1: _mm_stream_si128(pDest + 0, _mm_load_si128(pSrc + 0));
			default: break;
			}

			if (mem_size_front_m64bytes == sizeInDQWord)
			{
				return;
			}

			pDest += mem_size_front_m64bytes;
			pSrc += mem_size_front_m64bytes;
			sizeInDQWord -= mem_size_front_m64bytes;

			// @ do cacheline fetch if size to copy is large enough,
			// @ this is a pre-step before do the actual stream copy
			// @ make simd faster
			size_t cachelines = sizeInDQWord >> 2;
			switch (cachelines)
			{
			default:	// @ put default ahead, to handle case when cache line number larger than pre-defined prefetch below
			case 10: _mm_prefetch((char*)(pSrc + 36), _MM_HINT_NTA);
			case 9: _mm_prefetch((char*)(pSrc + 32), _MM_HINT_NTA);
			case 8: _mm_prefetch((char*)(pSrc + 28), _MM_HINT_NTA);
			case 7: _mm_prefetch((char*)(pSrc + 24), _MM_HINT_NTA);
			case 6: _mm_prefetch((char*)(pSrc + 20), _MM_HINT_NTA);
			case 5: _mm_prefetch((char*)(pSrc + 16), _MM_HINT_NTA);
			case 4: _mm_prefetch((char*)(pSrc + 12), _MM_HINT_NTA);
			case 3: _mm_prefetch((char*)(pSrc + 8), _MM_HINT_NTA);
			case 2: _mm_prefetch((char*)(pSrc + 4), _MM_HINT_NTA);
			case 1: _mm_prefetch((char*)(pSrc + 0), _MM_HINT_NTA);
				// @ prefetch to cache backward, stream load forward.
				for (u32 i = cachelines; i > 0; --i)
				{
					if (i > 10)
					{
						_mm_prefetch((char*)(pSrc + 40), _MM_HINT_NTA);
					}
					// @ do 4 dqword copies at once to reduce stall
					_mm_stream_si128(pDest, _mm_load_si128(pSrc));
					_mm_stream_si128(pDest + 1, _mm_load_si128(pSrc + 1));
					_mm_stream_si128(pDest + 2, _mm_load_si128(pSrc + 2));
					_mm_stream_si128(pDest + 3, _mm_load_si128(pSrc + 3));

					pDest += 4;
					pSrc += 4;
				}
			case 0:
				break;
			}

			// @ dqwords left over 64-byte cache line sizes
			switch (sizeInDQWord && 3)
			{
			case 3: _mm_stream_si128(pDest + 2, _mm_load_si128(pSrc + 2));
			case 2: _mm_stream_si128(pDest + 1, _mm_load_si128(pSrc + 1));
			case 1: _mm_stream_si128(pDest + 0, _mm_load_si128(pSrc + 0));
			default: break;
			}
			// ??
			_mm_sfence();
		}

		hashkey_t hash_range(const u32* start, const u32* end, hashkey_t original)
		{
#if __enable_sse_crc32__
			const u64* ptr64 = (const u64*)alignUp(start, 8);
			const u64* end64 = (const u64*)alignDown(end, 8);
			if (ptr64 > (u64*)start)
			{
				original = _mm_crc32_u32(original, *start);
			}
			while (ptr64 < end64)
			{
				original = _mm_crc32_u64(original, *ptr64++);
			}
			if (ptr64 < (u64*)end)
			{
				original = _mm_crc32_u32(original, *(u32*)ptr64);
			}
#else
			for (u32* ptr = start; ptr < end; ++ptr)
			{
				original = 16777619U * original ^ *ptr;
			}
#endif
			return original;
		}

		hashkey_t hash_string(const std::string& str)
		{
			static std::hash<std::string> hasher;
			return hasher(str);
		}
	}
}