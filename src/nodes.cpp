#include "nodes.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver* r)
{
    double num = double(preferences_t_.size());
    if (num == 0)
    {
        preferences_t_[r] = 1;
    }
    else
    {
        for (auto &receiver: preferences_t_)
        {
            receiver.second = 1 / (num + 1);
        }
        preferences_t_[r] = 1 / (num + 1);
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r)
{
    double num = double(preferences_t_.size());
    if (num > 1)
    {
        for (auto &receiver: preferences_t_)
        {
            if (receiver.first != r)
            {
                receiver.second = 1 / (num - 1);
            }
        }
    }
    preferences_t_.erase(r);
}

IPackageReceiver* ReceiverPreferences::choose_receiver()
{
    auto probability = pg_();
    if (probability >= 0 && probability <= 1)
    {
        double distribution = 0.0;
        for (auto &rec: preferences_t_)
        {
            distribution += rec.second;
            if (distribution < 0 || distribution > 1)
            {
                return nullptr;
            }
            if (probability <= distribution)
            {
                return rec.first;
            }
        }
        return nullptr;
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
    } 
    else 
    {
        if (t - t_ + 1 == pd_) 
        {
            push_package(Package(sending_buffer_.value().get_id()));
            sending_buffer_.reset();
            if (!q_->empty()) 
            {
                sending_buffer_.emplace(q_->pop());
            }
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
        push_package(Package());
        sending_buffer_.emplace(id_);
        t_ = t;
    } 
    else 
    {
        if (t - di_ == t_) 
        {
            push_package(Package());
        }
    }
}
