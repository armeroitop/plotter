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
    std::string pop();
    bool empty() const;

private:
    std::deque<std::string> queue;
    mutable std::mutex mutex;
    std::condition_variable condVar;
};