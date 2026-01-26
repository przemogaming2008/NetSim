#ifndef PACKAGE_HPP
#define PACKAGE_HPP

#include "types.hpp"
#include <set>
#include <algorithm>

class Package {
    
public:
    // Konstruktor bez ID - automatyczne przydzielanie
    Package();

    // Konstruktor z ręcznym ID
    explicit Package(ElementID ID);

    // Move constructor
    Package(Package&& package) noexcept;

    // Move assignment
    Package& operator=(Package&& package) noexcept;

    // Get ID
    ElementID get_id() const { return ID_; }

    // Destruktor
    ~Package();

private:
    ElementID ID_;

    // Statyczne zbiory dla unikalności
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;

    // Pomocnicza funkcja do przydzielania ID
    static ElementID get_next_id();
};

#endif // PACKAGE_HPP