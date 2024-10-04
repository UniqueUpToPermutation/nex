#include <nex/error.hpp>

#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Log.h>

using namespace nex::log;

void nex::log::Init() {
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
}

void nex::Log(Error const& err, bool isWarning) {
    if (err.IsError()) {
        if (auto multi = std::get_if<MultipleErrors>(&err.details)) {
            for (auto const& subErr : multi->errors) {
                Log(subErr, isWarning);
            }
        } else {
            if (isWarning) {
                PLOG_WARNING << err.ToString();
            } else {
                PLOG_ERROR << err.ToString();
            }
        }
    }
}