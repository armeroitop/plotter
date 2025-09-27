#pragma once

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

class GcodeQueue {
public:
    //void push(const std::string& command);
    void push_back(const std::string& command);
    void push_front(const std::string& command);
    std::string pop();   // bloquea y saca
    std::string peek();  // mira el siguiente sin sacarlo
    //std::vector<std::string> tryPeekN(size_t n);
    bool empty() const;

private:
    std::deque<std::string> queue;
    mutable std::mutex mutex;
    std::condition_variable condVar;
};