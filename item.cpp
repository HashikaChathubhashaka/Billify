// --- Function definition for item.h ---

#include "Item.h"

// Constructor
Item::Item(int id, const QString& name, int quantity, double price, const QString& category, const QString& supplier)
    : m_id(id), m_name(name), m_quantity(quantity), m_price(price), m_category(category), m_supplier(supplier) {}

// Getters
int Item::getId() const { return m_id; }
QString Item::getName() const { return m_name; }
int Item::getQuantity() const { return m_quantity; }
double Item::getPrice() const { return m_price; }
QString Item::getCategory() const { return m_category; }
QString Item::getSupplier() const { return m_supplier; }

// Setters
void Item::setName(QString name){m_name = name;}
void Item::setQuantity(int qty) { m_quantity = qty; }
void Item::setPrice(double p) { m_price = p; }
void Item::setCategory(QString category){m_category = category;}
void Item::setSupplier(QString supplier){m_supplier = supplier;}
