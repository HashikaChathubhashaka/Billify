// ---- MainWindow.h function definition for Bill Tab ----

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


void MainWindow::updateBillTable()
{

    QVector<BillItem> billItems = manager.Get_Bill_list();
    ui->billOutputTableWidget->setRowCount(billItems.size());

    for (int i = 0; i < billItems.size(); ++i) {

        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(billItems[i].getID()));
        QTableWidgetItem *nameItem = new QTableWidgetItem(billItems[i].getName());
        QTableWidgetItem *quantityItem = new QTableWidgetItem(QString::number(billItems[i].getQuantity()));
        QTableWidgetItem *pricePerUnitItem = new QTableWidgetItem(QString::number(billItems[i].getPricePerUnit()));
        QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(billItems[i].getTotalPrice()));

        // Set items as read-only
        idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        quantityItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        pricePerUnitItem -> setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        priceItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        // Add items to table
        ui->billOutputTableWidget->setItem(i, 0, idItem);
        ui->billOutputTableWidget->setItem(i, 1, nameItem);
        ui->billOutputTableWidget->setItem(i, 2, quantityItem);
        ui->billOutputTableWidget->setItem(i,3,pricePerUnitItem);
        ui->billOutputTableWidget->setItem(i, 4, priceItem);

    }

    // update the quantity of inventory table according to that
    for (uint8_t row = 0; row < manager.getItems().size(); ++row) {
        const Item& item = manager.getItems()[row];
        QTableWidgetItem* quantityItem = new QTableWidgetItem(QString::number(item.getQuantity()));
        quantityItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->outputTableWidget->setItem(row, 2, quantityItem);
    }


    ui->searchNameLineEdit->clear();

}

void MainWindow::on_BillItemClicked(int row) {

    qDebug()<<"Bill Item clicked";

    if (row < 0 || row >= manager.Get_Bill_list().size()) return; // Safety check

    const BillItem& selectedItem = manager.Get_Bill_list().at(row);  // Get selected item
    int itemId = selectedItem.getID();
    QString itemName = selectedItem.getName();

    // Ask for quantity input
    bool ok;
    int quantity = QInputDialog::getInt(this, "Enter Quantity",
                                        "Enter quantity for remove " + itemName + ":",
                                        1, 1, selectedItem.getQuantity(), 1, &ok);

    if (ok) { // If user pressed OK
        manager.Remove_from_Bill(itemId, quantity); // Add item to bill
        updateBillTable(); // Update the bill table

        double total = manager.get_bill_total();  // function for total price
        ui->billTotalLabel->setText("Total: " + QString::number(total, 'f', 2)); // Display with 2 decimal places
    }
}

void MainWindow::on_cleanBillButton_clicked()
{


    if (manager.Get_Bill_list().size() != 0){
        manager.emptyingBill();
        updateBillTable();
        ui->billTotalLabel->setText("Total: 0.00"); // Reset the label
        return;
    }

    else{
        return;

    }

}

void MainWindow::on_getTheBillButton_clicked()
{
    if (manager.Get_Bill_list().size() != 0) {
        showCustomerDialog(); // Open the dialog for customer name
    }
}

void MainWindow::showCustomerDialog()
{
    // Create the dialog
    QDialog dialog(this);
    dialog.setWindowTitle("Conform Bill");
    dialog.setModal(true);  // Blocks main window until closed

    // Create a layout
    QVBoxLayout layout(&dialog);

    // Label + Input Field
    QLabel label("Enter Customer Name:");
    QLineEdit nameInput("Customer");
    layout.addWidget(&label);
    layout.addWidget(&nameInput);

    // Buttons: Process Bill & Cancel
    QHBoxLayout buttonLayout;
    QPushButton processButton("Process Bill");
    QPushButton cancelButton("Go Back");
    buttonLayout.addWidget(&processButton);
    buttonLayout.addWidget(&cancelButton);
    layout.addLayout(&buttonLayout);

    // Connect buttons to actions
    connect(&processButton, &QPushButton::clicked, [&]() {
        manager.setCustomerName(nameInput.text());  // Store customer name
        save_bill_logs_to_database();
        processBill();  // Process the bill

        dialog.accept();  // Close the dialog
    });

    connect(&cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // Show the dialog
    dialog.exec();
}

void MainWindow::processBill()
{
    qDebug() << "Processing bill for: " << manager.getCustomerName();

    if (ui->BillsCheckBox->isChecked()){ // is checkbox is ticked

        // Generate PDF
        if (!manager.generateBillPDF()) { // calling for PDF generating function
            qDebug() << "Error: Failed to generate PDF!";
        }

    }

    // Save inventory data
    manager.save_data();

    // Reset the manager object
    manager = BillManager();  // Recreate manager (clear bill)
    QSettings settings("HashikaCompany", "Billify");
    manager.setSaveDirectory( settings.value("saveDirectory", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString() );

    // Reset UI
    ui->billTotalLabel->setText("Total: 0.00");
    updateBillTable();
    ui->billOutputTableWidget->clearContents();
}
