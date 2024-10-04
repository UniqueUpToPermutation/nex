#pragma once

#include <unordered_map>
#include <filesystem>
#include <functional>

#include <nex/error.hpp>

namespace nex {

    namespace embed {
        struct PathHash {
            std::size_t operator()(const std::filesystem::path& p) const noexcept {
                return std::filesystem::hash_value(p);
            }
        };
    };

	typedef std::unordered_map<std::filesystem::path, std::string_view, embed::PathHash> file_map_t;
	typedef std::function<void(file_map_t&)> embedded_file_loader_t;

	class IVirtualFileSystem {
	public:
		virtual ~IVirtualFileSystem() = default;

		virtual Expected<std::string_view> Find(const std::filesystem::path& source) const = 0;
	};

	class EmbeddedFileLoader : public IVirtualFileSystem {
	private:
		file_map_t _internalShaders;

	public:
		void Add(const embedded_file_loader_t& factory);

		inline auto begin() { return _internalShaders.begin(); }
		inline auto end() { return _internalShaders.end(); }
		inline auto begin() const { return _internalShaders.begin(); }
		inline auto end() const { return _internalShaders.end(); }

        EmbeddedFileLoader() = default;
		~EmbeddedFileLoader() = default;

        EmbeddedFileLoader(const embedded_file_loader_t& factory);

		Expected<std::string_view> Find(const std::filesystem::path& source) const override;
	};
}