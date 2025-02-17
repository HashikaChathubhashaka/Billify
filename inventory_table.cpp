// ---MainWindow.h function definition for Inventory table ---


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "bill_manager.h"
#include <QInputDialog>  // For user input dialog
#include <QMessageBox>   // For error messages
#include <qfiledialog.h>
#include <cstdint>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QSettings>


void MainWindow::filterItems()
{
    QString searchText = ui->searchNameLineEdit->text().toLower(); // Get text and convert to lowercase
    for (int row = 0; row < ui->outputTableWidget->rowCount(); ++row) {
        QTableWidgetItem* item = ui->outputTableWidget->item(row, 1); // Get Name column (index 1)
        if (item) {
            QString itemName = item->text().toLower(); // Convert name to lowercase for case-insensitive search
            bool match = itemName.contains(searchText); // Shows items even if the search text appears in the middle
            ui->outputTableWidget->setRowHidden(row, !match); // Hide rows that don’t match

        }
    }
}


void MainWindow::filterByID()
{
    QString searchText = ui->searchIDLineEdit->text(); // Get the entered ID text

    for (int row = 0; row < ui->outputTableWidget->rowCount(); ++row) {
        QTableWidgetItem* item = ui->outputTableWidget->item(row, 0); // Get ID column (index 0)
        if (item) {
            QString itemID = item->text(); // Get the text of the ID column
            bool match = itemID.contains(searchText, Qt::CaseInsensitive); // Case-insensitive search
            ui->outputTableWidget->setRowHidden(row, !match); // Hide rows that don’t match
        }
    }
}


void MainWindow::on_inventoryItemClicked(int row) {
    if (ui->tabWidget->currentIndex() == 0){

        qDebug()<<"intentory Item clicked";

        if (row < 0 || row >= manager.getItems().size()) return; // Safety check

        const Item& selectedItem = manager.getItems().at(row);  // Get selected item
        int itemId = selectedItem.getId();
        QString itemName = selectedItem.getName();

        // Ask for quantity input
        bool ok;
        int quantity = QInputDialog::getInt(this, "Enter Quantity",
                                            "Enter quantity for add " + itemName + ":",
                                            1, 1, selectedItem.getQuantity(), 1, &ok);

        if (ok) { // If user pressed OK
            manager.Add_to_Bill(itemId, quantity); // Add item to bill item vector (add item to bill)
            updateBillTable(); // Update the bill table

            double total = manager.get_bill_total();  // Assuming you have a function for total price
            ui->billTotalLabel->setText("Total: " + QString::number(total, 'f', 2)); // Display with 2 decimal places

        }

    }
}


void MainWindow::updateInventoryTable(){

    qDebug() << "Main inventory updated";


    ui->outputTableWidget->clear(); // Clear previous data
    ui->outputTableWidget->setRowCount(manager.getItems().size()); // Set rows dynamically
    ui->outputTableWidget->setColumnCount(6); // Have 6 columns

    // Set table headers
    QStringList Inventory_headers = {"ID", "Name", "Quantity", "Price", "Category", "Supplier"};
    ui->outputTableWidget->setHorizontalHeaderLabels(Inventory_headers);

    // Inventory Table
    for (uint8_t row = 0; row < manager.getItems().size(); ++row) {
        const Item& item = manager.getItems().at(row);

        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(item.getId()));
        QTableWidgetItem* nameItem = new QTableWidgetItem(item.getName());
        QTableWidgetItem* quantityItem = new QTableWidgetItem(QString::number(item.getQuantity()));
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(item.getPrice(), 'f', 2)); // 2 decimal places
        QTableWidgetItem* categoryItem = new QTableWidgetItem(item.getCategory());
        QTableWidgetItem* supplierItem = new QTableWidgetItem(item.getSupplier());

        //  Set items as read-only
        idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        quantityItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        priceItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        categoryItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        supplierItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        //  Add items to table
        ui->outputTableWidget->setItem(row, 0, idItem);
        ui->outputTableWidget->setItem(row, 1, nameItem);
        ui->outputTableWidget->setItem(row, 2, quantityItem);
        ui->outputTableWidget->setItem(row, 3, priceItem);
        ui->outputTableWidget->setItem(row, 4, categoryItem);
        ui->outputTableWidget->setItem(row, 5, supplierItem);
    }

}


void MainWindow::on_RefreshButton_clicked()
{
    updateInventoryTable();
}



