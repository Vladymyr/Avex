/*
* Interesting error if you import windows.h after doing this. Here is the fix too:
* https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
* Error C2872: 'byte': ambiguous symbol
*/
//using namespace std;

#define CSTR(str) ((str).c_str())

inline void configure_parser(cli::Parser& parser);

void rename_path(std::string& file_path);

void overwrite_content(std::string& file_path);

void unlink_path(std::string& path);

std::string decompose_path(std::string file_path);

#include <filesystem>
namespace fs = std::filesystem;

template <typename  _DirIt>
std::list<fs::path> list_data(const _DirIt& dir_iterator);

inline void erase(std::string& path);

inline auto generate_random_str(const int len);

inline auto compare_dir_last(const fs::path& first, const fs::path& second);

// Template functions should go in a separate file that is included at the bottom of the header file containing their prototypes. 
// Preferred extensions for this file are ".ipp", ".tpp", or ".template"
#include "avex.ipp"

// Inline functions should go in a separate file that is included at the bottom of the header file containing their prototypes. 
// Preferred extensions for this file are ".ipp", ".inl", or ".inline". Do NOT put them in a file with extension ".cpp"
#include "avex.inl"

