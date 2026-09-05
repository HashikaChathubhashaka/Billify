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

    QSqlQuery schemaQuery(db);
    schemaQuery.exec("ALTER TABLE Inventory ADD COLUMN UnitType INTEGER NOT NULL DEFAULT 0");
    schemaQuery.exec("ALTER TABLE Inventory ADD COLUMN Barcode TEXT NOT NULL DEFAULT ''");

    QSqlQuery query(db);
    query.prepare("SELECT ID, Name, Quantity, Price, UnitType, Barcode FROM Inventory");
    if (!query.exec()) {
        qDebug() << "Error: Failed to load inventory:" << query.lastError().text();
        return;
    }

    items.clear(); // Clear existing data before loading

    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        double quantity = query.value(2).toDouble();
        double price = query.value(3).toDouble();
        Unit unit = static_cast<Unit>(query.value(4).toInt());
        QString barcode = query.value(5).toString();

        Item item(id, name, quantity, unit, price, barcode);
        items.push_back(item);

        qDebug() << "Loaded item:" << id << name << quantity << price << query.value(4).toInt();
    }

    db.close();
    qDebug() << "Data ing from SQLite completed successfully.";
}


QVector <Item>& InventoryManager::getItems()  {
    return items;

};


void InventoryManager::add_quantity(int id, double quantity) {
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

    DB_updates_saver.push_back({
        ChangeTypeForDB::Added ,
        new_item
    });



    return;
}

void InventoryManager::add_new_item_to_db_query(Item item){

    DB_updates_saver.push_back({
        ChangeTypeForDB::Added ,
        item
    });
};

// For in-memory
void InventoryManager::remove_item_by_index(int index) {

    // remove item from in-memory
    items.remove(index);

}

// For DB
void InventoryManager::add_remove_item_to_db_query(Item item){
    
    DB_updates_saver.push_back({
        ChangeTypeForDB::Deleted,
        item
    });

};

// For in-memory
void InventoryManager::edit_item_by_index(int index, QString new_name,
                                           double new_quantity, Unit new_unit,
                                           double new_price){

    items[index].setName(new_name);
    items[index].setQuantity(new_quantity);
    items[index].setUnit(new_unit);
    items[index].setPrice(new_price);


}

// For DB
void InventoryManager::add_edit_item_to_db_query(Item updated_item){
    DB_updates_saver.push_back({
        ChangeTypeForDB::Modified,
        updated_item
    });

}

// completed for both Delete, Add and Modify
// need to edit  -> (when DB is structure is change and then sql querue need to change)
void  InventoryManager::update_DB_with_new_inventory_queries(){
        
            QSqlDatabase db = QSqlDatabase::database();

            for (const auto& change : DB_updates_saver)
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
                            "(ID, Name, Quantity, Price, UnitType, Barcode) "
                            "VALUES (:id, :name, :quantity, :price, :unitType, :barcode)"
                        );

                        query.bindValue(":id", change.item.getId());
                        query.bindValue(":name", change.item.getName());
                        query.bindValue(":quantity", change.item.getQuantity());
                        query.bindValue(":price", change.item.getPrice());
                        query.bindValue(":unitType", static_cast<int>(change.item.getUnit()));
                        query.bindValue(":barcode", change.item.getBarcode());

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
                            "UnitType = :unitType, "    
                            "Barcode = :barcode "
                            "WHERE ID = :id"
                        );

                        query.bindValue(":id", change.item.getId());
                        query.bindValue(":name", change.item.getName());
                        query.bindValue(":quantity", change.item.getQuantity());
                        query.bindValue(":price", change.item.getPrice());
                        query.bindValue(":unitType", static_cast<int>(change.item.getUnit()));
                        query.bindValue(":barcode", change.item.getBarcode());

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

            DB_updates_saver.clear();
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





