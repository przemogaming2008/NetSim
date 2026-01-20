#ifndef NODES_HPP
#define NODES_HPP

#include "helpers.hpp"
#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"

#include <memory>
#include <optional>

class IPackageReceiver
{
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id(void) = 0;
    virtual ~IPackageReceiver() = default;
};

class Worker
{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
    : id_(id), pd_(pd), q_(std::move(q)) {}

    void do_work(Time t);
    TimeOffset get_processing_duration(void);
    Time get_package_processing_start_time(void);

private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
};

class Storehouse : IPackageStockpile
{
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d)
    : id_(id), d_(std::move(d)) {}
private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};


class ReceiverPreferences
{
public:
    ReceiverPreferences(ProbabilityGenerator pg)
    : pg_(pg) {}

    void add_Receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver(void);
    preferences_t& get_preferences(void);

private:
    ProbabilityGenerator pg_;

};

class PackageSender
{
public:
    PackageSender(PackageSender&&);

    void send_package(void);
    std::optional<Package>& get_sending_buffer(void);
    
    static void push_package(Package&&);
private:
    ReceiverPreferences receiver_preferences_;
};

class Ramp
{
public:
    Ramp(ElementID id, TimeOffset di)
    : id_(id), di_(di) {}

    void deliver_goods(Time t);
    TimeOffset get_delivery_interval(void);
    ElementID get_id(void);

private:
    ElementID id_;
    TimeOffset di_;
};








#endif //NODES_HPP