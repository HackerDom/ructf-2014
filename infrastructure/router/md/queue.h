#pragma once
#include "input.h"
#include <algorithm>
#include <cassert>
#include <vector>

template<typename Data>
class BaseQueue {
public:
    BaseQueue(const Data& initial, size_t length)
        : last_(initial), data_(length), pos_() {
    }

    Data GetLast(ssize_t n) const {
        if (!pos_) {
            return Data();
        } else {
            size_t size = data_.size();
            if (pos_ < size) {
                assert(n < pos_);
                return data_[pos_ - n - 1];
            } else {
                size_t pos = size + n < pos_ ? pos_ - size : pos_;
                return data_[pos - n - 1];
            }
        }
    }

protected:
    void Update(const Data& data) {
        size_t size = data_.size();
        Data diff = last_ < data ? data - last_ : Data();
        if (pos_ < size) {
            // partial fill
            data_[pos_++] = diff;
        } else {
            // full fill, start to forget
            data_[pos_ - size] = diff;
            if (++pos_ == 2*size) {
                // start from the beginning
                pos_ -= size;
            }
        }
        last_ = data;
    }

private:
    Data last_;
    std::vector<Data> data_;
    size_t pos_;
};

template<size_t length>
struct Length {
    static const size_t value = length;
};
template<size_t length>
size_t GetLength(Length<length>) {
    return length;
}

template<typename Data, typename Length>
class Counter {
    typedef BaseQueue<Data> Queue;
public:
    Counter() : cur_(), sum_() {
    }

    void Pop(const Queue& queue) {
        if (cur_ == GetLength(Length()))
            sum_ = sum_ - queue.GetLast(cur_-1);
    }

    void Push(const Queue& queue) {
        sum_ = sum_ + queue.GetLast(0);
        if (cur_ < GetLength(Length()))
            ++cur_;
    }

    std::pair<size_t, Data> GetDiff() const {
        return std::make_pair(cur_, sum_);
    }

    size_t cur_;
    Data sum_;
};

template<typename Data, typename... Tail>
class Queue;

template<typename Data> class Queue<Data> : public BaseQueue<Data> {
    typedef BaseQueue<Data> BaseQueue;
public:
    void Update(const Data& data) {
        BaseQueue::Push(data);
    }

    void GetDiffs(std::vector<std::pair<size_t, Data>>* counters) {
        counters->clear();
    }

protected:
    Queue(Data& initial, size_t length) : BaseQueue(initial, length) {
    }

    static size_t GetSize() {
        return 0;
    }

    void Push() { }
    void Pop() { }
};

template<typename Data, typename Length, typename... Tail>
class Queue<Data, Length, Tail...>
    : public Queue<Data, Tail...>,
      public Counter<Data, Length> {

    typedef Queue<Data, Tail...> NextQueue;
    typedef Counter<Data, Length> Counter;

public:
    void Update(const Data& data) {
        Pop();
        BaseQueue<Data>::Update(data);
        Push();
    }

    static Queue Create(Data& initial) {
        return Queue(initial, GetSize());
    }

    void GetDiffs(std::vector<std::pair<size_t, Data>>* counters) {
        NextQueue::GetDiffs(counters);
        counters->push_back(Counter::GetDiff());
    }

protected:
    Queue(Data& initial, size_t length) : NextQueue(initial, length) {
    }

    static size_t GetSize() {
        return std::max(GetLength(Length()), NextQueue::GetSize());
    }

    void Push() {
        Counter::Push(*this);
        NextQueue::Push();
    }

    void Pop() {
        Counter::Pop(*this);
        NextQueue::Pop();
    }
};
