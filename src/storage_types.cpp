#include "storage_types.hpp"

Package PackageQueue::pop() {
    Package package;

    if (this->type_ == PackageQueueType::LIFO) 
    {
        package = std::move(this->packages_.back());
        this->packages_.pop_back();
    } else 
    {
        package = std::move(this->packages_.front());
        this->packages_.pop_front();
    }

    return package;
}