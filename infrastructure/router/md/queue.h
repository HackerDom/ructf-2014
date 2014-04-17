#pragma once
#include "input.h"
#include <vector>


template<typename Data>
class BaseQueue {
public:
    BaseQueue(const Data& initial, size_t length)
        : last_(initial), data_(length), pos_() {
    }

    Data GetLast(size_t n) const {
        if (!pos_) {
            return Data();
        } else {
            size_t size = data_.size();
            size_t pos = pos_ < size ?
                (n < pos_ ? n : pos_) :
                (size + n < pos_ ? pos_ : pos_ + size);
            return data_[pos - n - 1];
        }
    }

protected:
    void Update(const Data& data) {
        size_t size = data_.size();
        if (pos_ < size) {
            // partial fill
            data_[pos_++] = data - last_;
        } else if (pos_ < 2*size) {
            // full fill, start to forget
            data_[pos_ - size] = data - last_;
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

template<typename Data, typename Length>
class Counter {
    typedef BaseQueue<Data> Queue;
public:
    Counter() : n_(), cur_(), sum_() {
    }

    void Pop(const Queue& queue) {
        sum_ = sum_ - queue.GetLast(cur_);
    }

    void Push(const Queue& queue) {
        sum_ = sum_ + queue.GetLast(0);
        if (cur_ < n_)
            ++cur_;
    }

    std::pair<size_t, Data> GetDifference() const {
        return std::make_pair(cur_, sum_);
    }

    const size_t n_;

    size_t cur_;
    Data sum_;
};

template<typename Data, typename... Tail>
class Queue;

template<typename Data> class Queue<Data> : public BaseQueue<Data> {
    typedef BaseQueue<Data> BaseQueue;
public:
    Queue(Data& initial, size_t length) : BaseQueue(initial, length) {
    }

    void Update(const Data& data) {
        BaseQueue::Push(data);
    }

protected:
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
    Queue(Data& initial, size_t length) : NextQueue(initial, length) {
    }

    void Update(const Data& data) {
        Pop();
        Update(data);
        Push();
    }

protected:
    void Push() {
        Counter::Push(*this);
        NextQueue::Push();
    }

    void Pop() {
        Counter::Pop(*this);
        NextQueue::Pop();
    }
};
