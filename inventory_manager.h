#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H


#include "Item.h"

#include <QVector>
#include <QString>
#include <QDebug>

using namespace std;

class inventory_manager {

protected:

    QVector<Item> items;
    QString m_loadDirectory;
    QVector <QString> m_inventory_logs;

public:

    inventory_manager(); // When making inventory manager object, data are load.
    ~inventory_manager();


    // load and save data to CSV file
    void load_data();
    void save_data();


    // Function for changing items properties
    void add_quantity(int id, int quantity);   // update quantity of exsisting item
    void add_new_item(Item new_item);
    void remove_item_by_index(int index);
    void edit_item_by_index(int index , QString new_name , QString new_category , QString new_supplier, int new_quantity , double new_price);


    void add_item_to_inventory_log(QString log);
    void clear_inventory_log();
    bool is_inventory_log_empty();


    //Getters
    QVector <Item>& getItems()  ;
    QString getloadDirectory() const;
    QVector <QString>& getInventoryLogs();

    //Setters
    void setloadDirectory();





};

#endif // INVENTORY_MANAGER_H
