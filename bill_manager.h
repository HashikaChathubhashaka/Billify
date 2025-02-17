#ifndef BILL_MANAGER_H
#define BILL_MANAGER_H

#include "inventory_manager.h"
#include "Bill_Item.h"
#include <QVector>
#include <QString>
#include <QDebug>

class BillManager : public InventoryManager {

private:
    QVector<BillItem> m_bill_items; // To store the each bill Item

    double total_price{ 0 }; // total price of the bill
    QString customer_name;
    QString m_saveDirectory;
    QString m_shopName = "My Shop"; // default

public:

    BillManager();

    //setters
    void setCustomerName(QString name);
    void setSaveDirectory(QString directory);
    void setShopName(QString name);

    //getters
    QString getSaveDirectory() const;
    QString getCustomerName();
    QVector <BillItem> Get_Bill_list(); // to get the items in the bill list
    QString getShopName();

    // Bill Functionalities
    void Add_to_Bill(int id, int Quantity = 1);
    void Remove_from_Bill(int id, int Quantity = 1);
    void emptyingBill();
    bool generateBillPDF();
    void generateLogsPDF();

    // For debugging and Get total of Bill -> Get bill Items using QDebug and returning total of bill
    double get_bill_total();


};

#endif // BILL_MANAGER_H
