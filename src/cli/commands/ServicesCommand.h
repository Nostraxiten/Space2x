#pragma once

#include <space2x/core/Result.h>
#include <string>
#include <vector>

namespace space2x::core {
class Engine;
}

namespace space2x::cli {

class ServicesCommand {
public:
    static int execute(core::Engine& engine,
                       const std::vector<std::string>& args,
                       bool jsonMode);
};

} // namespace space2x::cli
