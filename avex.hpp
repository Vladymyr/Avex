/*
* Interesting error if you import windows.h after doing this. Here is the fix too:
* https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
* Error C2872: 'byte': ambiguous symbol
*/
//using namespace std;

#define CSTR(str) ((str).c_str())

inline void configure_parser(cli::Parser& parser);

inline auto parse_string_into_args(char* line, const int limit, int& counter, char* args[]);

char* split_line(char** str, char** word);

void erase(std::string& path, bool rename, const int passes);

inline void rename_path(std::string& file_path);

inline auto generate_random_str(const int size);

inline void overwrite_content(std::string& file_path, const int passes);

inline void unlink_path(std::string& path);

#include <filesystem>
namespace fs = std::filesystem;

template <typename  _DirIt>
auto list_data(const _DirIt& dir_iterator);

std::string decompose_path(std::string file_path);

// Inline functions should go in a separate file that is included at the bottom of the header file containing their prototypes. 
// Preferred extensions for this file are ".ipp", ".inl", or ".inline". Do NOT put them in a file with extension ".cpp"
#include "avex.inl"

// Template functions should go in a separate file that is included at the bottom of the header file containing their prototypes. 
// Preferred extensions for this file are ".ipp", ".tpp", or ".template"
#include "avex.ipp"



