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

void MainWindow::filterByBarcode()
{
    QString searchText = ui->searchBarcodeLineEdit->text();

    for (int row = 0; row < ui->outputTableWidget->rowCount(); ++row) {
        QTableWidgetItem* item = ui->outputTableWidget->item(row, 5); // Barcode column
        if (item) {
            bool match = item->text().contains(searchText, Qt::CaseInsensitive);
            ui->outputTableWidget->setRowHidden(row, !match);
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
        double quantity = QInputDialog::getDouble(this, "Enter Quantity",
                              "Enter quantity for add " + itemName + ":",
                              selectedItem.getUnit() == Unit::Piece ? 1 : 0.001,
                              selectedItem.getUnit() == Unit::Piece ? 1 : 0.001,
                              selectedItem.getQuantity(),
                              selectedItem.getUnit() == Unit::Piece ? 0 : 3, &ok);

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
    QStringList Inventory_headers = {"ID", "Name", "Unit", "Quantity", "Price", "Barcode"};
    ui->outputTableWidget->setHorizontalHeaderLabels(Inventory_headers);

    // Inventory Table
    for (uint8_t row = 0; row < manager.getItems().size(); ++row) {
        const Item& item = manager.getItems().at(row);

        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(item.getId()));
        QTableWidgetItem* nameItem = new QTableWidgetItem(item.getName());
        QTableWidgetItem* unitItem = new QTableWidgetItem(item.getUnit() == Unit::Piece ? "Piece" : item.getUnit() == Unit::Kilogram ? "Kilogram" : "Meter");
        QTableWidgetItem* quantityItem = new QTableWidgetItem(QString::number(item.getQuantity()));
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(item.getPrice(), 'f', 2));
        QTableWidgetItem* barcodeItem = new QTableWidgetItem(item.getBarcode());

        //  Set items as read-only
        idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        quantityItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        priceItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        unitItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        barcodeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        //  Add items to table
        ui->outputTableWidget->setItem(row, 0, idItem);
        ui->outputTableWidget->setItem(row, 1, nameItem);
        ui->outputTableWidget->setItem(row, 2, unitItem);
        ui->outputTableWidget->setItem(row, 3, quantityItem);
        ui->outputTableWidget->setItem(row, 4, priceItem);
        ui->outputTableWidget->setItem(row, 5, barcodeItem);
    }

}


void MainWindow::on_RefreshButton_clicked()
{
    updateInventoryTable();
}



