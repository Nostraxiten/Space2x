#pragma once

#include <string>
#include <vector>

namespace space2x::core {
class Engine;
}

namespace space2x::cli {

class NetworkCommand {
public:
    static int execute(core::Engine& engine,
                       const std::vector<std::string>& args,
                       bool jsonMode);
};

} // namespace space2x::cli
