#ifndef ITEM_H
#define ITEM_H

#include <QString> // Include for QString

// Main Item class

enum class Unit : int
{
    Piece = 0,
    Kilogram = 1,
    Meter = 2
};



class Item {
public:
    // Constructor
    Item(int id, const QString& name, 
        double quantity, Unit unitType ,
        double price, const QString& barcode
        );

    // Getters
    int getId() const;
    QString getName() const;
    double getQuantity() const;
    double getPrice() const;
    QString getBarcode() const;
    Unit getUnit() const;


    // Setters
    void setName(QString name);
    void setBarcode(QString barcode);
    void setQuantity(double qty);
    void setPrice(double p);
    void setUnit(Unit unit);


private:
    int m_id; // (PK)
    QString m_name; // (NN)
    double m_quantity; // (NN)
    double m_price; // per quantity price (NN)
    QString m_barcode; 
    Unit m_unitType; // (NN)


};

#endif // ITEM_H
