#include "NtPCH.h"
#include "FileOps.h"

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
					UASSERT(false);
				}
			}
			else
			{
				UASSERT(false);
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
	}
}