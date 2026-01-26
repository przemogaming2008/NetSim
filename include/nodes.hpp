#ifndef NODES_HPP
#define NODES_HPP

#include "helpers.hpp"
#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"

#include <memory>
#include <optional>
#include <map>

class IPackageReceiver
{
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id(void) const = 0;
    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences
{
public:
    ReceiverPreferences(ProbabilityGenerator pg = probability_generator)
    : pg_(std::move(pg)) {}

    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const { return this->preferences_t_; }

    const_iterator begin() const { return preferences_t_.cbegin(); }
    const_iterator end() const { return preferences_t_.cend(); }
    const_iterator cbegin() const { return preferences_t_.cbegin(); }
    const_iterator cend() const { return preferences_t_.cend(); }
private:
    ProbabilityGenerator pg_;
    preferences_t preferences_t_;
};

class PackageSender
{
public:
    PackageSender() = default;
    PackageSender(PackageSender&& move) = default;
    virtual ~PackageSender() = default;
    
    void send_package();
    const std::optional<Package>& get_sending_buffer() const { return sending_buffer_; };
    
protected:
    void push_package(Package&& moved) { sending_buffer_.emplace(std::move(moved)); };
    
    ReceiverPreferences receiver_preferences_;
    std::optional<Package> sending_buffer_ = std::nullopt;
};

class Worker: public IPackageReceiver, public PackageSender
{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
    : PackageSender(), id_(id), pd_(pd), q_(std::move(q)) {}

    void do_work(Time t );
    void receive_package(Package&& p) override;

    ElementID get_id() const override { return id_; };
    TimeOffset get_processing_duration() const { return pd_; };
    TimeOffset get_package_processing_start_time() const { return t_; };

private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    Time t_;
};

class Storehouse : public IPackageReceiver
{
public:
    Storehouse(ElementID id,
    std::unique_ptr<IPackageStockpile> d);

    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; };
private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

class Ramp : public PackageSender
{
public:
    Ramp(ElementID id, TimeOffset di)
    : PackageSender(), id_(id), di_(di) {}

    void deliver_goods(Time t);
    TimeOffset get_delivery_interval() const {return di_;}
    ElementID get_id() const {return id_;}

private:
    ElementID id_;
    TimeOffset di_;
    Time t_;
};

#endif //NODES_HPP