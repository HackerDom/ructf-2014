#include "input.h"
#include <iostream>

bool GetCounters(std::vector<NamedPair>* output) {
    size_t count;
    std::cin >> count;
    if (!count)
        return false;

    output->resize(count);

    for (size_t i = 0; i != count; ++i) {
        std::string pair;
        std::cin >> pair;

        size_t dash = pair.find('-');
        if (dash == std::string::npos)
            return false;

        (*output)[i].from = pair.substr(0, dash);
        (*output)[i].to   = pair.substr(dash + 1);

        std::cin >> (*output)[i].counts.packets;
        std::cin >> (*output)[i].counts.bytes;
    }

    return true;
}
