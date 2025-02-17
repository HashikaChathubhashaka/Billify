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
#include <QFormLayout>


void MainWindow::on_addItemButton_clicked()
{


    // Create a dialog window
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Item");
    dialog.setModal(true); // Block interactions with the main window

    // Create layout
    QVBoxLayout layout(&dialog);

    // Create input fields
    QLineEdit idField, nameField, quantityField, priceField, categoryField, supplierField;
    idField.setPlaceholderText("Enter ID");
    nameField.setPlaceholderText("Enter Name");
    quantityField.setPlaceholderText("Enter Quantity");
    priceField.setPlaceholderText("Enter Price");
    categoryField.setPlaceholderText("Enter Category");
    supplierField.setPlaceholderText("Enter Supplier");

    // Add fields to layout
    layout.addWidget(new QLabel("Item ID:"));
    layout.addWidget(&idField);
    layout.addWidget(new QLabel("Item Name:"));
    layout.addWidget(&nameField);
    layout.addWidget(new QLabel("Quantity:"));
    layout.addWidget(&quantityField);
    layout.addWidget(new QLabel("Price:"));
    layout.addWidget(&priceField);
    layout.addWidget(new QLabel("Category:"));
    layout.addWidget(&categoryField);
    layout.addWidget(new QLabel("Supplier:"));
    layout.addWidget(&supplierField);

    // Add buttons
    QPushButton addButton("Add Item"), cancelButton("Cancel");
    layout.addWidget(&addButton);
    layout.addWidget(&cancelButton);

    // Close dialog on cancel
    connect(&cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // Connect Add button
    connect(&addButton, &QPushButton::clicked, [&]() {
        // Get input values
        QString idText = idField.text();
        QString name = nameField.text();
        QString quantityText = quantityField.text();
        QString priceText = priceField.text();
        QString category = categoryField.text();
        QString supplier = supplierField.text();

        // Validation: Check for empty fields
        if (idText.isEmpty() || name.isEmpty() || quantityText.isEmpty() ||
            priceText.isEmpty() || category.isEmpty() || supplier.isEmpty()) {
            QMessageBox::warning(&dialog, "Error", "All fields must be filled!");
            return;
        }

        // Convert values
        bool idOk, quantityOk, priceOk;
        int id = idText.toInt(&idOk);
        int quantity = quantityText.toInt(&quantityOk);
        double price = priceText.toDouble(&priceOk);

        // Validate number conversions
        if (!idOk || !quantityOk || !priceOk) {
            QMessageBox::warning(&dialog, "Error", "Invalid numeric values!");
            return;
        }

        // Check for duplicate ID
        for (const Item &item : manager.getItems()) {
            if (item.getId() == id) {
                QMessageBox::warning(&dialog, "Error", "Item ID already exists!");
                return;
            }
        }

        // Add new item
        manager.add_new_item(Item(id, name, quantity, price, category, supplier));


        //  Store item details for logging
        QString addItemLog = QString("Item ID: %1 | Name: %2 | Qty: %3 | Price: %4 | Cat: %5 | Supplier: %6 , has been added.\n")
                                     .arg(id)
                                     .arg(name)
                                     .arg(quantity)
                                     .arg(price)
                                     .arg(category)
                                     .arg(supplier);

        QMessageBox::information(&dialog, "Success", "Item added successfully!");
        manager.add_item_to_inventory_log(addItemLog);

        ui->logTextEdit->append(addItemLog);

        dialog.accept(); // Close the dialog after successful addition
    });

    // Show the dialog
    dialog.exec();


    // Refresh table after adding new item
    // updateInventoryTable();

}


void MainWindow::on_removeItemButton_clicked()
{
    // Step 1: Ask for the item ID using a dialog
    bool ok;
    int itemId = QInputDialog::getInt(this, "Remove Item", "Enter Item ID to remove:", 0, 0, 1000000, 1, &ok);

    // Step 2: If the user pressed Cancel, exit the function
    if (!ok) {
        return;
    }


    const auto& items = manager.getItems();
    for (uint32_t i = 0; i < items.size(); i++) {
        if (items[i].getId() == itemId) {


            //  Store item details for logging
            QString removedItemLog = QString("Item ID: %1 | Name: %2 | Qty: %3 | Price: %4 | Cat: %5 | Supplier: %6 , has been removed.\n")
                                         .arg(items[i].getId())
                                         .arg(items[i].getName())
                                         .arg(items[i].getQuantity())
                                         .arg(items[i].getPrice())
                                         .arg(items[i].getCategory())
                                         .arg(items[i].getSupplier());

                ;

        manager.add_item_to_inventory_log(removedItemLog);


            ui->logTextEdit->append(removedItemLog);
            manager.remove_item_by_index(i);
            QMessageBox::information(this, "Success", "Item removed successfully!");
            return;
        }
    }


    QMessageBox::warning(this, "Error", "Item ID not found in inventory!");
    return;
}


void MainWindow::on_editItemButton_clicked()
{

    // Step 1: Ask for the item ID
    bool ok;
    int itemId = QInputDialog::getInt(this, "Edit Item", "Enter Item ID to edit:", 0, 0, 1000000, 1, &ok);
    if (!ok) return; // User canceled

    // Step 2: Search for the item using a for loop
    const auto& items = manager.getItems();
    int index = -1;

    for (int i = 0; i < items.size(); i++) {
        if (items[i].getId() == itemId) {
            index = i;
            break;
        }
    }

    // Step 3: If the item was not found, show an error message
    if (index == -1) {
        QMessageBox::warning(this, "Error", "Item ID not found in inventory!");
        return;
    }

    // // Step 4: Create a dialog with input fields for all details
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Item Details");

    QFormLayout form(&dialog);

    QLineEdit nameEdit(items[index].getName(), &dialog);
    QLineEdit categoryEdit(items[index].getCategory(), &dialog);
    QLineEdit supplierEdit(items[index].getSupplier(), &dialog);
    QLineEdit quantityEdit(QString::number(items[index].getQuantity()), &dialog);
    QLineEdit priceEdit(QString::number(items[index].getPrice(), 'f', 2), &dialog);

    form.addRow("Name:", &nameEdit);
    form.addRow("Category:", &categoryEdit);
    form.addRow("Supplier:", &supplierEdit);
    form.addRow("Quantity:", &quantityEdit);
    form.addRow("Price:", &priceEdit);

    // // Step 5: Add buttons
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form.addRow(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Step 6: Show the dialog
    if (dialog.exec() == QDialog::Accepted) {
        // Validate input
        if (nameEdit.text().isEmpty() || categoryEdit.text().isEmpty() || supplierEdit.text().isEmpty() ||
            quantityEdit.text().isEmpty() || priceEdit.text().isEmpty()) {
            QMessageBox::warning(this, "Error", "All fields must be filled!");
            return;
        }

        // Update item details



        QString beforeUpdate = QString("Item ID: %1 | Name: %2 | Qty: %3 | Price: %4 | Cat: %5 | Supplier: %6 , has been updated to: ")
                                     .arg(items[index].getId())
                                     .arg(items[index].getName())
                                     .arg(items[index].getQuantity())
                                     .arg(items[index].getPrice() , 0, 'f', 2)
                                     .arg(items[index].getCategory())
                                     .arg(items[index].getSupplier());
        //Log the edit action
        QString logMessage = QString(beforeUpdate + " Name: %2 | Qty: %3 | Price: %4 | Cat: %5 | Supplier: %6 . \n")
                                 .arg(nameEdit.text())
                                 .arg(quantityEdit.text().toInt())
                                 .arg(priceEdit.text().toDouble(), 0, 'f', 2)
                                 .arg(categoryEdit.text())
                                 .arg(supplierEdit.text());

        manager.edit_item_by_index(index,nameEdit.text(),categoryEdit.text() ,supplierEdit.text() ,
                                   quantityEdit.text().toInt() ,priceEdit.text().toDouble());

        ui->logTextEdit->append(logMessage);
        manager.add_item_to_inventory_log(logMessage);

        QMessageBox::information(this, "Success", "Item updated successfully!");
    }

}


void MainWindow::on_undoChangesButton_clicked()
{
    manager.clear_inventory_log();
    manager.load_data();
    ui->logTextEdit->clear();

}

void MainWindow::on_saveChangesButton_clicked()
{
    if (!manager.is_inventory_log_empty()){
        save_inventory_logs_to_database(); // update inventory_logs table

        if (ui->InventoryCheckBox->isChecked()){ // is checkbox is ticked
            manager.generateLogsPDF();
        }

    }
    updateInventoryTable();
    manager.save_data(); // update inventory table in database [inventory Vector -> update the database table]
    manager.clear_inventory_log();
    ui->logTextEdit->clear();

}

