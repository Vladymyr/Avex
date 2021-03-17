#include <fstream>
#include <iostream>
#include "loguru.hpp"
#include "cmdparser.hpp"
#include "avex.hpp"

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
	locale::global(locale(""));
	cout.imbue(locale());
	wcerr.imbue(locale());

	cli::Parser parser(argc, argv);
	configure_parser(parser);

	string logo = ("     e                                   \n    d8b     Y88b    /  e88~~8e  Y88b  /  \n   /Y88b     Y88b  /  d888  88b  Y88b/   \n  /  Y88b     Y88b/   8888__888   Y88b   \n /____Y88b     Y8/    Y888    ,   /Y88b  \n/      Y88b     Y      \"88___/   /  Y88b\n");
	RAW_LOG_F(INFO, CSTR(logo));

	if (!parser.run()) {
		LOG_F(ERROR, "There was a problem while parsing the arguments");
		exit(1);
	}

	string path = parser.get<string>("p");
	LOG_F(INFO, "Targeted path: %s", CSTR(path));

	if (!fs::exists(path)) {
		LOG_F(ERROR, "The path provided is invalid (doesn't exit)");
		exit(1);
	}

	const bool sub = parser.get<bool>("s");
	DLOG_F(INFO, "Look for sub-routines: %s", sub ? "true" : "false");

	if (fs::is_directory(path)) {
		DLOG_F(INFO, "Collecting paths of individual files and subdirectories");
		list<fs::path> path_list;
		if (sub) {
			path_list = list_data(fs::recursive_directory_iterator(path));
		}
		else {
			path_list = list_data(fs::directory_iterator(path));
		}

		path_list.sort(compare_dir_last);
		for (auto& _path : path_list) {
			if (sub || !fs::is_directory(_path)) {
				auto _path_str = _path.string();

				erase(_path_str);
			}
		}

		if (sub) {
			DLOG_F(INFO, "Attempting to unlink directory and the contents of all its subdirectories");

			erase(path);
		}
	}
	else {
		DLOG_F(INFO, "Attempting to unlink single file");

		erase(path);
	}

	return 0;
}

inline void configure_parser(cli::Parser& parser) {
	DLOG_F(INFO, "Attempting to configure parser");

	parser.set_optional<string>("p", "path", "C:\\Users\\Vladymyr\\Downloads\\32830186\\lol", "Path to a file or folder for erasion");
	parser.set_optional<bool>("s", "subroutines", true, "Erase the subdirectories, the content inside these and the targeted directory itself");
	parser.set_optional<int>("r", "rename", 1, "Rename the directories/folders before overwriting and unlinking them");
	parser.set_optional<int>("ps", "passes", 1, "Number of times the deleted data will be overwritten");

	DLOG_F(INFO, "Parser configured correctly");
}

// stdlib.h
// https://comp.os.ms-windows.programmer.win32.narkive.com/XUjC2CBO/max-path-vs-max-path-what-s-the-difference
#define _MAX_PATH 260

void rename_path(string& file_path)
{
	const auto _file_path = CSTR(file_path);

	ERROR_CONTEXT("Renaming", _file_path);
	DLOG_F(INFO, "Attempting to rename %s", _file_path);

	const auto original_name = decompose_path(file_path);
	const auto original_length = original_name.length();
	const auto new_path = file_path.substr(0, file_path.length() - original_length) // get original path
		+ generate_random_str(original_length); // generate new file name

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

void overwrite_content(string& file_path) {
	DLOG_F(INFO, "Attempting overwrite: %s", CSTR(file_path));

	const auto size = fs::file_size(file_path);
	DLOG_F(INFO, "File size: %i", size);

	ofstream dummy(file_path, ios_base::binary | ios_base::out);
	for (int i = 0; i < size; ++i) {
		dummy << 0; //Overwrite with zeros
	}
	dummy.close();

	DLOG_F(INFO, "Succesfully overwritten");
}


string decompose_path(string file_path)
{
#if defined _WIN32
	const auto last_separator = file_path.find_last_of('\\');
#else
	const auto last_separator = file_path.find_last_of('/');
#endif

	// Remove directory if present
	// Do this before extension removal incase directory has a period character
	if (string::npos != last_separator)
	{
		file_path.erase(0, last_separator + 1);
	}

	return file_path;
}

void unlink_path(string& path) {
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
	}
	else {
		LOG_F(ERROR, "Error while trying to remove \"%s\" from file system's entry", CSTR(path));
	}
}
