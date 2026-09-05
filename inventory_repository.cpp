#include "inventory_repository.h"

InventoryRepository::InventoryRepository() {
    db = QSqlDatabase::database();
};

bool InventoryRepository::add(const Item& item)
{

    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO Inventory "
        "(ID, Name, Quantity, Price, UnitType, Barcode) "
        "VALUES (:id, :name, :quantity, :price, :unitType, :barcode)"
        );

    query.bindValue(":id", item.getId());
    query.bindValue(":name", item.getName());
    query.bindValue(":quantity",item.getQuantity());
    query.bindValue(":price", item.getPrice());
    query.bindValue(":unitType", static_cast<int>(item.getUnit()));
    query.bindValue(":barcode", item.getBarcode());

    if (!query.exec())
    {
        qDebug() << "Error: Failed to add item!"
                 << query.lastError().text();
        return false;
    }
    else
    {
        qDebug() << "Added item:"
                 << item.getId()
                 << item.getName();
        return true;
    }


};


bool InventoryRepository::update(const Item& item){

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

    query.bindValue(":id", item.getId());
    query.bindValue(":name", item.getName());
    query.bindValue(":quantity", item.getQuantity());
    query.bindValue(":price", item.getPrice());
    query.bindValue(":unitType", static_cast<int>(item.getUnit()));
    query.bindValue(":barcode", item.getBarcode());

    if (!query.exec())
    {
        qDebug() << "Error: Failed to update item!"
                 << query.lastError().text();
        return false;
    }
    else
    {
        qDebug() << "Updated item:"
                 << item.getId()
                 << item.getName();
        return true;
    }


};


bool InventoryRepository::remove(const Item& item){

    QSqlQuery query(db);

    query.prepare("DELETE FROM Inventory WHERE ID = :id");
    query.bindValue(":id", item.getId());

    if (!query.exec())
    {
        qDebug() << "Error: Failed to delete item!"
                 << query.lastError().text();
        return false;
    }
    else
    {
        qDebug() << "Deleted item:"
                 << item.getId()
                 << item.getName();
        return true;
    }



};


