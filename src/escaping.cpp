#include "escaping.hpp"

namespace Escaping {
    bool isEscaped(const std::string& s, size_t pos) {
        if (pos == 0)
            return false;

        size_t last = s.find_last_not_of('\\', pos-1);

        return ((last == std::string::npos && pos % 2 == 1)
                || (last != std::string::npos && (pos - last) % 2 == 0));
    }
}
