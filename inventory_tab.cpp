// --- MainWindow.h function definition for Inventory Tab ---


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
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QtMath>


void MainWindow::on_addItemButton_clicked()
{

    // Create a dialog window
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Item");
    dialog.setModal(true); // Block interactions with the main window

    // Create layout
    QVBoxLayout layout(&dialog);

    // Create input fields
    QLineEdit idField, nameField, barcodeField;
    QDoubleSpinBox quantityField, priceField;
    QComboBox unitField;
    idField.setPlaceholderText("Enter ID");
    nameField.setPlaceholderText("Enter Name");
    quantityField.setRange(0, 1000000000);
    quantityField.setDecimals(3);
    quantityField.setSingleStep(0.001);
    priceField.setRange(0, 1000000000);
    priceField.setDecimals(2);
    unitField.addItem("Piece", static_cast<int>(Unit::Piece));
    unitField.addItem("Kilogram", static_cast<int>(Unit::Kilogram));
    unitField.addItem("Meter", static_cast<int>(Unit::Meter));
    barcodeField.setPlaceholderText("Enter Barcode");

    // Add fields to layout
    layout.addWidget(new QLabel("Item ID:"));
    layout.addWidget(&idField);
    layout.addWidget(new QLabel("Item Name:"));
    layout.addWidget(&nameField);
    layout.addWidget(new QLabel("Unit:"));
    layout.addWidget(&unitField);
    layout.addWidget(new QLabel("Quantity:"));
    layout.addWidget(&quantityField);
    layout.addWidget(new QLabel("Price:"));
    layout.addWidget(&priceField);
    layout.addWidget(new QLabel("Barcode:"));
    layout.addWidget(&barcodeField);

    connect(&unitField, qOverload<int>(&QComboBox::currentIndexChanged), [&](int) {
        const bool isPiece = unitField.currentData().toInt() == static_cast<int>(Unit::Piece);
        quantityField.setDecimals(isPiece ? 0 : 3);
        quantityField.setSingleStep(isPiece ? 1 : 0.001);
        if (isPiece) {
            quantityField.setValue(qFloor(quantityField.value()));
        }
    });

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
        double quantity = quantityField.value();
        double price = priceField.value();
        Unit unit = static_cast<Unit>(unitField.currentData().toInt());

        // Validation: Check for empty fields
        if (idText.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(&dialog, "Error", "All fields must be filled!");
            return;
        }

        // Convert values
        bool idOk;
        int id = idText.toInt(&idOk);

        // Validate number conversions
        if (!idOk || quantity < 0 || price < 0) {
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
        Item new_item(id, name, quantity, unit, price, barcodeField.text());
        manager.add_new_item(new_item);
        manager.add_new_item_to_db_query(new_item);


        //  Store item details for logging
        QString addItemLog = QString("Item ID: %1 | Name: %2 | Unit: %3 | Qty: %4 | Price: %5, has been added.\n")
                                     .arg(id)
                                     .arg(name)
                         .arg(unitField.currentText())
                         .arg(quantity)
                         .arg(price);

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
            QString removedItemLog = QString("Item ID: %1 | Name: %2 | Qty: %3 | Price: %4, has been removed.\n")
                                         .arg(items[i].getId())
                                         .arg(items[i].getName())
                                         .arg(items[i].getQuantity())
                                         .arg(items[i].getPrice());

                ;

            manager.add_item_to_inventory_log(removedItemLog);


            ui->logTextEdit->append(removedItemLog);

            manager.add_remove_item_to_db_query(items[i]);
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
    QComboBox unitEdit(&dialog);
    unitEdit.addItem("Piece", static_cast<int>(Unit::Piece));
    unitEdit.addItem("Kilogram", static_cast<int>(Unit::Kilogram));
    unitEdit.addItem("Meter", static_cast<int>(Unit::Meter));
    unitEdit.setCurrentIndex(unitEdit.findData(static_cast<int>(items[index].getUnit())));
    QDoubleSpinBox quantityEdit(&dialog);
    quantityEdit.setRange(0, 1000000000);
    quantityEdit.setDecimals(items[index].getUnit() == Unit::Piece ? 0 : 3);
    quantityEdit.setValue(items[index].getQuantity());
    QDoubleSpinBox priceEdit(&dialog);
    priceEdit.setRange(0, 1000000000);
    priceEdit.setDecimals(2);
    priceEdit.setValue(items[index].getPrice());
    QLineEdit barcodeEdit(items[index].getBarcode(), &dialog);

    form.addRow("Name:", &nameEdit);
    form.addRow("Unit:", &unitEdit);
    form.addRow("Quantity:", &quantityEdit);
    form.addRow("Price:", &priceEdit);
    form.addRow("Barcode:", &barcodeEdit);
    connect(&unitEdit, qOverload<int>(&QComboBox::currentIndexChanged), [&](int) {
        const bool isPiece = unitEdit.currentData().toInt() == static_cast<int>(Unit::Piece);
        quantityEdit.setDecimals(isPiece ? 0 : 3);
        if (isPiece) quantityEdit.setValue(qFloor(quantityEdit.value()));
    });

    // // Step 5: Add buttons
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form.addRow(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Step 6: Show the dialog
    if (dialog.exec() == QDialog::Accepted) {
        // Validate input
        if (nameEdit.text().isEmpty()) {
            QMessageBox::warning(this, "Error", "All fields must be filled!");
            return;
        }

        // Update item details



        QString beforeUpdate = QString("Item ID: %1 | Name: %2 | Qty: %3 | Price: %4, has been updated to: ")
                                     .arg(items[index].getId())
                                     .arg(items[index].getName())
                                     .arg(items[index].getQuantity())
                         .arg(items[index].getPrice() , 0, 'f', 2);
        //Log the edit action
        QString logMessage = QString(beforeUpdate + " Name: %2 | Unit: %3 | Qty: %4 | Price: %5 . \n")
                                 .arg(nameEdit.text())
                     .arg(unitEdit.currentText())
                     .arg(quantityEdit.value())
                     .arg(priceEdit.value(), 0, 'f', 2);
       
        Unit unit = static_cast<Unit>(unitEdit.currentData().toInt());
        manager.edit_item_by_index(index, nameEdit.text(), quantityEdit.value(), unit, priceEdit.value());
        
        Item updated_item = Item(
            itemId,
            nameEdit.text(),
            quantityEdit.value(),
            unit,
            priceEdit.value(),
            barcodeEdit.text()
            );
        manager.add_edit_item_to_db_query(updated_item);

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
    manager.update_DB_with_new_inventory_queries();
    manager.clear_inventory_log();
    ui->logTextEdit->clear();

}

