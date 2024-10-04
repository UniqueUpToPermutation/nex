#pragma once

namespace nex::log {
    void Init();
}

namespace nex {
    struct Error;
    void Log(Error const& err, bool isWarning = false);
}