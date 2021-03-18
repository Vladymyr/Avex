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

inline void erase(std::string& path) {
	LOG_F(INFO, "Removing: %s", CSTR(path));
	rename_path(path);
	if (!fs::is_directory(path)) { 
		overwrite_content(path);
	}
	unlink_path(path);
}

// https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-std::string-in-c
inline auto generate_random_str(const int len) {
	std::string tmp_str;
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

inline auto compare_dir_last(const fs::path& first, const fs::path& second)
{
	return !fs::is_directory(first) && fs::is_directory(second);
}
