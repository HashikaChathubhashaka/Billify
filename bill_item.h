#ifndef BILL_ITEM_H
#define BILL_ITEM_H

#include <QString>



// since this is a small class, Define it in the header file
class Bill_Item {


public:

    // Constructor
    explicit Bill_Item(int id, QString Name, int qty, double price)
        : m_ID(id), m_name(Name), m_quantity(qty), m_price_per_unit(price) {

        m_total_price = m_quantity * m_price_per_unit; // calculate total price of one Item when it added to bill
    }

    // Getters
    int getID() const { return m_ID; }
    QString getName() const { return m_name; }
    int getQuantity() const { return m_quantity; }
    double getPricePerUnit() const { return m_price_per_unit; }
    double getTotalPrice() const { return m_total_price; }

    // Setter
    void setQuantity(int qty) {
        m_quantity = qty;
        m_total_price = m_quantity * m_price_per_unit; // Update total price when quantity changes
    }

private:
    int m_ID;
    QString m_name;
    int m_quantity;
    double m_price_per_unit;
    double m_total_price; // total price of an item (same item)

};


#endif // BILL_ITEM_H
