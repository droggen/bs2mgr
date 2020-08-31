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

#ifndef __PORTWINDOW_H
#define __PORTWINDOW_H

#include <QtGlobal>


#include <QDialog>
#include <QTableWidgetItem>
#include <QList>

#ifndef Q_OS_WINRT
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#endif


namespace Ui {
    class PortWindow;
}

class PortWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PortWindow(QWidget *parent = 0);
    ~PortWindow();

    QString getSelectedPort();
    QList<QString> getSelectedPorts();
    static QList<QString> getPorts(bool ftdionly,bool stonly);


private slots:
    void on_uitwPorts_cellDoubleClicked(int row, int column);

    void accept();

    void on_uirbShowAll_clicked();

    void on_uirbShowFTDI_clicked();

    void on_uirbShowST_clicked();

private:
    Ui::PortWindow *ui;

    QString selectedport;

    void populate();
    #ifndef Q_OS_WINRT
    static QList<QSerialPortInfo> listPorts(bool ftdionly,bool stonly);
    #endif

};



#endif // __PORTWINDOW_H
