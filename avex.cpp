#include <fstream>
#include <iostream>
#include "loguru.hpp"
#include "cmdparser.hpp"
#include "avex.hpp"

constexpr auto logo = "     e                                   \n    d8b     Y88b    /  e88~~8e  Y88b  /  \n   /Y88b     Y88b  /  d888  88b  Y88b/   \n  /  Y88b     Y88b/   8888__888   Y88b   \n /____Y88b     Y8/    Y888    ,   /Y88b  \n/      Y88b     Y      \"88___/   /  Y88b\n";
constexpr auto invalid_path = "_";

int main(int argc, char* argv[])
{

	//loguru::g_preamble = false;
	loguru::g_preamble_header = false;

	loguru::g_preamble_uptime = false;
	loguru::g_preamble_thread = false;
	loguru::g_preamble_file = false;

#ifndef _DEBUG
	loguru::g_preamble_date = false;
	loguru::g_preamble_time = false;
#endif

	// Force to use global locale
	setlocale(LC_ALL, "");
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale());
	std::wcerr.imbue(std::locale());

	cli::Parser parser(argc, argv);
	configure_parser(parser);

	RAW_LOG_F(INFO, logo);

	if (!parser.run()) {
		LOG_F(ERROR, "There was a problem while parsing the arguments");
		exit(1);
	}

	auto path = parser.get<std::string>("p");
	LOG_F(INFO, "Targeted path: %s", CSTR(path));

	if (path._Equal(invalid_path) || !fs::exists(path)) {
		LOG_F(ERROR, "The path provided is invalid (doesn't exit)");
		exit(1);
	}

	const auto sub = parser.get<bool>("s");
	const auto rename = parser.get<bool>("r");
	const auto passes = parser.get<int>("ps");

	DLOG_F(INFO, "Look for sub-routines: %s", sub ? "true" : "false");
	if (fs::is_directory(path)) {
		DLOG_F(INFO, "Collecting paths of individual files and subdirectories");
		auto path_list = sub
			? list_data(fs::recursive_directory_iterator(path))
			: list_data(fs::directory_iterator(path));

		path_list.sort(compare_dir_last);
		for (auto& _path : path_list) {
			if (sub || !fs::is_directory(_path)) {
				auto _path_str = _path.string();

				erase(_path_str, rename, passes);
			}
		}

		if (sub) {
			DLOG_F(INFO, "Attempting to unlink directory and the contents of all its subdirectories");

			erase(path, rename, passes);
		}
	}
	else {
		DLOG_F(INFO, "Attempting to unlink single file");

		erase(path, rename, passes);
	}


	return 0;
}

inline void configure_parser(cli::Parser& parser) {
	DLOG_F(INFO, "Attempting to configure parser");

	parser.set_optional<std::string>("p", "path", invalid_path, "Path to a file or folder for erasion");
	parser.set_optional<bool>("s", "subroutines", false, "Erase the subdirectories, the content inside these and the targeted directory itself");
	parser.set_optional<bool>("r", "rename", true, "Rename the directories/folders before overwriting and unlinking them");
	parser.set_optional<int>("ps", "passes", 1, "Number of times the deleted data will be overwritten");

	DLOG_F(INFO, "Parser configured correctly");
}

void rename_path(std::string& file_path)
{
	const auto _file_path = CSTR(file_path);

	ERROR_CONTEXT("Renaming", _file_path);
	DLOG_F(INFO, "Attempting to rename %s", _file_path);

	const auto original_name = decompose_path(file_path);
	const auto original_name_length = original_name.length();
	const auto original_path = file_path.substr(0, file_path.length() - original_name_length);
	const auto new_path = original_path // get original path
		+ generate_random_str(min(original_name_length, _MAX_PATH - original_path.length())); // generate new name
	;
	DLOG_F(INFO, "New generated name: \"%s\"", CSTR(new_path));

	// https://stackoverflow.com/a/48614612
	try {
		/*
		* Personal note: If you use the "std::rename" function (in my case: rename(_file_path, CSTR(new_path)))
		* provided by C and the new name collides with an already existing file then it just doesn't
		* do it. But with C++ 17 file system, if that's the case, the original file will get over written
		* by the renamed one.
		*/
		fs::rename(file_path, new_path);
		DLOG_F(INFO, "Succesfully renamed");
		file_path = new_path;
	}
	catch (std::filesystem::filesystem_error& e) {
		LOG_F(ERROR, "Error while trying to rename \"%s\"", _file_path);
		perror("Error renaming file");
	}
}

void overwrite_content(std::string& file_path, int passes) {
	DLOG_F(INFO, "Attempting overwrite: %s", CSTR(file_path));

	const auto size = fs::file_size(file_path);
	DLOG_F(INFO, "File size: %i", size);
	for (auto p = 0; p < passes; ++p) {
		std::ofstream dummy(file_path, std::ios_base::binary | std::ios_base::trunc | std::ios_base::out);
		for (auto i = 0; i < size; ++i) {
			dummy.put(0); //Overwrite with zeros
		}
		dummy.close();
	}

	DLOG_F(INFO, "Succesfully overwritten");
}

std::string decompose_path(std::string file_path)
{
#ifdef _WIN32
	const auto last_separator = file_path.find_last_of('\\');
#else
	const auto last_separator = file_path.find_last_of('/');
#endif

	// Remove directory if present
	// Do this before extension removal incase directory has a period character
	if (std::string::npos != last_separator)
	{
		file_path.erase(0, last_separator + 1);
	}

	return file_path;
}

void unlink_path(std::string& path) {
	const char* _path_str = CSTR(path);
	ERROR_CONTEXT("Unlinking", _path_str);
	DLOG_F(INFO, "Attempting to unlink: %s", _path_str);

	auto status = -1;
	if (fs::is_directory(path)) {
		status = fs::remove_all(_path_str);
		DLOG_F(INFO, "Remove all call %b", status);
	}
	else {
		status = fs::remove(_path_str);
		DLOG_F(INFO, "Remove call %b", status);
	}
	path = decompose_path(path);
	if (status) {
		DLOG_F(INFO, "File entry \"%s\" and it's contents removed from file system entry", CSTR(path));
		LOG_F(INFO, "Entry erased successfully");
	}
	else {
		LOG_F(ERROR, "Error while trying to erase \"%s\" from file system's entry", CSTR(path));
	}
}
