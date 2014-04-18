#include "input.h"
#include <iostream>

bool GetCounters(std::vector<NamedPair>* output) {
    using std::cin;

    size_t count;
    cin >> count;
    if (!count)
        return false;

    output->resize(count);

    for (size_t i = 0; i != count; ++i) {
        cin >> (*output)[i].name
            >> (*output)[i].counts.packets
            >> (*output)[i].counts.bytes;
    }

    return true;
}
