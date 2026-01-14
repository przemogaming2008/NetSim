#include "Package.hpp"

// statyczne definicje
std::set<ElementID> Package::assigned_IDs;
std::set<ElementID> Package::freed_IDs;

// Pomocnicza funkcja
ElementID Package::get_next_id() {
    if (!freed_IDs.empty()) {
        ElementID id = *freed_IDs.begin();
        freed_IDs.erase(freed_IDs.begin());
        return id;
    }

    ElementID id = 0;
    if (!assigned_IDs.empty()) {
        id = *assigned_IDs.rbegin() + 1;
    }
    return id;
}

// Konstruktor bez ID
Package::Package() : ID_(get_next_id()) {
    assigned_IDs.insert(ID_);
}

// Konstruktor z ID
Package::Package(ElementID ID) : ID_(ID) {
    if (assigned_IDs.find(ID) != assigned_IDs.end()) {
        throw std::runtime_error("ID already assigned!");
    }
    assigned_IDs.insert(ID_);
}

// Move constructor
Package::Package(Package&& other) noexcept : ID_(other.ID_) {
    other.ID_ = get_next_id();   // opcjonalnie daj mu nowe ID, żeby nie dublować w assigned
    assigned_IDs.insert(ID_);
}

// Move assignment
Package& Package::operator=(Package&& other) noexcept {
    if (this != &other) {
        // zwolnij stare ID
        freed_IDs.insert(ID_);
        assigned_IDs.erase(ID_);

        // przejmij ID
        ID_ = other.ID_;
        assigned_IDs.insert(ID_);

        // opcjonalnie nadaj nowe ID dla other
        other.ID_ = get_next_id();
    }
    return *this;
}

// Destruktor
Package::~Package() {
    assigned_IDs.erase(ID_);
    freed_IDs.insert(ID_);
}
