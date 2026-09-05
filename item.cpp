// --- Function definition for item.h ---

#include "Item.h"

// Constructor
Item::Item(int id,
           const QString& name,
           double quantity,
           Unit unitType,
           double price,
           const QString& barcode)
    : m_id(id),
      m_name(name),
      m_quantity(quantity),
      m_unitType(unitType),
      m_price(price),
      m_barcode(barcode)
{
}

// Getters
int Item::getId() const { return m_id; }
QString Item::getName() const { return m_name; }
double Item::getQuantity() const { return m_quantity; }
double Item::getPrice() const { return m_price; }
Unit Item::getUnit() const { return m_unitType; }
QString Item::getBarcode() const { return m_barcode; }


// Setters
void Item::setName(QString name) { m_name = name; }
void Item::setQuantity(double qty) { m_quantity = qty; }
void Item::setPrice(double p) { m_price = p; }
void Item::setUnit(Unit unit) { m_unitType = unit; }
void Item::setBarcode(QString barcode) { m_barcode = barcode; }

