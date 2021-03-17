#ifdef _WIN32 // Note: without the underscore, it's not msdn official!
#include <windows.h>
typedef DWORD(WINAPI* func_proto_GetCurrentProcessId)(void);
static func_proto_GetCurrentProcessId getpid = GetCurrentProcessId;
// Windows (x64 and x86)
#elif defined(__unix__) or defined(__linux__)
#include <unistd.h>
#elif __APPLE__
// Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
#endif

/*
* Interesting error and fix
* https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
* Error C2872: 'byte': ambiguous symbol
*/
using namespace std;
#include <filesystem>
namespace fs = filesystem;

#define CSTR(str) (str).c_str()

inline void configure_parser(cli::Parser& parser);

void rename_path(string& file_path);

void overwrite_content(string& file_path);

string decompose_path(string file_path);

void unlink_path(string& path);

inline void erase(string& path) {
	LOG_F(INFO, "Removing: %s", CSTR(path));
	rename_path(path);
	overwrite_content(path);
	unlink_path(path);
}

template <typename  _DirIt>
list<fs::path> list_data(const _DirIt& dir_iterator) {
	list<fs::path> path_list;
	try {
		for (const auto& entry : dir_iterator) {
			const auto path = entry.path();
			auto path_str = path.string();

			/*
			* Originally wanted to use IsWindows10OrGreater() for Windows 10 Version 1903 (build 18362)
			* because they can now opt-in via their app manifest to use UTF-8 as a process-wide codepage,
			* even if the user locale is not set to UTF - 8. But even then, if the file name is using asian
			* characters, most require UTF-16 which C++ doesn't support properly (as far as I know):
			* - https://stackoverflow.com/questions/17103925/how-well-is-unicode-supported-in-c11
			* - https://stackoverflow.com/questions/50613451/the-proper-way-to-handle-unicode-with-c-in-2018
			* So it's either go only for ASCII file names or play the russian roulette.
			*/
			//if (!std::any_of(path_str.begin(), path_str.end(),
			//	[](const char c) {
			//		return static_cast<unsigned char>(c) > 127; //Unicode character detected
			//	})) {

			path_list.push_back(path);
			DLOG_F(INFO,
				"File \"%s\" (%s) added to the list",
				CSTR(decompose_path(path_str)), fs::is_directory(path) ? "folder" : "file");
			//}
		}
	}
	catch (std::filesystem::filesystem_error& e) {
		LOG_F(ERROR, "Error while iterating through the files");
		perror("Error iterating files");
	}

	if (path_list.empty()) {
		LOG_F(INFO, "The directory is empty...");
	}

	return path_list;
}

// https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
static inline auto generate_random_str(const int len) {
	string tmp_str;
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	static auto once = false;

	if (once) {
		srand(static_cast<unsigned int>(time(nullptr)));
		once = true;
	}

	tmp_str.reserve(len);

	for (auto i = 0; i < len; ++i) {
		tmp_str += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_str;
}

static auto compare_dir_last(const fs::path& first, fs::path& second)
{
	return !fs::is_directory(first) && fs::is_directory(second);
}
