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

std::string GcodeQueue::peek() {
    std::unique_lock<std::mutex> lock(mutex);
    if (queue.empty()) {
        return "";  // no hay nada que mirar
    }
    return queue.front();  // devuelve sin quitarlo
}

/*std::vector<std::string> GcodeQueue::tryPeekN(size_t n)  {
    std::unique_lock<std::mutex> lock(mutex);

    std::vector<std::string> result;
    result.reserve(n);

    if (queue.empty()) {
        return result;  // devuelve vacío si no hay nada
    }

    // Acceder al contenedor subyacente (std::deque)
    const auto& container = queue._Get_container();  // ⚠️ no estándar, depende del compilador

    for (size_t i = 0; i < n && i < container.size(); ++i) {
        result.push_back(container[i]);
    }

    return result;
}*/

bool GcodeQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}
