#include "nodes.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver* r)
{
    preferences_t_[r] = 1.0;
    double prob = 1.0 / preferences_t_.size();

    for (auto& p : preferences_t_) 
    {
        p.second = prob;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r)
{
    preferences_t_.erase(r);
    if (!preferences_t_.empty()) 
    {
        double prob = 1.0 / preferences_t_.size();
        for (auto& p : preferences_t_) 
        {
            p.second = prob;
        }
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver()
{
    double p = pg_();
    double acc = 0.0;

    for (auto& el : preferences_t_) 
    {
        acc += el.second;
        if (p <= acc) return el.first;
    }
    return nullptr;
}

void PackageSender::send_package()
{
    IPackageReceiver *receiver;
    if (sending_buffer_) 
    {
        receiver = receiver_preferences_.choose_receiver();
        receiver->receive_package(std::move(*sending_buffer_));
        sending_buffer_.reset();
    }
}

void Worker::do_work(Time t)
{
    if (!sending_buffer_ && !q_->empty()) 
    {
        sending_buffer_.emplace(q_->pop());
        t_ = t;
        return;
    }

    if (sending_buffer_ && t - t_ + 1 >= pd_) 
    {
        push_package(std::move(*sending_buffer_));
        sending_buffer_.reset();

        if (!q_->empty()) 
        {
            sending_buffer_.emplace(q_->pop());
            t_ = t;
        }
    }
}
    
void Worker::receive_package(Package &&p)
{
    q_->push(std::move(p));
}

void Storehouse::receive_package(Package &&p)
{
    d_->push(std::move(p));
}

void Ramp::deliver_goods(Time t)
{
    if (!sending_buffer_) 
    {
        sending_buffer_.emplace(Package());
        t_ = t;
    }
    else if (t - t_ + 1 >= di_) 
    {
        push_package(std::move(*sending_buffer_));
        sending_buffer_.reset();

        sending_buffer_.emplace(Package());
        t_ = t;
    }
}

Storehouse::Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d)
    : id_(id), d_(std::move(d))
{}