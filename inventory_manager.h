#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

// Class reference to edit inventory section

#include "Item.h"


#include <QVector>
#include <QString>
#include <QDebug>
#include <QSqlQuery>

using namespace std;

enum class ChangeTypeForDB {
    Added,
    Modified,
    Deleted

};

struct InventoryChangesForDB{
    ChangeTypeForDB type;
    Item item;

};

class InventoryManager {

protected:

    // Main Item Vector - Store Items as in-memory data
    QVector<Item> items;

    QString m_loadDirectory;
    QVector <QString> m_inventory_logs;



public:

    QVector < InventoryChangesForDB> DB_queries_saver;

     InventoryManager(); // When making inventory manager object, data are load.
    ~InventoryManager();


    // load and save data to DB
    void load_data();
    void save_data();

    void update_DB_with_new_inventory_queries();




    // Function for changing items vector + DB
    void add_quantity(int id, int quantity);   // update quantity of exsisting item - useful when item is remove from bill

    void add_new_item(Item new_item); // adding to both in-memory vector and DB query vector

    void remove_item_by_index(int index);
    void edit_item_by_index(int index , QString new_name , QString new_category , QString new_supplier, int new_quantity , double new_price);

    void add_remove_item_to_db_query(Item item);
    void add_edit_item_to_db_query(Item updated_item);

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
