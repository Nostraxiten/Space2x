#pragma once

#include <string>
#include <vector>

namespace space2x::core {

struct ConfigDeltaItem {
    std::string key{};
    std::string oldValue{};
    std::string newValue{};
};

struct ConfigDelta {
    std::string                  targetFile{};
    std::vector<ConfigDeltaItem> changes{};
    std::string                  rawPreview{};

    [[nodiscard]] bool hasChanges() const noexcept {
        return !changes.empty();
    }
};

} // namespace space2x::core
