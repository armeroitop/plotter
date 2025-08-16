// src/fifo/GcodeQueue.cpp
#include "GcodeQueue.hpp"

void GcodeQueue::push_back(const std::string& command) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push_back(command);
    condVar.notify_one(); // Despierta al consumidor
}

void GcodeQueue::push_front(const std::string& command) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push_front(command);
    condVar.notify_one(); // Despierta al consumidor
}

std::string GcodeQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex);
    condVar.wait(lock, [this]() { return !queue.empty(); }); // Espera si está vacía
    std::string command = queue.front();
    queue.pop_front();
    return command;
}

bool GcodeQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}
