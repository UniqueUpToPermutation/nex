#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <sstream>

namespace fs = std::filesystem;

using namespace std;

struct PathHasher {
	std::size_t operator() (const std::filesystem::path& path) const {
		return std::filesystem::hash_value(path);
	}
};

set<string> important_ext = {
	".psh",
	".vsh",
	".json",
	".hlsl",
	".csh",
	".gsh",
    ".glsl",
	".vs",
	".fs",
	".gs"
};

void write_into_lookup(
	const std::filesystem::path& base,
	const fs::directory_entry& path,
	unordered_map<std::filesystem::path, string, PathHasher>* map, ofstream& out) {
	
	stringstream ss;

	ss << "g_" << std::filesystem::relative(path.path(), base).string() << "_data";

	string name = ss.str();
	std::replace(name.begin(), name.end(), '.', '_');
	std::replace(name.begin(), name.end(), '/', '_');
	out << "std::string_view " << name << " = R\"(";
	ifstream f(path.path());
	if (!f.is_open()) {
		stringstream err_ss;
		err_ss << "Failed to open file: " << path.path();
		throw std::runtime_error(err_ss.str());
	}

	std::string str((std::istreambuf_iterator<char>(f)),
		std::istreambuf_iterator<char>());

	out << str << ")\";";
	out << "\n\n";

	(*map)[path.path()] = name;
}

void find_files(
	const std::filesystem::path& base,
	const fs::directory_iterator& it,
	std::vector<std::filesystem::path>& output) {
	for (const auto& entry : it) {
		if (entry.is_regular_file()) {
			if (important_ext.find(entry.path().extension().string()) != important_ext.end()) {
				output.emplace_back(entry.path());
			}
		} else if (entry.is_directory()) {
			find_files(base, fs::directory_iterator(entry.path()), output);
		}
	}
}

void expand_file(
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
				&contents.at(last_include_pos), 
				&contents.at(include_pos), '\n');

			size_t endLineIndex = contents.find('\n', include_pos);
			bool bGlobalSearch = false;

			auto quotesIt = std::find(
				&contents.at(include_pos),
				&contents.at(endLineIndex), '\"');

			ss << contents.substr(last_include_pos, include_pos - last_include_pos);

			if (quotesIt == &contents.at(endLineIndex)) {
				std::stringstream ss;
				ss << path.string() << ": Include detected without include file!";
				throw std::runtime_error(ss.str());
			}
			else {
				size_t endIndx;
				size_t startIndx;

				startIndx = quotesIt - &contents.at(0) + 1;
				endIndx = contents.find('\"', startIndx);
				bGlobalSearch = false;
				
				if (endIndx == std::string::npos) {
					if (warningOut) {
						*warningOut << sourceStr << 
							": Warning: unmatched quote in #include!\n";
					}
					return {};
				}

				last_include_pos = endIndx + 1;

				std::filesystem::path includeSource = contents.substr(startIndx, endIndx - startIndx);
				std::filesystem::path nextPath = source.parent_path();

				if (bGlobalSearch) {
					auto err = Load(includeSource, fileLoader, defaults, overrides, streamOut,
						warningOut, output, bAddLineNumbers, alreadyVisited);

				} else {
					includeSource = nextPath / includeSource;

					auto err = Load(includeSource, fileLoader, defaults, overrides, streamOut,
						warningOut, output, bAddLineNumbers, alreadyVisited);
				}

				ss << std::endl << "\n#line " << current_line + 1 
					<< " \"" << sourceStr << "\"\n"; // Reset line numbers
			}

			include_pos = contents.find("#include", include_pos + 1);
		}

		ss << contents.substr(last_include_pos);
		return {};
	} else {
		std::stringstream ss;
		ss << "Invalid include " << path.string();
		throw std::runtime_error(ss.str());
	}
}

int main(int argc, char* argv[])
{   
	if (argc < 4) {
		throw std::runtime_error("Incorrect number of runtime arguments!");
	}

	std::string function_name = argv[3];

	string path(argv[1]);
	ofstream f_out(argv[2]);
	unordered_map<std::filesystem::path, string, PathHasher> map;
	f_out << "#include <okami/embed.hpp>\n";
	f_out << "\n";

	std::vector<std::filesystem::path> files;
	find_files(path, fs::directory_iterator(path), std::ref(files));

	

	f_out << "void " << function_name << "(okami::file_map_t& map) {\n";
	for (auto& it : map) {
		auto rel = std::filesystem::relative(it.first, path);
		f_out << "\tmap[\"" << rel.string() << "\"] = " << it.second << ";\n";
	}
	f_out << "}" << endl;
	return 0;
} 