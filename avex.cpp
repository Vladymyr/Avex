#include <fstream>
#include <iostream>
#include "loguru.hpp"
#include "cmdparser.hpp"
#include "avex.hpp"

constexpr auto logo = "     e                                   \n    d8b     Y88b    /  e88~~8e  Y88b  /  \n   /Y88b     Y88b  /  d888  88b  Y88b/   \n  /  Y88b     Y88b/   8888__888   Y88b   \n /____Y88b     Y8/    Y888    ,   /Y88b  \n/      Y88b     Y      \"88___/   /  Y88b\n";

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

avex:
	cli::Parser parser(argc, argv);
	configure_parser(parser);

	RAW_LOG_F(INFO, logo);

	if (!parser.run()) {
		LOG_F(ERROR, "There was a problem while parsing the arguments");
		exit(EXIT_FAILURE);
	}

	auto path = parser.get<std::string>("p");
	LOG_F(INFO, "Targeted path: %s", CSTR(path));

	if (!fs::exists(path)) {
		LOG_F(ERROR, "The path provided is invalid (doesn't exit)");
		exit(EXIT_FAILURE);
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

		path_list.sort([](const fs::path& first, const fs::path& second) {
			return !fs::is_directory(first) && fs::is_directory(second);
		});

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

	LOG_F(INFO, "\nErasion completed. Exit? (Y/N)");
	if (std::cin.get() == 'N') {
		LOG_F(INFO, "Waiting for command");

		std::string next_cmd;
		std::getline(std::cin, next_cmd);
		if (!next_cmd.empty()) {
			//argv[0] is always the app
			next_cmd = "Avex " + next_cmd;

			// std::string into a writable C string
			const auto line_size = next_cmd.size();
			auto input = new char[line_size + 1];
			std::copy(next_cmd.begin(), next_cmd.end(), input);
			input[line_size] = '\0';

			constexpr auto argc_MAX = 10;
			char* _argv[argc_MAX] = { 0 };
			auto _argc = 0;

			if (parse_string_into_args(input, argc_MAX, _argc, _argv) != '\0') { // or more clear `strlen(rest)==0` but not efficient
				DLOG_F(WARNING, "There is still something to parse. argc_MAX is too small.");
			}

			delete[] input;
			input = nullptr;
			argc = _argc;
			argv = _argv;
			goto avex;
		}
	}

	return EXIT_SUCCESS;
}

void erase(std::string& path, bool rename, int passes) {
	LOG_F(INFO, "Removing: %s", CSTR(path));
	if (rename) {
		rename_path(path);
	}

	if (!fs::is_directory(path) && passes != 0) {
		overwrite_content(path, passes);
	}

	unlink_path(path);
}

// https://stackoverflow.com/a/44326935
// Maybe try making this C++ like instead of C
char* split_line(char** str, char** word)
{
	constexpr auto QUOTE = '\'';
	auto inquotes = false;

	// optimization
	if (**str == 0) {
		return nullptr;
	}

	// Skip leading spaces.
	while (**str && isspace(**str)) {
		(*str)++;
	}

	if (**str == '\0') {
		return nullptr;
	}

	// Phrase in quotes is one arg
	if (**str == QUOTE) {
		(*str)++;
		inquotes = true;
	}

	// Set phrase begining
	*word = *str;

	// Skip all chars if in quotes
	if (inquotes) {
		while (**str && **str != QUOTE) {
			(*str)++;
			//if( **str!= QUOTE )
		}
	}
	else {
		// Skip non-space characters.
		while (**str && !isspace(**str)) {
			(*str)++;
		}
	}
	// Null terminate the phrase and set `str` pointer to next symbol
	if (**str) {
		*(*str)++ = '\0';
	}

	return *str;
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

