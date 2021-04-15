#pragma once

template <typename  _DirIt>
auto list_data(const _DirIt& dir_iterator){
	std::list<fs::path> path_list;
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