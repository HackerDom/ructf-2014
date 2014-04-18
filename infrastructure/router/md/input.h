#pragma once

#include <cstdint>
#include <string>
#include <vector>


typedef uint64_t PacketCount;
typedef uint64_t ByteCount;

struct Pair {
    Pair(PacketCount packets = 0, ByteCount bytes = 0)
        : packets(packets), bytes(bytes) {
    }

    Pair operator-(const Pair& rhs) const {
        return Pair(packets - rhs.packets, bytes - rhs.bytes);
    }

    Pair operator+(const Pair& rhs) const {
        return Pair(packets + rhs.packets, bytes + rhs.bytes);
    }

    bool operator<(const Pair& rhs) const {
        return (packets < rhs.packets) ||
               (bytes < rhs.bytes);
    }

    PacketCount packets;
    ByteCount   bytes;
};

struct NamedPair {
    std::string name;
    Pair counts;
};

bool GetCounters(std::vector<NamedPair>* output);
