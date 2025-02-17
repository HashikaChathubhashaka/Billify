// --- Function definition for bill_manager.h ---

#include "Bill_manager.h"
#include <QVector>

#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QStandardPaths>
#include <QPdfWriter>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>



BillManager::BillManager() {
        qDebug() << "Bill manager constructor";
}

// Add item to bill and update inventory
void BillManager::Add_to_Bill(int id, int Quantity) {
    for (int i = 0; i < items.size(); i++) {
        if (items[i].getId() == id) {




            if (items[i].getQuantity() >= Quantity) {


                // search for update
                for( int x=0; x < m_bill_items.size();x++){
                    if ( m_bill_items[x].getID() == id ){


                        // have to update elements

                        items[i].setQuantity(items[i].getQuantity() - Quantity); // remove from inventory
                        m_bill_items[x].setQuantity(m_bill_items[x].getQuantity()+ Quantity); // update in Bill

                        //update the total price wit adding new Quantity
                        total_price += m_bill_items[x].getPricePerUnit() * Quantity;

                        return;
                    }
                }



                items[i].setQuantity(items[i].getQuantity() - Quantity);

                BillItem item(id, items[i].getName(), Quantity, items[i].getPrice());

                m_bill_items.append(item);

                total_price += item.getTotalPrice();
                qDebug() << "Added to bill: " << item.getName() << " x" << Quantity;
                qDebug() << "Stock Quantity: " << items[i].getQuantity();
                return;


            } else {
                qDebug() << "Not enough quantity available";
                return;
            }
        }
    }
    qDebug() << "Item not found";
}

// Remove item from bill and update inventory
void BillManager::Remove_from_Bill(int id, int Quantity) {
    for (int i = 0; i < m_bill_items.size(); i++) {
        if (m_bill_items[i].getID() == id) {
            if (m_bill_items[i].getQuantity() > Quantity) {

                m_bill_items[i].setQuantity(m_bill_items[i].getQuantity() - Quantity);


                total_price -= m_bill_items[i].getPricePerUnit() * Quantity;


                add_quantity(id, Quantity); // Restore removed items to inventory [only update the quantity]

                qDebug() << "Removed from bill: " << m_bill_items[i].getName() << " x" << Quantity;

                return;
            }

            else if (m_bill_items[i].getQuantity() == Quantity) {

                total_price -= m_bill_items[i].getPricePerUnit() * Quantity;


                add_quantity(id, Quantity); // Restore removed items to inventory [only update the quantity]


                m_bill_items.erase(m_bill_items.begin()+i); // remove the item entirely



                return;
            }

            return;

        }
    }
    qDebug() << "Item not found";
}


QVector <BillItem> BillManager::Get_Bill_list(){
    return m_bill_items;
}

QString BillManager::getSaveDirectory() const{
    return m_saveDirectory;
}

QString BillManager::getCustomerName(){
    return customer_name;
}

// Print bill details for debugging
double BillManager::get_bill_total() {
    qDebug() << "Customer: " << customer_name;
    qDebug() << "----------------------------";
    for (const BillItem &item : m_bill_items) {
        qDebug() << "Item: " << item.getName() << " | Qty: " << item.getQuantity()
        << " | Price: $" << item.getPricePerUnit() << " | Total: $" << item.getTotalPrice();
    }
    qDebug() << "----------------------------";
    qDebug() << "Total Bill Amount: $" << total_price;

    return total_price;
}


void BillManager::setCustomerName(QString name){
    customer_name = name;
    return;
}

void BillManager::setSaveDirectory(QString directory){
    m_saveDirectory = directory;

}


void BillManager::emptyingBill(){

    for (int i = 0; i < m_bill_items.size(); i++) {

        total_price -= m_bill_items[i].getTotalPrice();


        add_quantity(m_bill_items[i].getID(),m_bill_items[i].getQuantity()); // Restore removed items to inventory [only update the quantity]

        qDebug() << "remove item: " << m_bill_items[i].getName();


    }

    m_bill_items.clear(); // clear all the elements in the Bill item array
    qDebug() << "size of array: " << m_bill_items.size();
    return;
}

bool BillManager::generateBillPDF() {

    QString DateTimeForSaving = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss_");

    QString saveDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    if (! m_saveDirectory.isEmpty()) {
        saveDirectory = m_saveDirectory;
    }


    QString filePath = saveDirectory + "/bill_" + DateTimeForSaving +  getCustomerName() + ".pdf";

    // QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/bill_" + DateTimeForSaving + getCustomerName() + ".pdf";


    // run the billing machine function [if billing machine exist]


    // Delete the file if it already exists
    QFile file(filePath);
    if (file.exists()) {
        file.remove();
    }

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A6));
    pdfWriter.setResolution(300);

    QPainter painter;
    if (!painter.begin(&pdfWriter)) {
        qDebug() << "Failed to open PDF file for writing!";
        return false;
    }

    // Font settings
    QFont titleFont("Arial", 12, QFont::Bold);
    QFont textFont("Arial", 6);

    // Page Margins
    int xStart = 20; // Left margin
    int yStart = 100; // Top margin

    // Draw Title
    painter.setFont(titleFont);
    painter.drawText(xStart+350, yStart, getShopName());

    yStart +=50;
    painter.setFont(textFont);
    // Get the current date and time

    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    painter.drawText(xStart, yStart, "Date & Time: " + currentDateTime);



    // Draw Customer Name
    yStart += 60; // Move down

    painter.drawText(xStart, yStart, "Customer: " + getCustomerName());

    // Draw Table Headers
    yStart += 100;
    painter.setFont(QFont("Arial", 7, QFont::Bold));
    int column1 = xStart;
    int column2 = xStart + 80;
    int column3 = xStart + 370;
    int column4 = xStart + 570;
    int column5 = xStart + 770;

    painter.drawText(column1, yStart, "NO");
    painter.drawText(column2, yStart, "ITEM");
    painter.drawText(column3, yStart, "QTY");
    painter.drawText(column4, yStart, "PRICE");
    painter.drawText(column5, yStart, "AMOUNT");

    painter.drawLine(20, yStart-40, 1000,yStart-40 ); // (x1, y1, x2, y2)
    painter.drawLine(20, yStart+10, 1000,yStart+10 ); // (x1, y1, x2, y2)



    // Draw Bill Items
    painter.setFont(QFont("Arial", 6));
    yStart += 60;
    int ItemNumber = 1;

    double grandTotal = 0.0;
    for (const BillItem& item : m_bill_items) {
        double totalPrice = item.getQuantity() * item.getPricePerUnit();
        grandTotal += totalPrice;
        painter.drawText(column1, yStart, QString::number(ItemNumber));
        painter.drawText(column2, yStart, item.getName());
        yStart +=30;
        painter.drawText(column2, yStart, QString::number(item.getID()));
        // painter.drawText(column2, yStart, item.getName());
        painter.drawText(column3, yStart, QString::number(item.getQuantity()));
        painter.drawText(column4, yStart, QString::number(item.getPricePerUnit(), 'f', 2));
        //QRect(int x, int y, int width, int height)

        QRect totalPriceRect(column5, yStart-20, 150, 40); // Adjust width as needed
        painter.drawText(totalPriceRect, Qt::AlignRight, QString::number(totalPrice, 'f', 2));


        // painter.drawText(column5, yStart, QString::number(totalPrice, 'f', 2));
        ItemNumber ++;
        yStart += 80; // Move to next row
    }

    // Draw Grand Total
    // painter.setFont(QFont("Arial", 12, QFont::Bold));
    yStart -=20;
    painter.drawLine(20, yStart, 1000,yStart ); // (x1, y1, x2, y2)
    painter.drawLine(20, yStart+5, 1000,yStart+5 ); // (x1, y1, x2, y2)
    yStart += 50;
    painter.drawText(column2, yStart, "Net Total:");
    QRect totalPriceRect(column5, yStart-20, 150, 40); // Adjust width as needed

    painter.drawText(totalPriceRect, Qt::AlignRight, QString::number(grandTotal, 'f', 2));

    // painter.drawText(column5, yStart, QString::number(grandTotal,'f',2));

    // End painting
    painter.end();
    return true;
}

QString BillManager::getShopName(){
    return m_shopName;
}

void BillManager::setShopName(QString shopName){
    m_shopName = shopName;
}


void BillManager::generateLogsPDF(){

    QString DateTimeForSaving = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss_");

    QString saveDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    if (! m_saveDirectory.isEmpty()) {
        saveDirectory = m_saveDirectory;
    }


    QString filePath = saveDirectory + "/inventory_logs_" + DateTimeForSaving +  getShopName() + ".pdf";
    QFile file(filePath);
    if (file.exists()) {
        file.remove();
    }

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A3));
    pdfWriter.setResolution(300);

    QPainter painter;
    if (!painter.begin(&pdfWriter)) {
        qDebug() << "Failed to open PDF file for writing!";
        return;
    }

    // Font settings
    QFont titleFont("Arial", 16, QFont::Bold);
    QFont textFont("Arial", 12);

    // Page Margins
    int xStart = 20; // Left margin
    int yStart = 100; // Top margin


    painter.setFont(titleFont);
    painter.drawText(xStart+1050, yStart, getShopName()+" - Inventory Logs");

    yStart +=100;
    painter.setFont(textFont);


    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    yStart += 10;
    painter.drawText(xStart, yStart, "Date & Time: " + currentDateTime);


    yStart += 60;

    painter.drawText(xStart, yStart, "Shop: " + getShopName());
    yStart += 100;

    //Drawing logs
    QFont logFont("Arial", 8);
    painter.setFont(logFont);
    for (uint8_t i=0 ; i< m_inventory_logs.size() ; i++){
        painter.drawText(xStart, yStart, QString::number(i+1) + "]. " + m_inventory_logs[i]);
        yStart += 80;

    }

    return;

}
