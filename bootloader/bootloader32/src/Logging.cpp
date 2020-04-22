#include "Logging.hpp"

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/VGA.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>

namespace FunnyOS::Bootloader32::Logging {
    using namespace Misc::TerminalManager;

    namespace {
        /**
         * Colors for the tags of the specific log levels.
         */
        Color g_logLevelColors[] = {Color::Cyan,   Color::LightBlue, Color::LightGreen,
                                    Color::Yellow, Color::LightRed,  Color::Red};

        /**
         * Tags for the specific log levels.
         */
        const char* g_logLevelNames[] = {
            " DBG", "INFO", " OK ", "WARN", " ERR", "FAIL",
        };

        bool g_debugModeEnabled = false;
    }  // namespace

    bool IsDebugModeEnabled() {
        return g_debugModeEnabled;
    }

    void SetDebugModeEnabled(bool enabled) {
        g_debugModeEnabled = enabled;
    }

    Misc::TerminalManager::TerminalManager* GetTerminalManager() {
        static HW::VGAInterface c_interface{};
        static Misc::TerminalManager::TerminalManager c_terminalManager{&c_interface};

        return &c_terminalManager;
    }

    void PostLog(LogLevel level, const char* message) {
        if (level == LogLevel::Debug && !g_debugModeEnabled) {
            return;
        }

        TerminalManager* terminal = GetTerminalManager();

        const Color preservedColor = terminal->GetForegroundColor();

        // Tag start
        terminal->ChangeForegroundColor(Color::White);
        terminal->PrintString(" * [");

        // Actual tag
        terminal->ChangeForegroundColor(g_logLevelColors[static_cast<int>(level)]);
        terminal->PrintString(g_logLevelNames[static_cast<int>(level)]);

        // Tag finish
        terminal->ChangeForegroundColor(Color::White);
        terminal->PrintString("] * ");

        // Message
        terminal->ChangeForegroundColor(preservedColor);
        terminal->PrintString(message);
        terminal->PrintLine();
    }

    void PostLogFormatted(LogLevel level, const char* format, ...) {
        static char bufferData[512];
        static Stdlib::String::StringBuffer buffer{bufferData, 512};

        va_list args;
        va_start(args, format);
        const bool ret = Stdlib::String::Format(buffer, format, &args);
        va_end(args);

        PostLog(level, ret ? bufferData : format);
    }
}  // namespace FunnyOS::Bootloader32::Logging