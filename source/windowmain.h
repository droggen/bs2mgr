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

#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H


#include <QMainWindow>
#include <QThread>
#include "widgetterminal.h"
#include "device.h"
#include "deviceworker.h"
#include "annotation.h"

namespace Ui {
class WindowMain;
}

class WindowMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowMain(QWidget *parent = 0);
    ~WindowMain();

private slots:
    void on_uipbUp_clicked();

    void on_uipbDown_clicked();

    void on_uipbLeft_clicked();

    void on_uipbRight_clicked();

    void on_uipbRemove_clicked();

    void on_uipbCommandGetTime_clicked();
    void on_uipbCommandCustom_clicked();
    void on_uipbCommandGetBat_clicked();
    void on_uipbCommandGetPower_clicked();
    void on_uipbAddAllUSB_clicked();

    void on_uipbCommandIdentify_clicked();

    void on_uipbClear_clicked();

    void on_uipbSyncDateTime_clicked();


    void on_uipbCommandM33_clicked();

    void on_uipbCommandQuit_clicked();

    void on_uipbCommandReboot_clicked();

    void on_uipbCommandOff_clicked();

    void on_uipbCommandBoot_clicked();

    void on_uipbCommandQuit2_clicked();

    void on_uipbClose_clicked();

    void on_uipbAbort_clicked();

    void on_uipbRetry_clicked();

    void on_uipbSelectUSB_clicked();

    void on_uipbSelectBluetooth_clicked();


    void on_uipbCommandGetDate_clicked();

    void on_uipbCommandSDInitialize_clicked();

    void on_uipbCommandFormat14_clicked();

    void on_uipbHelp_clicked();

    void on_uirbAnn1_clicked();

    void on_uirbAnn2_clicked();

    void on_uirbAnn3_clicked();

    void on_uirbAnn4_clicked();

    void on_uirbAnn5_clicked();

    void on_uirbAnn7_clicked();

    void on_uirbAnn8_clicked();

    void on_uirbAnn9_clicked();

    void on_uirbAnn10_clicked();

    void on_uirbAnnNull_clicked();

    void on_uipbAnnotationSave_clicked();

    void on_uipbAnnotationClear_clicked();

    void on_uirbAnn6_clicked();

    void on_uipbCommandHelp_clicked();

    void on_uipbCommandM19_clicked();

    void on_uipbCommandM41_clicked();

    void on_uipbCommandM44_clicked();

    void on_uipbCommandFTextAll_clicked();

    void on_uipbCommandFTextNone_clicked();

    void on_uipbAnnotationCopy_clicked();

    //void on_pushButton_clicked();

    void on_uipbAddAllUSBST_clicked();

    void on_uipbCommangGetDateNew_clicked();

private:
    Ui::WindowMain *ui;
    Controller controller;

    QList<DDEVICE> devices;

    QList<WidgetTerminal*> tabs;
    DEVICE_ACTION currentDeviceAction;      // Command passed to the worker thread for the device
    QString currentDeviceActionParam;       // Parameter passed to the worker thread for the device
    int currentDeviceActionTimeout;         // Timeout parameter passed to the worker thread
    int currentDeviceConnectTimeout;
    int currentDevice;                      // Current device being processed
    bool mustAbort;                         // Indicate whether to abort processing the next devices

    QVector<ANNOTATION> Annotations;        // Holds annotations

    void populateDevices();
    //void printDevices();

    void processNextDevice();
    void doCommand(DEVICE_ACTION action, QString action_param, int action_timeout=0);
    void addUSBToPortList(const QList<QString> &ports);
    void addBTToPortList(const QList<QString> &ports);
    void uiModeCloseRetry();
    void uiModeProcessing();

    int getCommandTimeout();
    int getCommandLongTimeout();
    int getConnectTimeout();

    bool saveSettings(QString=QString());
    bool loadSettings(QString=QString());
    bool saveSettingsAnnotations(QString=QString());

    int getTimestamp();
    void addAnnotation(int label);
    void printAnnotations();
    void printAnnotations(QTextStream &out);
    QString printAnnotationsInString(bool numericalonly=false);
    void printAnnotationsInTerminal();
    QString saveDialog(QString title);

public slots:
    void getControllerStatus(DEVICE_ACTION_RESULT result,const QByteArray &ba);
    void getPrintToTerminal(const QString &str);

    //void openFileNameReady(QString fileName);


};

#endif // WINDOWMAIN_H
