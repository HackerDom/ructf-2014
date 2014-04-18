#include "input.h"
#include "queue.h"

#include <iostream>
#include <vector>


typedef Length<1>     Second;
typedef Length<10>    TenSeconds;
typedef Length<60*5>  FiveMinutes;
typedef Length<60*60> Hour;

typedef Queue<Pair, Second, TenSeconds, FiveMinutes, Hour> MainQueue;

int main(void) {
    using std::vector; using std::string; using std::pair;
    using std::cin; using std::cout; using std::cerr; using std::endl;

    vector<NamedPair> snapshot;
    GetCounters(&snapshot);
    size_t size = snapshot.size();

    vector<pair<string, MainQueue>> queues;
    queues.reserve(size);
    for (size_t i = 0; i != size; ++i) {
        queues.push_back(std::make_pair(
            snapshot[i].name,
            MainQueue::Create(snapshot[i].counts)));
    }

    while (GetCounters(&snapshot)) {
        size_t size = snapshot.size();
        vector<pair<size_t, Pair>> counters;
        for (size_t i = 0; i != size; ++i) {
            pair<string, MainQueue>& queue = queues[i];
            if (queue.first != snapshot[i].name) {
                cerr << "Wrong name: " << snapshot[i].name
                     << ", was " << queue.first << endl;
                return 1;
            }
            queue.second.Update(snapshot[i].counts);
            queue.second.GetDiffs(&counters);

            cout << queue.first << ':';
            for (size_t j = 0; j != counters.size(); ++j) {
                cout << counters[j].first << ':' << counters[j].second.packets;
                if (j != counters.size() - 1) {
                    cout << '|';
                }
            }
            if (i != size - 1) {
                cout << ',';
            }
        }

        cout << endl;
    }
}
