#ifndef STORAGE_TYPES_HPP
#define STORAGE_TYPES_HPP
#include "package.hpp"

#include <list>

enum class PackageQueueType {
    FIFO,
    LIFO
};

class IPackageStockpile 
{
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&& package) = 0;
    
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;

    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;

    virtual ~IPackageStockpile() = default;
};

class IPackageQueue : public IPackageStockpile
{
public:
    virtual Package pop() = 0;

    virtual PackageQueueType get_queue_type() const = 0;

};

class PackageQueue : public IPackageQueue
{
public:
    PackageQueue() = delete;

    explicit PackageQueue(PackageQueueType type) : type_(type), packages_() {}

    ~PackageQueue() override = default;

    void push(Package&& moved) override {
        this->packages_.emplace_back(std::move(moved));
    }
    bool empty() const override {
        return this->packages_.empty();
    }
    size_t size() const override {
        return this->packages_.size();
    }
    const_iterator begin() const override {
        return this->packages_.cbegin();
    }
    const_iterator end() const override {
        return this->packages_.cend();
    }
    const_iterator cbegin() const override {
        return this->packages_.cbegin();
    }
    const_iterator cend() const override {
        return this->packages_.cend();
    }

    Package pop() override;

    PackageQueueType get_queue_type() const override {
        return this->type_;
    };

private:
    PackageQueueType type_;
    std::list<Package> packages_;
};

#endif // STORAGE_TYPES_HPP