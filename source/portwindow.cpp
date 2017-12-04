/*
   Copyright (C) 2017:
         Daniel Roggen, droggen@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "portwindow.h"
#include "ui_portwindow.h"


PortWindow::PortWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortWindow)
{
   ui->setupUi(this);





   populate();

}

PortWindow::~PortWindow()
{
    delete ui;
}
QList<QSerialPortInfo> PortWindow::listPorts(bool ftdionly)
{
    // Fill the dialog
    QList<QSerialPortInfo> portsall = QSerialPortInfo::availablePorts();

    QList<QSerialPortInfo> ports;
    foreach (auto p, portsall) {
        if(!ftdionly || (ftdionly && p.manufacturer()=="FTDI"))
        {
            printf("Append %s\n",p.manufacturer().toStdString().c_str());
            ports.append(p);
        }
        printf("%s: %d\n",p.manufacturer().toStdString().c_str(),p.manufacturer()=="FTDI");


    }
    return ports;
}
QList<QString> PortWindow::getPorts(bool ftdionly)
{
    QList<QSerialPortInfo> ports = listPorts(ftdionly);
    QList<QString> pn;

    for (int i = 0; i < ports.size(); i++)
    {
        pn.append(ports.at(i).portName());
    }
    return pn;
}


void PortWindow::populate()
{
    bool ftdionly = ui->uicbFTDIOnly->isChecked();

    QList<QSerialPortInfo> ports = listPorts(ftdionly);

    ui->uitwPorts->setRowCount(0);

    ui->uitwPorts->setRowCount(ports.size());


    for (int i = 0; i < ports.size(); i++)
    {

        QTableWidgetItem *newItem;
        newItem = new QTableWidgetItem(ports.at(i).portName());
        //newItem->setFlags(Qt::NoItemFlags);
        //newItem->setFlags(Qt::ItemIsEnabled);
        ui->uitwPorts->setItem(i,0, newItem);
        newItem = new QTableWidgetItem(ports.at(i).description());
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,1, newItem);
        newItem = new QTableWidgetItem(ports.at(i).manufacturer());
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,2, newItem);
        newItem = new QTableWidgetItem(QString::number(ports.at(i).productIdentifier()));
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,3, newItem);
        newItem = new QTableWidgetItem(QString::number(ports.at(i).vendorIdentifier()));
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,4, newItem);
        newItem = new QTableWidgetItem(ports.at(i).serialNumber());
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,5, newItem);
    }
    ui->uitwPorts->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->uitwPorts->horizontalHeader()->setStretchLastSection(true);

    // Select first row, if available
     if(ports.size()>0)
     {
         ui->uitwPorts->selectRow(0);
     }
}

void PortWindow::on_uitwPorts_cellDoubleClicked(int row, int column)
{
    printf("cell double clicked\n");
    // Get the leftmost column which is the name
    printf("%s\n",ui->uitwPorts->item(row,0)->text().toStdString().c_str());

    accept();
}



void PortWindow::accept()
{
    // Check which line is selected
    int row = ui->uitwPorts->currentRow();

    if(row!=-1)
    {
        selectedport = ui->uitwPorts->item(row,0)->text();
    }


    QDialog::accept();
}

QString PortWindow::getSelectedPort()
{
    return selectedport;
}


QList<QString> PortWindow::getSelectedPorts()
{
    // Get all the selection ranges and put each row in a list
    QList<QTableWidgetSelectionRange> sr = ui->uitwPorts->selectedRanges();
    QList<int> rows;
    QList<QString> ports;
    foreach(auto i,sr)
    {
        //printf("selection range: %d %d - %d %d\n",i.leftColumn(),i.topRow(),i.rightColumn(),i.bottomRow());
        for(int j=i.topRow();j<=i.bottomRow();j++)
            rows.append(j);
    }

    qSort(rows);

    foreach(auto i,rows)
    {
        printf("%d\n",i);
    }

    foreach(auto i,rows)
    {
        ports.append(ui->uitwPorts->item(i,0)->text());
    }

    //return selectedport;
    return ports;
}



void PortWindow::on_uicbFTDIOnly_clicked()
{
    populate();
}
