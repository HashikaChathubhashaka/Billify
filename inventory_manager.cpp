// --- Function definition for inventory manager.h ---

#include "inventory_manager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

// Constructor:  data on initialization
InventoryManager::InventoryManager() {
    qDebug() << "InventoryManager constructor";

    load_data();

    QSqlDatabase db = QSqlDatabase::database();  // Use existing connection if available

    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE");  // Only add if no existing connection
        db.setDatabaseName("database.db");
    }

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Error: Cannot open database!" << db.lastError().text();
        return;
    }

}

InventoryManager::~InventoryManager() {


}


// Load inventory data from DB
void InventoryManager::load_data() {

    QSqlDatabase db = QSqlDatabase::database();  // Use existing connection if available

    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE");  // Only add if no existing connection
        db.setDatabaseName("database.db");
    }

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Error: Cannot open database!" << db.lastError().text();
        return;
    }

    QSqlQuery query("SELECT ID, Name, Quantity, Price, Category, Supplier FROM Inventory");

    items.clear(); // Clear existing data before loading

    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        int quantity = query.value(2).toInt();
        double price = query.value(3).toDouble();
        QString category = query.value(4).toString();
        QString supplier = query.value(5).toString();


        Item item(id, name, quantity, price, category, supplier);
        items.push_back(item);

        qDebug() << "ed item:" << id << name << quantity << price << category << supplier;
    }

    db.close();
    qDebug() << "Data ing from SQLite completed successfully.";
}

void InventoryManager::save_data() {

    QSqlDatabase db = QSqlDatabase::database();  // Use existing connection if available

    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE");  // Only add if no existing connection
        db.setDatabaseName("database.db");
    }

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Error: Cannot open database!" << db.lastError().text();
        return;
    }

    QSqlQuery query;

    // **Step 1: Create a set of existing item IDs in memory**
    QSet<int> currentItemIds;
    for (const auto &item : items) {
        currentItemIds.insert(item.getId());
    }

    // **Step 2: Get all item IDs from the database**
    QSqlQuery fetchQuery("SELECT ID FROM Inventory");
    QSet<int> dbItemIds;

    while (fetchQuery.next()) {
        dbItemIds.insert(fetchQuery.value(0).toInt());
    }

    // **Step 3: Delete items from DB that are NOT in memory (items vector)**
    for (int dbId : dbItemIds) {
        if (!currentItemIds.contains(dbId)) {
            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM Inventory WHERE ID = :id");
            deleteQuery.bindValue(":id", dbId);

            if (!deleteQuery.exec()) {
                qDebug() << "Error: Failed to delete item!" << deleteQuery.lastError().text();
            } else {
                qDebug() << "Deleted item ID:" << dbId;
            }
        }
    }

    // **Step 4: Insert or update items (same as before)**
    for (const auto &item : items) {
        query.prepare("SELECT COUNT(*) FROM Inventory WHERE ID = :id");
        query.bindValue(":id", item.getId());
        query.exec();
        query.next();

        int count = query.value(0).toInt();

        if (count > 0) {
            query.prepare("UPDATE Inventory SET Name = :name, Quantity = :quantity, Price = :price, "
                          "Category = :category, Supplier = :supplier WHERE ID = :id");
        } else {
            query.prepare("INSERT INTO Inventory (ID, Name, Quantity, Price, Category, Supplier) "
                          "VALUES (:id, :name, :quantity, :price, :category, :supplier)");
        }

        query.bindValue(":id", item.getId());
        query.bindValue(":name", item.getName());
        query.bindValue(":quantity", item.getQuantity());
        query.bindValue(":price", item.getPrice());
        query.bindValue(":category", item.getCategory());
        query.bindValue(":supplier", item.getSupplier());

        if (!query.exec()) {
            qDebug() << "Error: Failed to save item!" << query.lastError().text();
        } else {
            qDebug() << "Saved/Updated item:" << item.getId() << item.getName();
        }
    }


    db.close();
    qDebug() << "Data saved successfully with additions, updates, and removals.";
}



QVector <Item>& InventoryManager::getItems()  {
    return items;

};


void InventoryManager::add_quantity(int id, int quantity) {
    for (uint32_t i = 0; i < items.size(); i++) {
        if (items[i].getId() == id) {
            items[i].setQuantity(items[i].getQuantity() + quantity);
            return;
        }
    }
}

// For DB + in-memory
void InventoryManager::add_new_item(Item new_item){
    // add item into In-memory vector
    items.push_back(new_item);

    DB_queries_saver.push_back({
        ChangeTypeForDB::Added ,
        new_item
    });



    return;
}

// For in-memory
void InventoryManager::remove_item_by_index(int index) {

    // remove item from in-memory
    items.remove(index);

}

// For DB
void InventoryManager::add_remove_item_to_db_query(Item item){
    
    DB_queries_saver.push_back({
        ChangeTypeForDB::Deleted,
        item
    });

};

// For in-memory
void InventoryManager::edit_item_by_index(int index , QString new_name ,
                                           QString new_category , QString new_supplier,
                                           int new_quantity , double new_price){

    items[index].setName(new_name);
    items[index].setCategory(new_category);
    items[index].setSupplier(new_supplier);
    items[index].setQuantity(new_quantity);
    items[index].setPrice(new_price);


}

// For DB
void InventoryManager::add_edit_item_to_db_query(Item updated_item){
    DB_queries_saver.push_back({
        ChangeTypeForDB::Modified,
        updated_item
    });

}

// completed for both Delete, Add and Modify
// need to edit  -> (when DB is structure is change and then sql querue need to change)
void  InventoryManager::update_DB_with_new_inventory_queries(){
        
            QSqlDatabase db = QSqlDatabase::database();

            for (const auto& change : DB_queries_saver)
            {
                switch (change.type)
                {
                    case ChangeTypeForDB::Deleted:  // DELETE
                    {
                        QSqlQuery query(db);

                        query.prepare("DELETE FROM Inventory WHERE ID = :id");
                        query.bindValue(":id", change.item.getId());

                        if (!query.exec())
                        {
                            qDebug() << "Error: Failed to delete item!"
                                    << query.lastError().text();
                        }
                        else
                        {
                            qDebug() << "Deleted item:"
                                    << change.item.getId()
                                    << change.item.getName();
                        }

                        break;
                    }

                    // Added and Modified will go here later

                    case ChangeTypeForDB::Added: // INSERT
                    {
                        QSqlQuery query(db);

                        query.prepare(
                            "INSERT INTO Inventory "
                            "(ID, Name, Quantity, Price, Category, Supplier) "
                            "VALUES (:id, :name, :quantity, :price, :category, :supplier)"
                        );

                        query.bindValue(":id", change.item.getId());
                        query.bindValue(":name", change.item.getName());
                        query.bindValue(":quantity", change.item.getQuantity());
                        query.bindValue(":price", change.item.getPrice());
                        query.bindValue(":category", change.item.getCategory());
                        query.bindValue(":supplier", change.item.getSupplier());

                        if (!query.exec())
                        {
                            qDebug() << "Error: Failed to add item!"
                                    << query.lastError().text();
                        }
                        else
                        {
                            qDebug() << "Added item:"
                                    << change.item.getId()
                                    << change.item.getName();
                        }

                        break;
                    }


                    case ChangeTypeForDB::Modified:
                    {
                        QSqlQuery query(db);

                        query.prepare(
                            "UPDATE Inventory SET "
                            "Name = :name, "
                            "Quantity = :quantity, "
                            "Price = :price, "
                            "Category = :category, "
                            "Supplier = :supplier "
                            "WHERE ID = :id"
                        );

                        query.bindValue(":id", change.item.getId());
                        query.bindValue(":name", change.item.getName());
                        query.bindValue(":quantity", change.item.getQuantity());
                        query.bindValue(":price", change.item.getPrice());
                        query.bindValue(":category", change.item.getCategory());
                        query.bindValue(":supplier", change.item.getSupplier());

                        if (!query.exec())
                        {
                            qDebug() << "Error: Failed to update item!"
                                    << query.lastError().text();
                        }
                        else
                        {
                            qDebug() << "Updated item:"
                                    << change.item.getId()
                                    << change.item.getName();
                        }

                        break;
                    }
                }

    
            }

            DB_queries_saver.clear();
    }


QVector <QString>& InventoryManager:: getInventoryLogs(){

    return m_inventory_logs;
}

void InventoryManager::add_item_to_inventory_log(QString log){
    m_inventory_logs.push_back(log);
}


void InventoryManager::clear_inventory_log(){
    m_inventory_logs.clear();
}

bool InventoryManager::is_inventory_log_empty(){
    return m_inventory_logs.empty() ? true:false ;
}





