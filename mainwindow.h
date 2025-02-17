#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "bill_manager.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // --- Function in inventory table (left side table)---
    //      fun. def. -> inventory_table.cpp

    void filterItems();
    void filterByID();
    void on_inventoryItemClicked(int row);
    void updateInventoryTable(); // updating visible inventory table according to Qvector (not from stored data)
    void on_RefreshButton_clicked();


    // --- Function for Inventory tab ---
    //  fun. def. -> inventory_tab.cpp

    void on_addItemButton_clicked();
    void on_removeItemButton_clicked();
    void on_editItemButton_clicked();
    void on_undoChangesButton_clicked();
    void on_saveChangesButton_clicked();

    // --- Function for billing tab ---
    //  fun. def. -> bill_tab.cpp

    void updateBillTable();  // updating visible bill table according to QVector (not from stored data)
    void on_BillItemClicked(int row); // what should happen when bill item is clicked
    void on_cleanBillButton_clicked();
    void on_getTheBillButton_clicked();
    void showCustomerDialog(); // dialog box for taking customer name
    void processBill(); // processing bill


    void save_bill_logs_to_database();
    void save_inventory_logs_to_database();


    // --- Function in menubar ---
    //  fun. def. -> mainwindow.cpp

    void setSaveLocation();
    void openSaveLocation();
    void changeBillName();

    void export_inventory_logs_to_csv();
    void export_bills_logs_to_csv();

    void Save_Bill_Logs();
    void Save_Inventory_Logs();

    void About_Application();

    // For Bill and Inventory tab Widget
    void on_tabWidget_currentChanged(int index);

private:

    Ui::MainWindow *ui;
    BillManager manager; // no name when constructing [name should get as input]


};
#endif // MAINWINDOW_H
