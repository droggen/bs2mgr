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


#ifndef __BTWINDOW_H
#define __BTWINDOW_H


#include <QDialog>
#include <QList>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>

namespace Ui {
    class BtWindow;
}

class BtWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BtWindow(QWidget *parent = 0);
    ~BtWindow();

    QString getSelectedPort();
    QList<QString> getSelectedPorts();

private:
    Ui::BtWindow *ui;
    QBluetoothDeviceDiscoveryAgent *devDiscoveryAgent;

    QString selectedport;

    QList<QBluetoothDeviceInfo> bdi;
    void populateTable(bool expandonly);
    QString coreConfigurationToString(QBluetoothDeviceInfo::CoreConfigurations c);


    QList<QBluetoothDeviceInfo> addBDI(QList<QBluetoothDeviceInfo> allbdi,QBluetoothDeviceInfo newbdi);




private slots:
    void on_uitwPorts_cellDoubleClicked(int row, int column);
    void deviceDiscovered(QBluetoothDeviceInfo);
    void serviceDiscovered(QBluetoothServiceInfo);
    void deviceFinished();
    void deviceCanceled();
    void accept();
    void on_uicbRovingNetworksOnly_toggled(bool);
};

#endif // __BTWINDOW_H
