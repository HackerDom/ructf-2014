#include "input.h"
#include "queue.h"

#include <iostream>
#include <vector>


const size_t SECOND = 1;
const size_t TEN_SECONDS = 10;
const size_t MINUTE = 60;
const size_t HOUR = 60*60;

int main(void) {
    std::vector<NamedPair> snap;
    while (GetCounters(&snap)) {
        std::cout << snap.size() << " ";
    }

    Queue<Pair,
          Length<SECOND>,
          Length<TEN_SECONDS>,
          Length<MINUTE>,
          Length<HOUR>> queue(snap[0].counts, HOUR);

    queue.Update(snap[0].counts);
}
