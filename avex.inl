inline void configure_parser(cli::Parser& parser) {
	DLOG_F(INFO, "Attempting to configure parser");

	parser.set_required<std::string>("p", "path", "Path to a file or folder for erasion");
	parser.set_optional<bool>("s", "subroutines", false, "Erase the subdirectories, the content inside these and the targeted directory itself");
	parser.set_optional<bool>("r", "rename", true, "Rename the directories/folders before overwriting and unlinking them");
	parser.set_optional<int>("ps", "passes", 1, "Number of times the deleted data will be overwritten");

	DLOG_F(INFO, "Parser configured correctly");
}

inline auto parse_string_into_args(char* line, const int limit, int& counter, char* args[]) {
	for (counter = 0; counter < limit - 1 && split_line(&line, &args[counter]); ++counter) {
		if (*line == '\0') {
			break;
		}
	}

	args[counter] = nullptr;
	return line;
};

// https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-std::string-in-c
inline auto generate_random_str(const int size) {
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

	tmp_str.reserve(size);

	for (auto i = 0; i < size; ++i) {
		tmp_str += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_str;
}

inline void rename_path(std::string& file_path) {
	const auto _file_path = CSTR(file_path);

	ERROR_CONTEXT("Renaming", _file_path);
	DLOG_F(INFO, "Attempting to rename %s", _file_path);

	const auto original_name = decompose_path(file_path);
	const auto original_name_length = original_name.length();
	const auto original_path = file_path.substr(0, file_path.length() - original_name_length);
	const auto new_path = original_path // get original path
		+ generate_random_str(std::min(original_name_length, _MAX_PATH - original_path.length())); // generate new name

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

inline void overwrite_content(std::string& file_path, int passes) {
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

inline void unlink_path(std::string& path) {
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
		DLOG_F(INFO, "Remove call %s", status ? "true" : "false");
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
