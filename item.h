#ifndef ITEM_H
#define ITEM_H

#include <QString> // Include for QString

class Item {
public:
    // Constructor
    Item(int id, const QString& name, int quantity, double price, const QString& category, const QString& supplier);

    // Getters
    int getId() const;
    QString getName() const;
    int getQuantity() const;
    double getPrice() const;
    QString getCategory() const;
    QString getSupplier() const;

    // Setters
    void setName(QString name);
    void setCategory(QString category);
    void setSupplier(QString supplier);
    void setQuantity(int qty);
    void setPrice(double p);

private:
    int m_id; // ID is taken as int
    QString m_name;
    int m_quantity;
    double m_price;
    QString m_category;
    QString m_supplier;
};

#endif // ITEM_H
