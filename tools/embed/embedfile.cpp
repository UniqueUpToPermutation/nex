#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <sstream>
#include <regex>

namespace fs = std::filesystem;

using namespace std;

struct PathHasher {
	std::size_t operator() (const std::filesystem::path& path) const {
		return std::filesystem::hash_value(path);
	}
};

std::string GetFileUid(
	const std::filesystem::path& base,
	const std::filesystem::path& path) {
	
	auto s = std::filesystem::relative(path, base).string();
	std::replace(s.begin(), s.end(), '.', '_');
	std::replace(s.begin(), s.end(), '/', '_');
	std::replace(s.begin(), s.end(), '\\', '_');

	stringstream ss;
	ss << "g_" << s << "_data";
	return ss.str();
}

void FindFiles(
	const std::filesystem::path& base,
	const fs::directory_iterator& it,
	const std::unordered_set<string>& importantExt,
	std::vector<std::filesystem::path>& output) {
	for (const auto& entry : it) {
		if (entry.is_regular_file()) {
			if (importantExt.find(entry.path().extension().string()) != importantExt.end()) {
				output.emplace_back(entry.path());
			}
		} else if (entry.is_directory()) {
			FindFiles(base, fs::directory_iterator(entry.path()), importantExt, output);
		}
	}
}

void FindFiles(
	const std::filesystem::path& base,
	const std::unordered_set<string>& importantExt,
	std::vector<std::filesystem::path>& output) {
	FindFiles(base, fs::directory_iterator(base), importantExt, std::ref(output));
}

void ExpandFile(
	std::filesystem::path const& path,
	std::unordered_map<std::filesystem::path, std::string, PathHasher> const& corpus,
	std::unordered_set<std::filesystem::path>& alreadyVisited,
	std::stringstream& output
) {
	if (alreadyVisited.find(path) != alreadyVisited.end()) {
        return;
    }

    alreadyVisited.emplace(path);

	if (auto it = corpus.find(path); it != corpus.end()) {
		// Numbers to begin processing
		size_t last_include_pos = 0;
		size_t current_line = 0;

		// Write preprocessed string
		std::stringstream& ss = output;

		// Reset line numbers
		ss << std::endl << "#line " << current_line + 1 << " \"" << 
			path.string() << "\"" << std::endl; 

		// Find all includes
		auto const& contents = it->second;
		size_t include_pos = contents.find("#include");
		while (include_pos != std::string::npos) {

			current_line += std::count(
				contents.begin() + last_include_pos, 
				contents.begin() + include_pos, '\n');

			size_t endLineIndex = std::min(contents.find('\n', include_pos), contents.size());
			bool bGlobalSearch = false;

			auto quotesIt = std::find(
				contents.begin() + include_pos,
				contents.begin() + endLineIndex, '\"');

			ss << contents.substr(last_include_pos, include_pos - last_include_pos);

			if (quotesIt == contents.begin() + endLineIndex) {
				std::stringstream ss;
				ss << path.string() << ": Include detected without include file!";
				throw std::runtime_error(ss.str());
			}
			else {
				size_t endIndx;
				size_t startIndx;

				startIndx = quotesIt - contents.begin() + 1;
				endIndx = contents.find('\"', startIndx);
				
				if (endIndx == std::string::npos) {
					std::cout << path.string() << 
							": Warning: unmatched quote in #include!\n";
					return;
				}

				last_include_pos = endIndx + 1;

				std::filesystem::path includeSource = contents.substr(startIndx, endIndx - startIndx);
				std::filesystem::path nextPath = path.parent_path();

				if (bGlobalSearch) {
					ExpandFile(includeSource, corpus, alreadyVisited, output);

				} else {
					includeSource = nextPath / includeSource;
					ExpandFile(includeSource, corpus, alreadyVisited, output);
				}

				ss << std::endl << "\n#line " << current_line + 1 
					<< " \"" << path.string() << "\"\n"; // Reset line numbers
			}

			include_pos = contents.find("#include", include_pos + 1);
		}

		ss << contents.substr(last_include_pos);
		return;
	} else {
		std::stringstream ss;
		ss << "Invalid include " << path.string();
		throw std::runtime_error(ss.str());
	}
}

void ExpandFile(
	std::filesystem::path const& path,
	std::unordered_map<std::filesystem::path, std::string, PathHasher> const& corpus,
	std::stringstream& output
) { 
	std::unordered_set<std::filesystem::path> alreadyVisited;
	ExpandFile(path, corpus, std::ref(alreadyVisited), std::ref(output));
}

int main(int argc, char* argv[])
{   
	std::vector<const char*> arg(&argv[0], &argv[argc]);

	if (arg.size() < 4) {
		throw std::runtime_error("Incorrect number of runtime arguments!");
	}

	std::string function_name = arg[3];

	string basePath(arg[1]);
	ofstream outFile(arg[2]);

	if (!outFile.is_open()) {
		std::stringstream ss;
		ss << "Failed to open " << arg[2] << "!";
		throw std::runtime_error(ss.str());
	}
	
	outFile << "#include <nex/embed.hpp>\n";
	outFile << "\n";

	// Arguments after #3 are the file extensions to embed.
	auto importantExt = [&]() {
		std::unordered_set<string> result;
		for (int i = 4; i < arg.size(); ++i) {
			result.emplace(arg[i]);
		}
		return result;
	}();

	// File all files with the specified extensions
	std::vector<std::filesystem::path> files;
	FindFiles(basePath, importantExt, std::ref(files));

	// Open all files in the directory
	auto unexpandedCorpus = [&]() { 
		unordered_map<std::filesystem::path, string, PathHasher> result;
		for (auto file : files) {
			std::ifstream s(file);

			if (!s.is_open()) {
				std::stringstream ss;
				ss << "Could not open file " << file.string();
				throw std::runtime_error(ss.str());
			}

			std::stringstream ss;
			ss << s.rdbuf();
			result[file] = ss.str();
		}
		return result;
	}();

	// Expand files by running the include import preprocessor
	auto expandedCorpus = [&]() {
		unordered_map<std::filesystem::path, string, PathHasher> result;

		for (auto [path, contents] : unexpandedCorpus) {
			std::stringstream ss;
			ExpandFile(path, unexpandedCorpus, std::ref(ss));
			result[path] = ss.str();
		}

		return result;
	}();

	// Write global variables with expanded file contents
	for (auto const& [path, expandedContents] : expandedCorpus) {
		outFile << "constexpr std::string_view " << GetFileUid(basePath, path) << " = R\"(\n";
		outFile << expandedContents;
		outFile << "\n)\";";
		outFile << "\n\n";
	}

	// Write function to retrieve contents of all expanded files
	outFile << "void " << function_name << "(nex::file_map_t& map) {\n";
	for (auto const& [path, expandedContents] : expandedCorpus) {
		auto rel = std::filesystem::relative(path, basePath);
		outFile << "\tmap[\"" << rel.string() << "\"] = " << GetFileUid(basePath, path) << ";\n";
	}
	outFile << "}" << endl;
	return 0;
} 