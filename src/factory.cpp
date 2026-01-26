#include "factory.hpp"
#include "nodes.hpp"


bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {
    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("Sender does not have any receivers");
    }

    for (const auto& receiver : sender->receiver_preferences_.get_preferences()) {
        if (receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE) {
            return true;
        } else if (receiver.first->get_receiver_type() == ReceiverType::WORKER) {
            PackageSender* sendrecv_ptr = dynamic_cast<PackageSender*>(dynamic_cast<class Worker*>(receiver.first));

            if (sendrecv_ptr == sender) {
                continue;
            }

            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED && has_reachable_storehouse(sendrecv_ptr, node_colors)) {
                return true;
            }
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;
    return false;
}

void Factory::remove_worker(ElementID id){
    Worker* node = &(*cont_w.find_by_id(id));

    std::for_each(cont_w.begin(), cont_w.end(), [node](Worker& worker) {
        worker.receiver_preferences_.remove_receiver(node);
    });

    std::for_each(cont_r.begin(), cont_r.end(), [node](Ramp& ramp) {
        ramp.receiver_preferences_.remove_receiver(node);
    });

    cont_w.remove_by_id(id);
}

void Factory::remove_storehouse(ElementID id)
{
    Storehouse* node = &(*cont_s.find_by_id(id));
    std::for_each(cont_w.begin(), cont_w.end(), [&node](Worker& ramp) {
        ramp.receiver_preferences_.remove_receiver(node);
    });

    std::for_each(cont_w.begin(), cont_w.end(), [&node](Worker& worker) {
        worker.receiver_preferences_.remove_receiver(node);
    });
    cont_s.remove_by_id(id);
}

void Factory::do_deliveries(Time t) {
    for (auto &ramp : cont_r)
        ramp.deliver_goods(t);
}

void Factory::do_work(Time t ) {
    for (auto& worker : cont_w)
        worker.do_work(t);
}

void Factory::do_package_passing() {
    for (auto &ramp : cont_r)
        ramp.send_package();

    for (auto &worker : cont_w)
        worker.send_package();
}
bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> kolor;

    auto set_unvisited_colors = [&kolor](const auto& container) {
        for (const auto& item : container) {
            const PackageSender* sender = dynamic_cast<const PackageSender*>(&item);
            kolor[sender] = NodeColor::UNVISITED;
        }
    };

    set_unvisited_colors(cont_w);
    set_unvisited_colors(cont_r);

    try {
        for (const auto& ramp : cont_r) {
            const PackageSender* sender = dynamic_cast<const PackageSender*>(&ramp);
            has_reachable_storehouse(sender, kolor);
        }
    } catch (const std::logic_error&) {
        return false;
    }

    return true;
}

std::vector<std::string> character_split(const std::string& splittable_str, char delimiter) {
    std::stringstream parameter_stream(splittable_str);
    std::string part;
    std::vector<std::string> result;

    while(std::getline(parameter_stream, part, delimiter)) {
        result.push_back(part);
    }

    return result;
}

ParsedLineData parse_line(std::string& line) {
    std::vector<std::string> tokens;
    std::string token;

    std::istringstream token_stream(line);
    char delimiter = ' ';
    while (std::getline(token_stream, token, delimiter))
        tokens.push_back(token);

    ParsedLineData parsed_data;

    std::map<std::string, ElementType> element_types{
            {"LOADING_RAMP", ElementType::RAMP},
            {"WORKER", ElementType::WORKER},
            {"STOREHOUSE", ElementType::STOREHOUSE},
            {"LINK", ElementType::LINK},
    };

    try {
        parsed_data.element_type = element_types.at(tokens[0]);

        std::for_each(std::next(tokens.cbegin()), tokens.cend(), [&](const std::string& parameter_str) {
            auto key_value = character_split(parameter_str, '=');
            parsed_data.parameters[key_value[0]] = key_value[1];
        });
    } catch (std::out_of_range& ex) {
        throw std::exception();
    }

    return parsed_data;
}


PackageQueueType get_package_queue_type(std::string& package_queue_type_str) {
    std::map<std::string, PackageQueueType> str_type_map{
            {"LIFO", PackageQueueType::LIFO},
            {"FIFO", PackageQueueType::FIFO}
    };

    return str_type_map.at(package_queue_type_str);
}

void link(Factory& factory, const std::map<std::string, std::string>& params) {
    // TODO: implementacja LINK
}

Factory load_factory_structure(std::istream& is) 
{
    Factory factory;

    std::string line;

    while (std::getline(is, line)) {
        if (line.empty() || line[0] == ';')
            continue;

        ParsedLineData parsed = parse_line(line);

        switch(parsed.element_type) {
            case ElementType::RAMP: {
                ElementID element_id = std::stoi(parsed.parameters.at("id"));
                TimeOffset delivery_interval = std::stoi(parsed.parameters.at("delivery-interval"));
                Ramp ramp(element_id, delivery_interval);

                factory.add_ramp(std::move(ramp));
                break;
            }
            case ElementType::WORKER: {
                ElementID element_id = std::stoi(parsed.parameters.at("id"));
                TimeOffset processing_time = std::stoi(parsed.parameters.at("processing-time"));
                PackageQueueType package_queue_t = get_package_queue_type(parsed.parameters.at("queue-type"));
                Worker worker(element_id, processing_time, std::make_unique<PackageQueue>(package_queue_t));

                factory.add_worker(std::move(worker));
                break;
            }
            case ElementType::STOREHOUSE: {
                ElementID element_id = std::stoi(parsed.parameters.at("id"));
                Storehouse storehouse(element_id);

                factory.add_storehouse(std::move(storehouse));
                break;
            }
            case ElementType::LINK: {
                link(factory, parsed.parameters);
                break;
            }
        }
    }

    return factory;
}

std::string queue_type_str(PackageQueueType type) {
    switch(type) {
        case PackageQueueType::FIFO: return "FIFO";
        case PackageQueueType::LIFO: return "LIFO";
    }
    return "UNKNOWN";
}

void link_stream_fill(std::stringstream& link_stream, const PackageSender& package_sender, ElementID package_sender_id, std::string&& package_sender_name) {
    auto prefs = package_sender.receiver_preferences_.get_preferences();

    std::for_each(prefs.cbegin(), prefs.cend(), [&](const std::pair<IPackageReceiver*, double>& key_value) {
        link_stream << "LINK src=" << package_sender_name << "-" << package_sender_id << " ";
        const IPackageReceiver* package_receiver = key_value.first;
        ReceiverType receiver_type = package_receiver->get_receiver_type();

        std::string receiver_type_str = receiver_type == ReceiverType::WORKER ? "worker" : "store";

        link_stream << "dest=" << receiver_type_str << "-" << package_receiver->get_id() << '\n';
        std::cout << link_stream.str();
    });
}
