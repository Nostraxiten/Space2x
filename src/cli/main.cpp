#include <space2x/core/Engine.h>
#include "commands/NetworkCommand.h"
#include "commands/ProfilesCommand.h"
#include "commands/ServicesCommand.h"
#include "commands/SystemCommand.h"

#include <iostream>
#include <string>
#include <vector>

namespace {

void printUsage() {
    std::cout << "Space2X - Cross-Platform Local Infrastructure Management CLI\n\n"
              << "Usage: space2x [--json] <command> [options...]\n\n"
              << "Commands:\n"
              << "  services [list|start|stop|restart|status|health] [service-id]\n"
              << "  profiles [list|apply] [profile-name] [--dry-run]\n"
              << "  system   [info|processes]\n"
              << "  network  [status|interfaces]\n"
              << "  audit    [limit]\n"
              << "  version  Print version information\n"
              << "  help     Print this help message\n\n"
              << "Options:\n"
              << "  --json   Emit output in JSON format for automated scripting\n";
}

} // anonymous namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 0;
    }

    std::vector<std::string> args;
    bool jsonMode = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--json") {
            jsonMode = true;
        } else {
            args.push_back(std::move(arg));
        }
    }

    if (args.empty() || args[0] == "help" || args[0] == "--help" || args[0] == "-h") {
        printUsage();
        return 0;
    }

    if (args[0] == "version" || args[0] == "--version" || args[0] == "-v") {
        std::cout << "Space2X version 0.1.0-alpha (C++20)\n";
        return 0;
    }

    auto engine = space2x::core::Engine::create();
    if (!engine) {
        std::cerr << "Fatal Error: Failed to initialize Space2X core engine.\n";
        return 1;
    }

    const std::string& command = args[0];
    std::vector<std::string> subArgs(args.begin() + 1, args.end());

    if (command == "services" || command == "service") {
        return space2x::cli::ServicesCommand::execute(*engine, subArgs, jsonMode);
    }

    if (command == "profiles" || command == "profile") {
        return space2x::cli::ProfilesCommand::execute(*engine, subArgs, jsonMode);
    }

    if (command == "system" || command == "sys") {
        return space2x::cli::SystemCommand::execute(*engine, subArgs, jsonMode);
    }

    if (command == "network" || command == "net") {
        return space2x::cli::NetworkCommand::execute(*engine, subArgs, jsonMode);
    }

    if (command == "audit") {
        size_t limit = 20;
        if (!subArgs.empty()) {
            try {
                limit = std::stoul(subArgs[0]);
            } catch (...) {}
        }
        auto events = engine->auditLog().getRecentEvents(limit);
        std::cout << "Recent Audit Log Events (" << events.size() << "):\n";
        std::cout << std::string(80, '-') << "\n";
        for (const auto& ev : events) {
            std::cout << "[" << ev.timestamp << "] "
                      << ev.action << " | Target: " << ev.targetId
                      << " | Outcome: " << ev.outcome;
            if (!ev.detail.empty()) {
                std::cout << " (" << ev.detail << ")";
            }
            std::cout << "\n";
        }
        return 0;
    }

    std::cerr << "Unknown command: '" << command << "'. Run 'space2x help' for available commands.\n";
    return 1;
}
