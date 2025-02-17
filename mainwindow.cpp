// ---MainWindow.h menubar Functions and other function defintions ---


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
#include <QSqlQuery>
#include <QSqlError>
#include <QTabWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Serching From inventory
    // By name
    connect(ui->searchNameLineEdit, &QLineEdit::textChanged, this, &MainWindow::filterItems);
    // By ID
    connect(ui->searchIDLineEdit, &QLineEdit::textChanged,this , &MainWindow::filterByID);

    //select from Inventory list - double clicked inventory item
    connect(ui->outputTableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::on_inventoryItemClicked);

    //select from Bill list - double clicked bill item
    connect(ui->billOutputTableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::on_BillItemClicked);

    //Action for menu buttons
    connect(ui->actionChange_Save_location, &QAction::triggered, this, &MainWindow::setSaveLocation);

    connect(ui->actionSave_Location, &QAction::triggered, this, &MainWindow::openSaveLocation);

    connect(ui->actionChange_Shop_Name, &QAction::triggered, this, &MainWindow::changeBillName);

    connect(ui->actionSave_Bills_Logs, &QAction::triggered, this, &MainWindow::Save_Bill_Logs);

    connect(ui->actionSave_Inventory_Logs, &QAction::triggered, this, &MainWindow::Save_Inventory_Logs);

    connect(ui->actionAbout_Application, &QAction::triggered, this, &MainWindow::About_Application);



    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::on_tabWidget_currentChanged);

    // Bill table initilize
    ui->billOutputTableWidget->setColumnCount(5);
    QStringList Bill_headers = {"ID", "Name", "Quantity","Price per unit", "Total Price"};
    ui->billOutputTableWidget->setHorizontalHeaderLabels(Bill_headers);



    // inventory Table initilize
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

    // Ensure proper display settings
    ui->outputTableWidget->setShowGrid(true);
    ui->outputTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Resize columns to fit content
    ui->outputTableWidget->resizeColumnsToContents();


    // display setting for Billing Side
    ui->billOutputTableWidget->setShowGrid(true);
    ui->billOutputTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    ui->billOutputTableWidget->resizeColumnsToContents();

    // --- Loading from saved data from QSettings ---
    // Load saved directory
    QSettings settings("HashikaCompany", "Billify");
    manager.setSaveDirectory( settings.value("saveDirectory", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString() );
    QString savedBillName = settings.value("billName", "").toString();

    // Set the bill name if it was saved previously
    if (!savedBillName.isEmpty()) {
        manager.setShopName(savedBillName);
    }

}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::openSaveLocation() {
    QString saveDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (! manager.getSaveDirectory().isEmpty()) {
        saveDirectory = manager.getSaveDirectory();
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(saveDirectory));
}

void MainWindow::setSaveLocation() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Save Location",
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    if (!dir.isEmpty()) {
        manager.setSaveDirectory(dir);  // Store the selected directory
        qDebug() << "Save location set to: " << dir;
    }

    // Save to persistent settings
    QSettings settings("HashikaCompany", "Billify");
    settings.setValue("saveDirectory", dir);
}

void MainWindow::changeBillName() {
    // Step 1: Ask for the new bill name using a dialog
    bool ok;
    QString newBillName = QInputDialog::getText(this, "Change Bill Name",
                                                "Enter new bill name:", QLineEdit::Normal,
                                                manager.getShopName(), &ok);

    // Step 2: Check if the user pressed OK and entered a valid name
    if (ok && !newBillName.isEmpty()) {
        // Save the new bill name using setBillname function
        manager.setShopName(newBillName);

        // Step 3: Store the new bill name in QSettings for persistence
        QSettings settings("HashikaCompany", "Billify");
        settings.setValue("billName", newBillName);

        QMessageBox::information(this, "Success", "Bill name changed successfully to: " + newBillName);
    } else {
        QMessageBox::warning(this, "Error", "Bill name change was canceled or empty!");
    }
}

void MainWindow::save_bill_logs_to_database()
{

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
    query.prepare("INSERT INTO BillsLogs (Date_Time, Customer_name, Total) "
                  "VALUES (datetime('now', 'localtime'), :customer_name, :total)");


    query.bindValue(":customer_name", manager.getCustomerName());
    query.bindValue(":total", manager.get_bill_total());

    if (!query.exec()) {
        qDebug() << "Error inserting into BillsLogs:" << query.lastError().text();
    } else {
        qDebug() << "Bill log saved successfully!";
    }

    db.close();
}

void MainWindow::save_inventory_logs_to_database() {

    QSqlDatabase db = QSqlDatabase::database();  // Use existing connection if available

    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE");  // Only add if no existing connection
        db.setDatabaseName("database.db");
    }

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Error: Cannot open database!" << db.lastError().text();
        return;
    }

    // Prepare the SQL query to insert logs
    QSqlQuery query;
    query.prepare("INSERT INTO InventoryLogs (Date_Time, Log) "
                  "VALUES (datetime('now', 'localtime'), :log)");

    // Get the inventory logs
    QVector<QString>& logs = manager.getInventoryLogs();  // Assuming this is how you get the logs

    // Iterate through each log and insert it into the database
    for (const QString& log : logs) {
        query.bindValue(":log", log);

        // Execute the query
        if (!query.exec()) {
            qDebug() << "Error inserting log into InventoryLogs:" << query.lastError().text();
        } else {
            qDebug() << "Log saved successfully!";
        }
    }

    // Close the database connection
    db.close();
}

void MainWindow::export_inventory_logs_to_csv() {

    QSqlDatabase db = QSqlDatabase::database();  // Use existing connection if available

    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE");  // Only add if no existing connection
        db.setDatabaseName("database.db");
    }

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Error: Cannot open database!" << db.lastError().text();
        return;
    }

    // Create the file where the CSV will be saved
    QString DateTimeForSaving = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss_");

    QString filePath = manager.getSaveDirectory() + "/latest_inventory_logs_on" + DateTimeForSaving + ".csv";

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error opening file for writing:" << file.errorString();
        return;
    }

    qDebug()<<" inventory logs saved at " <<filePath;
    QTextStream out(&file);

    // Query to get all records from the InventoryLogs table
    QSqlQuery query("SELECT * FROM InventoryLogs");

    // Write headers to the CSV file
    out << "Date_Time,Log\n";

    // Iterate through the results and write them to the CSV file
    while (query.next()) {
        QString dateTime = query.value("Date_Time").toString();
        QString log = query.value("Log").toString();
        out << dateTime << "," << log << "\n";
    }

    // Close the file and database
    file.close();
    db.close();

    qDebug() << "CSV file saved successfully!";
}

void MainWindow::export_bills_logs_to_csv() {

    QSqlDatabase db = QSqlDatabase::database();  // Use existing connection if available

    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE");  // Only add if no existing connection
        db.setDatabaseName("database.db");
    }

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Error: Cannot open database!" << db.lastError().text();
        return;
    }

    // Create the file where the CSV will be saved
    QString DateTimeForSaving = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss_");

    QString filePath = manager.getSaveDirectory() + "/latest_bill_logs_on" + DateTimeForSaving + ".csv";


    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error opening file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);

    // Query to get all records from the InventoryLogs table
    QSqlQuery query("SELECT * FROM BillsLogs");

    // Write headers to the CSV file
    out << "Date_Time,Customer_name,Total\n";

    // Iterate through the results and write them to the CSV file
    while (query.next()) {
        QString dateTime = query.value("Date_Time").toString();
        QString customerName = query.value("Customer_name").toString();
        QString Total = query.value("Total").toString();
        out << dateTime << "," << customerName << "," <<  Total << "\n";
    }

    // Close the file and database
    file.close();
    db.close();

    qDebug() << "CSV file saved successfully!";
}

void MainWindow::Save_Bill_Logs() {
    // Ask the user if they want to save the logs as a CSV file
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Save Logs", "Do you want to save the latest Bills logs as a CSV file?",
                                  QMessageBox::Yes | QMessageBox::No);

    // If the user clicks Yes, save the logs to CSV
    if (reply == QMessageBox::Yes) {
        // Call the function to export bills logs to CSV
        export_bills_logs_to_csv();

        // Show a success message box
        QMessageBox::information(this, "Success", "Bill logs have been saved successfully!", QMessageBox::Ok);
    }
}

void MainWindow::Save_Inventory_Logs(){

    // Ask the user if they want to save the logs as a CSV file
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Save Logs", "Do you want to save the latest Inventory logs as a CSV file?",
                                  QMessageBox::Yes | QMessageBox::No);

    // If the user clicks Yes, save the logs to CSV
    if (reply == QMessageBox::Yes) {
        // Call the function to export bills logs to CSV
        export_inventory_logs_to_csv();
        QMessageBox::information(this, "Success", "Inventory logs have been saved successfully!", QMessageBox::Ok);

    }

}

void MainWindow::on_tabWidget_currentChanged(int index) {
    static bool preventLoop = false;

    if (preventLoop) {
        preventLoop = false;
        return;
    }

    if (index == 1) {  // User trying to switch to tab 1
        if (!(manager.Get_Bill_list().size() == 0)) {
            ui->tabWidget->setCurrentIndex(0);  // Force user to stay on tab 0
            QMessageBox::warning(this, "Access Denied", "Get or Clear the Bill before going to the Inventory Tab.");
            preventLoop = true;

        }
    } else if (index == 0) {  // User trying to switch back to tab 0
        if (!(manager.is_inventory_log_empty())) {
            ui->tabWidget->setCurrentIndex(1);  // Force user to stay on tab 1
            QMessageBox::warning(this, "Access Denied", "Save or Undo Inventory before going to the Bill Tab.");
            preventLoop = true;

        }
    }
}

void MainWindow::About_Application() {
    QMessageBox::about(this, "About Billify",
                       "<h1><b>Billify</b></h1>"
                       "<h2>Version 1.0.0</h2>"
                       "<p>Visit for source code and new patches: <br>"
                       "<a href='https://github.com/HashikaChathubhashaka/Billify'>GitHub Repository</a></p>"
                       "<p><b>Developed by:</b> Hashika Chathubhashaka</p>"
                       "<p>Built with: Qt Creator | C++</p>"
                       "<p> 2025 </p>"
                       );
}

