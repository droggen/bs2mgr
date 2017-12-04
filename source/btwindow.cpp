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
#include "btwindow.h"
#include "ui_btwindow.h"

#include <QBluetoothUuid>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothAddress>
#include <QMessageBox>

BtWindow::BtWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BtWindow)
{
    ui->setupUi(this);

    // Discover devices
    devDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    //QBluetoothServiceDiscoveryAgent *srvDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(this);
    connect(devDiscoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    connect(devDiscoveryAgent, SIGNAL(finished()),this, SLOT(deviceFinished()));
    //connect(devDiscoveryAgent, SIGNAL(canceled()),this, SLOT(deviceCanceled()));


    // Start a discovery
    devDiscoveryAgent->start();

    // Setup a few fake entries
    /*
    QBluetoothAddress ba("12:12:31:31:22:33");
    QBluetoothDeviceInfo di(ba,"name1",123);
    bdi = addBDI(bdi,di);

    ba = QBluetoothAddress("33:44:55:66:77:88");
    di = QBluetoothDeviceInfo (ba,"name2",123);
    bdi = addBDI(bdi,di);

    ba = QBluetoothAddress("AA:BB:55:66:77:88");
    di = QBluetoothDeviceInfo (ba,"name3",123);
    bdi = addBDI(bdi,di);

    ba = QBluetoothAddress("CC:DD:55:66:77:88");
    di = QBluetoothDeviceInfo (ba,"name4",123);
    bdi = addBDI(bdi,di);

    ba = QBluetoothAddress("EE:FF:55:66:77:88");
    di = QBluetoothDeviceInfo (ba,"name5",123);
    bdi = addBDI(bdi,di);


    ba = QBluetoothAddress("AA:BB:55:66:77:88");
    di = QBluetoothDeviceInfo (ba,"name3",123);
    bdi = addBDI(bdi,di);*/

    populateTable(false);
}

BtWindow::~BtWindow()
{
    devDiscoveryAgent->stop();
    delete devDiscoveryAgent;
    delete ui;
}

QList<QBluetoothDeviceInfo> BtWindow::addBDI(QList<QBluetoothDeviceInfo> allbdi,QBluetoothDeviceInfo newbdi)
{
    // Add newbdi to allbdi only if new
    bool exist=false;
    foreach(auto bdi,allbdi)
    {
        if(bdi==newbdi)
        {
            exist=true;
            break;
        }
    }
    if(!exist)
        allbdi.append(newbdi);
    return allbdi;
}


void BtWindow::deviceDiscovered(QBluetoothDeviceInfo di)
{
    // Check if we want only roving networks
    QBluetoothAddress ba = di.address();
    if( (ui->uicbRovingNetworksOnly->isChecked() && ba.toString().startsWith("00:06:66")) || !ui->uicbRovingNetworksOnly->isChecked())
    {
        bdi = addBDI(bdi,di);
        populateTable(true);
    }
}

void BtWindow::serviceDiscovered(QBluetoothServiceInfo si)
{
    qDebug("discovered service\n");

    qDebug("Service desc: %s\n",si.serviceDescription().toStdString().c_str());
    qDebug("Service name: %s\n",si.serviceName().toStdString().c_str());
    qDebug("Service prov: %s\n",si.serviceProvider().toStdString().c_str());

    qDebug("************************************************ Hello\n");

}

QString BtWindow::coreConfigurationToString(QBluetoothDeviceInfo::CoreConfigurations c)
{
    switch(c)
    {
    case 1:
        return "BLE";
        break;
    case 2:
        return "STD";
        break;
    case 3:
        return "STD+BLE";
        break;
    default:
        return "?";
    }
    return "?";
}

void BtWindow::populateTable(bool expandonly)
{
    if(!expandonly)
    {
        // Erase all enties
        ui->uitwPorts->setRowCount(0);
    }

    int oldsize = ui->uitwPorts->rowCount();

    // Fill the table
    int from,to;
    if(expandonly)
    {
        from = oldsize;
        to = bdi.size();
    }
    else
    {
        from = 0;
        to = bdi.size();
    }

    ui->uitwPorts->setRowCount(bdi.size());
    for(int i=from;i<to;i++)
    {
        // Table format: Name MAC Type Major:Minor Class RSSI SrvUUID
        printf("Populate table %d\n",i);
        QTableWidgetItem *newItem;

        newItem = new QTableWidgetItem(bdi.at(i).name());
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,0, newItem);

        // TODO: on iOS/OSX must replace address by uuid, as the os does not return the address
        newItem = new QTableWidgetItem(bdi.at(i).address().toString());
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,1, newItem);

        newItem = new QTableWidgetItem(coreConfigurationToString(bdi.at(i).coreConfigurations()));
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,2, newItem);

        newItem = new QTableWidgetItem(QString::number(bdi.at(i).majorDeviceClass())+":"+QString::number(bdi.at(i).minorDeviceClass()));
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,3, newItem);

        newItem = new QTableWidgetItem(QString::number(bdi.at(i).serviceClasses()));
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,4, newItem);

        newItem = new QTableWidgetItem(QString::number(bdi.at(i).rssi()));
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,5, newItem);


        QBluetoothDeviceInfo::DataCompleteness c = bdi.at(i).serviceUuidsCompleteness();
        QList<QBluetoothUuid> siid = bdi.at(i).serviceUuids();
        QString siidstr="";
        printf("number of service uuid %d\n",siid.size());
        for(int j=0;j<siid.size();j++)
        {
            // TODO: use toUInt128 when QString::number will support quint128
            siidstr+=QString::number(siid.at(j).toUInt32());
            siidstr+=";";
        }
        if(c==QBluetoothDeviceInfo::DataIncomplete)
        {
            if(siidstr=="")
                siidstr="N/A";
            else
                siidstr+=" (partial)";
        }
        if(c==QBluetoothDeviceInfo::DataUnavailable)
            siidstr+="N/A";


        newItem = new QTableWidgetItem(siidstr);
        //newItem->setFlags(Qt::NoItemFlags);
        ui->uitwPorts->setItem(i,6, newItem);
    }

    ui->uitwPorts->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->uitwPorts->horizontalHeader()->setStretchLastSection(true);

    // Select first row, if available
    /*if(bdi.size()>0)
    {
        ui->uitwPorts->selectRow(0);
    }*/
}


void BtWindow::deviceFinished()
{
    qDebug("Scanning finished\n");
    // Restart scanning
    devDiscoveryAgent->start();
}
void BtWindow::deviceCanceled()
{
    qDebug("Scanning cancelled\n");
    // Restart scanning
    devDiscoveryAgent->start();
}

void BtWindow::on_uitwPorts_cellDoubleClicked(int row, int column)
{
    // Get the leftmost column which is the name
    //printf("%s\n",ui->uitwPorts->item(row,0)->text().toStdString().c_str());

    accept();
}


void BtWindow::accept()
{
    // Stop scanning
    devDiscoveryAgent->stop();

    // Check which line is selected
    int row = ui->uitwPorts->currentRow();

    printf("current row: %d\n",row);

    if(row!=-1)
    {
        selectedport = ui->uitwPorts->item(row,1)->text();
    }


    QDialog::accept();
}

QString BtWindow::getSelectedPort()
{
    return selectedport;
}


QList<QString> BtWindow::getSelectedPorts()
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
        //ports.append(ui->uitwPorts->item(i,0)->text());
        ports.append(ui->uitwPorts->item(i,1)->text());
    }

    //return selectedport;
    return ports;
}

void BtWindow::on_uicbRovingNetworksOnly_toggled(bool)
{
    qDebug("Stop scanning\n");
    devDiscoveryAgent->stop();
    // Clear the devices and restart discovery
    qDebug("Clear\n");
    bdi.clear();
    qDebug("Populate\n");
    populateTable(false);

    devDiscoveryAgent->start();

}
