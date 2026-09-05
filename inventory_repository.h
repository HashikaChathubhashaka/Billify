#ifndef INVENTORY_REPOSITORY_H
#define INVENTORY_REPOSITORY_H

#include "item.h"
#include <QVector>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class InventoryRepository
{
public:
    InventoryRepository();


    // Function for changing DB data.
    bool add(const Item& item);
    bool update(const Item& item);
    bool remove(const Item& item);

    // Load data from DB
    void load_data();
private:
    QSqlDatabase db;

};

#endif // INVENTORY_REPOSITORY_H
