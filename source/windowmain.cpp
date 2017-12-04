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
/*
TODO:
- Font size terminal
- Icon on windows
*/
/*
    DeviceWorker Returns the following status:

        During processing:
            DEVICE_ACTION_RESULT_START              : Indicates start of worker thread; visualized in black
            DEVICE_ACTION_RESULT_CONNECTING         : Indicates attempt to connect; not visualized
            DEVICE_ACTION_RESULT_CONNECTED          : Indicates attempt to connect; not visualized
            DEVICE_ACTION_RESULT_DISCONNECTED:      Indicates attempt to connect; not visualized

        To indicate completion of processing:
            DEVICE_ACTION_RESULT_DONE_SUCCESS
            DEVICE_ACTION_RESULT_DONE_ERROR
            DEVICE_ACTION_RESULT_DONE_UNDEFINED

*/

// Tab title on 2 lines: https://forum.qt.io/topic/21391/tab-widget-tab-height-to-accommodate-2-line-tab-names






#include "windowmain.h"
#include "ui_windowmain.h"
#include "portwindow.h"
#include "btwindow.h"
#include "helpwindow.h"
#include "androidfiledialog.h"
#include "widgetterminal.h"


#include "cio.h"

WindowMain::WindowMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowMain)
{
    ui->setupUi(this);

#ifdef DEVELMODE
    ConsoleInit();
    printf("Starting up\n");
#endif
    // Desactivate USB on Android
#ifdef Q_OS_ANDROID
    ui->uipbSelectUSB->setVisible(false);
    ui->uipbAddAllUSB->setVisible(false);
#endif
#ifdef Q_OS_ANDROID
    //this->setWindowState(this->windowState() | Qt::WindowFullScreen);
#endif


    QString styleSheet = QString("font-size:%1px;").arg(QApplication::font().pointSize());
    //this->setStyleSheet(styleSheet);


    // Get status notifications from controller
    //connect(worker, &Worker::signalStatus, this, &Controller::handleStatus);
    connect(&controller,&Controller::signalStatus,this,&WindowMain::getControllerStatus);
    connect(&controller,&Controller::signalPrintToTerminal,this,&WindowMain::getPrintToTerminal);

    loadSettings();


    /*DEVICE d;
    d.type=DEVICE_SER;
    d.port="COM11";
    d.mac="MAC";
    devices.append(d);

    //d.port="COM14";
    //devices.append(d);

    d.type=DEVICE_BT;
    devices.append(d);

    //d.port="COM15";
    //devices.append(d);

    //d.port="COM16";
    //devices.append(d);
*/


    populateDevices();

#ifndef DEVELMODE
    ui->uifLeftRightNav->setVisible(false);
#endif
}

WindowMain::~WindowMain()
{
    addAnnotation(0);
    saveSettings();
    delete ui;
}
void WindowMain::on_uipbAddAllUSB_clicked()
{
    QList<QString> ports = PortWindow::getPorts(true);
    addUSBToPortList(ports);
    printDevices(devices);
    populateDevices();
}
void WindowMain::addUSBToPortList(const QList<QString> &ports)
{
    foreach (auto p, ports)
    {
        DEVICE d;
        d.type = DEVICE_SER;
        d.port = p;
        devices.append(d);
    }
    devices = uniqueDevices(devices);
}
void WindowMain::addBTToPortList(const QList<QString> &ports)
{
    foreach (auto p, ports)
    {
        DEVICE d;
        d.type = DEVICE_BT;
        d.mac = p;
        devices.append(d);
    }
    devices = uniqueDevices(devices);
}

void WindowMain::on_uipbSelectUSB_clicked()
{
    QString port;
    PortWindow dialog(this);
    // The below should only be done on Android
    #ifdef Q_OS_ANDROID
        dialog.setWindowState(dialog.windowState() | Qt::WindowMaximized);
    #endif
    int ret = dialog.exec();
    if(ret == QDialog::Accepted)
    {
        QList<QString> ports = dialog.getSelectedPorts();
        addUSBToPortList(ports);
        printDevices(devices);
        populateDevices();
    }
}

void WindowMain::on_uipbSelectBluetooth_clicked()
{
    BtWindow dialog(this);
    // The below should only be done on Android
#ifdef Q_OS_ANDROID
    dialog.setWindowState(dialog.windowState() | Qt::WindowMaximized);
#endif
    int ret = dialog.exec();
    if(ret == QDialog::Accepted)
    {
        QList<QString> ports = dialog.getSelectedPorts();
        addBTToPortList(ports);
        printDevices(devices);
        populateDevices();
    }
}



void WindowMain::populateDevices()
{
    ui->uilwDevices->clear();

    foreach (auto d, devices)
    {
        ui->uilwDevices->addItem(Device2Str(d));
    }

}

void WindowMain::on_uipbUp_clicked()
{
    int row = ui->uilwDevices->currentRow();
    if(row==-1)
       return;
    if(row==0)
       return;

    //printf("Device length: %d\n",Devices.length());

    DEVICE d = devices[row];
    devices[row] = devices[row-1];
    devices[row-1] = d;

    populateDevices();

    ui->uilwDevices->setCurrentRow(row-1);
}

void WindowMain::on_uipbDown_clicked()
{
    int row = ui->uilwDevices->currentRow();
    if(row==-1)
       return;
    if(row>=ui->uilwDevices->count()-1)
       return;


    DEVICE d = devices[row];
    devices[row] = devices[row+1];
    devices[row+1] = d;

    populateDevices();

    ui->uilwDevices->setCurrentRow(row+1);
}

void WindowMain::on_uipbLeft_clicked()
{
    printf("-1\n");
    // Change the widget
    int current = ui->stackedWidget->currentIndex();
    printf("current: %d/%d\n",current,ui->stackedWidget->count());
    if(current<=0)
        return;
    ui->stackedWidget->setCurrentIndex(current-1);
}

void WindowMain::on_uipbRight_clicked()
{
    printf("+1\n");
    // Change the widget
    int current = ui->stackedWidget->currentIndex();
    printf("current: %d/%d\n",current,ui->stackedWidget->count());
    if(current>=ui->stackedWidget->count()-1)
        return;
    ui->stackedWidget->setCurrentIndex(current+1);
}

void WindowMain::on_uipbRemove_clicked()
{
    // Find which device to remove
    int row = ui->uilwDevices->currentRow();
    printf("row: %d\n",row);
    if(row!=-1)
    {
        DEVICE d = devices[row];
        printf("Remove device row %d: %s\n",row,Device2Str(d).toStdString().c_str());

        devices.removeAt(row);

        populateDevices();

        // Select either row or row-1, or nothing if empty
        if(devices.size())
        {
            if(row>=devices.size())
                ui->uilwDevices->setCurrentRow(row-1);
            else
                ui->uilwDevices->setCurrentRow(row);
        }
    }
}
void WindowMain::on_uipbClear_clicked()
{
    // Clear all selected devices
    devices.clear();

    populateDevices();
}







void WindowMain::getControllerStatus(DEVICE_ACTION_RESULT result, const QByteArray &ba)
{
    qDebug("WindowMain::getControllerstatus: %d %s\n",result,ba.constData());
    // Check whatever we get and update accordingly
    switch(result)
    {
        case DEVICE_ACTION_RESULT_START:
            // Change the color to indicate active
            ui->uitwDeviceTabs->tabBar()->setTabTextColor(currentDevice,Qt::black);
            break;
        case DEVICE_ACTION_RESULT_CONNECTING:
            qDebug("connecting\n");
            break;
        case DEVICE_ACTION_RESULT_CONNECTED:
            qDebug("Connected\n");
            break;
        case DEVICE_ACTION_RESULT_DISCONNECTED:
            qDebug("Disconnected\n");
            break;
        case DEVICE_ACTION_RESULT_DONE_ERROR:
            ui->uitwDeviceTabs->tabBar()->setTabTextColor(currentDevice,Qt::red);
            // Store the failure
            devices[currentDevice].ok = false;
            break;
        case DEVICE_ACTION_RESULT_DONE_SUCCESS:
        {
            ui->uitwDeviceTabs->tabBar()->setTabTextColor(currentDevice,Qt::blue);

            // Check which command returned
            if(currentDeviceAction == DEVICE_ACTION_IDENTIFY)
            {
                // Obtain the identity of the node
                devices[currentDevice].name = QString(ba);
                populateDevices();
            }

            // Store the success
            devices[currentDevice].ok = true;

            break;
        }
        case DEVICE_ACTION_RESULT_DONE_UNDEFINED:
            ui->uitwDeviceTabs->tabBar()->setTabTextColor(currentDevice,Qt::darkMagenta);

            // Store the failure
            devices[currentDevice].ok = false;
            break;
        default:
            qDebug("Unhandled status\n");
    }
    // Go to the next device only on "done" results
    if(result==DEVICE_ACTION_RESULT_DONE_ERROR || result==DEVICE_ACTION_RESULT_DONE_SUCCESS || result==DEVICE_ACTION_RESULT_DONE_UNDEFINED)
        processNextDevice();

}
void WindowMain::getPrintToTerminal(const QString &str)
{
    //printf("WindowMain::getPrintToTerminal: %s\n",str.toStdString().c_str());

    // Print to current terminal (assume 0 now);
    static_cast<WidgetTerminal*>(ui->uitwDeviceTabs->widget(currentDevice))->addBytes(str.toLocal8Bit());
}







void WindowMain::processNextDevice()
{
    // Called to process the next device
    if(mustAbort)
    {
        // User indicated to abort the processing
        // Update the widget title color accordingly
        for(int i=0;i<devices.size();i++)
        {
            if(devices[i].ok)
            {
                ui->uitwDeviceTabs->tabBar()->setTabTextColor(i,Qt::blue);
            }
            else
            {
                ui->uitwDeviceTabs->tabBar()->setTabTextColor(i,Qt::red);
            }
        }
        // Set UI to "retry/close" mode
        uiModeCloseRetry();
        return;
    }

    // Go to next device which is not ok
    while(1)
    {
        currentDevice++;
        // Check if we arrived at the end
        if(currentDevice>=ui->uitwDeviceTabs->count())
        {
            // Processed all the devices
            // Set UI to "retry/close" mode
            uiModeCloseRetry();
            // Terminate
            return;
        }
        // Process that next device only if not ok (so that when retrying the device is skipped)
        if(!devices[currentDevice].ok)
        {
            // Select the active device tab
            printf("Selecting current tab: %d\n",currentDevice);
            ui->uitwDeviceTabs->setCurrentIndex(currentDevice);

            // Initiate the processing of the next device.
            controller.start(devices[currentDevice],currentDeviceAction,currentDeviceActionParam,currentDeviceConnectTimeout,currentDeviceActionTimeout);
            // Return
            return;
        }
        // If the currentDevice is ok, try the next one
    }
}

void WindowMain::on_uipbCommandGetTime_clicked()
{
    doCommand(DEVICE_ACTION_GETTIME,"\n");
}
void WindowMain::on_uipbCommandGetDate_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"D\n");
}
void WindowMain::on_uipbCommandCustom_clicked()
{
    int timeout;
    timeout = getCommandTimeout();
    // Check if long timeout
    if(ui->uicbCommandCustomLongTimeout->isChecked())
        timeout=getCommandLongTimeout();

    doCommand(DEVICE_ACTION_CUSTOM,ui->uileCommand->text()+"\n",timeout);
    return;

    const char *str = ui->uileCommand->text().toStdString().c_str();


    // Does not process quote modes
    int numcmd=0;
    bool iscmd=false;

    for(unsigned i=0;i<=strlen(str);i++)     // Include the terminating 0
    {
        if(str[i]!=';' && str[i]!=0)
        {
            // ignore null commands.
            if(str[i]!=' ' && str[i]!='\t' && str[i]!='\n')
                iscmd=true;
        }
        else
        {
            // str[i] is ; or end of string - check if we got a command
            if(iscmd)
            {
                numcmd++;
                iscmd=0;
            }
        }
    }

    printf("Number of commands: %d\n",numcmd);



}
void WindowMain::on_uipbCommandGetBat_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"Q\n");
}
void WindowMain::on_uipbCommandGetPower_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"q\n");
}

void WindowMain::on_uipbCommandIdentify_clicked()
{
    doCommand(DEVICE_ACTION_IDENTIFY,"");
}

void WindowMain::on_uipbSyncDateTime_clicked()
{
    doCommand(DEVICE_ACTION_SYNCDATETIME,"");
}



void WindowMain::on_uipbCommandM33_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"M,33\n");
}
void WindowMain::on_uipbCommandM19_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"M,19\n");
}

void WindowMain::on_uipbCommandM41_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"M,41\n");
}

void WindowMain::on_uipbCommandM44_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"M,44\n");
}

void WindowMain::on_uipbCommandFTextAll_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"F,0,1,1,1,1\n");
}

void WindowMain::on_uipbCommandFTextNone_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"F,0,0,0,0,0\n");
}

void WindowMain::on_uipbCommandQuit_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"!\n");
}

void WindowMain::on_uipbCommandQuit2_clicked()
{
    on_uipbCommandQuit_clicked();
}

void WindowMain::on_uipbCommandReboot_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"O,3\n");
}

void WindowMain::on_uipbCommandOff_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"O\n");
}

void WindowMain::on_uipbCommandHelp_clicked()
{
   doCommand(DEVICE_ACTION_CUSTOM,"H\n");
}


void WindowMain::on_uipbCommandBoot_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"b\n");
}
void WindowMain::on_uipbCommandSDInitialize_clicked()
{
    doCommand(DEVICE_ACTION_CUSTOM,"X;I;!\n");
}

void WindowMain::on_uipbCommandFormat14_clicked()
{
    if(QMessageBox::critical(this, "Format?", "Confirm formatting",QMessageBox::Ok,QMessageBox::Cancel,QMessageBox::NoButton)==QMessageBox::Ok)
        doCommand(DEVICE_ACTION_CUSTOM,"X;F,14;!\n",ui->uisbTimeoutCommand->value()*1000);        // Format with extra timeout
}
int WindowMain::getCommandTimeout()
{
    return ui->uisbTimeoutCommand->value()*1000;
}
int WindowMain::getCommandLongTimeout()
{
    return ui->uisbTimeoutCommandLong->value()*1000;
}
int WindowMain::getConnectTimeout()
{
    return ui->uisbTimeoutConnect->value()*1000;
}
void WindowMain::doCommand(DEVICE_ACTION action,QString action_param,int action_timeout)
{
    // Memorize the actions
    currentDeviceAction=action;
    currentDeviceActionParam=action_param;
    currentDeviceActionTimeout=action_timeout;
    if(!currentDeviceActionTimeout)
        currentDeviceActionTimeout=getCommandTimeout();
    currentDeviceConnectTimeout=getConnectTimeout();

    // Initialise all the devices as "not ok"
    for(int i=0;i<devices.length();i++)
        devices[i].ok=false;
    // No abort
    mustAbort=false;

    // Create the user interface to display the device connection status;

    // Switch to terminals view
    ui->stackedWidget->setCurrentIndex(1);

    // Enable/disable the buttons
    uiModeProcessing();


    // Programmatically create tabs in uitwDeviceTabs
    // 1. Erase all tabs
    while(ui->uitwDeviceTabs->count())
        ui->uitwDeviceTabs->removeTab(0);
    // 2. Erase the list of tabs
    tabs.clear();
    // 3. Add one tab per device
    foreach(auto d,devices)
    {
        // Create the tab
        WidgetTerminal *wdt = new WidgetTerminal;
        // Set the tab device (which sets the tab title)
        wdt->setTitle(Device2Str(d));
        // Add the tab to the tabwidget
        ui->uitwDeviceTabs->addTab(wdt,Device2StrShort(d));
        // Keep a copy of the tab in a list. This should not be necessary as ui->uitwDeviceTabs->widget(n) should do that
        tabs.append(wdt);

    }
    // 4. Set tab title to grey
    for(int i=0;i<ui->uitwDeviceTabs->count();i++)
    {
        ui->uitwDeviceTabs->tabBar()->setTabTextColor(i,Qt::gray);
    }
    // Initialise currentDevice to -1 to trigger the processing with processNextDevice
    currentDevice=-1;

    // Trigger the processing
    processNextDevice();
}
void WindowMain::on_uipbRetry_clicked()
{
    // No abort
    mustAbort=false;

    // Enable/disable the buttons
    uiModeProcessing();

    // Initialise currentDevice to -1 to trigger the processing with processNextDevice
    currentDevice=-1;

    // Trigger the processing
    processNextDevice();
}









void WindowMain::on_uipbClose_clicked()
{
    // Processing the devices completed, return to main stacked widget
    ui->stackedWidget->setCurrentIndex(0);
}




void WindowMain::on_uipbAbort_clicked()
{
    // Indicate that we wish to abort the processing of the next devices
    mustAbort=true;
    // Change button status
    ui->uipbAbort->setEnabled(false);
}

void WindowMain::uiModeCloseRetry()
{
    // Puts the stacked widgets in the "close or retry" mode
    ui->uipbClose->setEnabled(true);
    // Check if there are any devices which have failed
    bool anyfailed=false;
    foreach(auto d,devices)
    {
        if(!d.ok)
            anyfailed=true;
    }
    if(anyfailed)
        ui->uipbRetry->setEnabled(true);
    else
        ui->uipbRetry->setEnabled(false);
    // No abort as there's nothing ongoing
    ui->uipbAbort->setEnabled(false);
    // Re-enable ability to change nodes in the node selection tab
    ui->uiwSelectNodeGroup->setEnabled(true);
}
void WindowMain::uiModeProcessing()
{
    // Puts the stacked widgets in the "processing" mode
    ui->uipbAbort->setEnabled(true);
    ui->uipbClose->setEnabled(false);
    ui->uipbRetry->setEnabled(false);
    // Disable ability to change nodes in the node selection tab
    ui->uiwSelectNodeGroup->setEnabled(false);
}



bool WindowMain::saveSettings(QString ini)
{
    QSettings *settings;

    if(ini==QString())
        settings=new QSettings(QSettings::IniFormat,QSettings::UserScope,"danielroggen","bs2mgr");
    else
        settings=new QSettings(ini,QSettings::IniFormat);

    if(settings->status()!=QSettings::NoError)
        return true;

    qDebug("Storing settings to %s\n",settings->fileName().toStdString().c_str());


    settings->setValue("TimeoutConnect",ui->uisbTimeoutConnect->value());
    settings->setValue("TimeoutCommand",ui->uisbTimeoutCommand->value());
    settings->setValue("TimeoutCommandLong",ui->uisbTimeoutCommandLong->value());

    // Save non-null annotations
    settings->beginWriteArray("annotations");
    for (int i = 0; i < Annotations.size(); i++)
    {
        if(Annotations.at(i).annotation!=0)
        {
            settings->setArrayIndex(i);
            settings->setValue("annotation", Annotations.at(i).annotation);
            settings->setValue("timestart", Annotations.at(i).timestart);
            settings->setValue("timeend", Annotations.at(i).timeend);
        }
    }
     settings->endArray();

    delete settings;
    return false;
}
bool WindowMain::loadSettings(QString fileName)
{
    QSettings *settings;

    if(fileName==QString())
        settings = new QSettings(QSettings::IniFormat,QSettings::UserScope,"danielroggen","bs2mgr");
    else
        settings = new QSettings(fileName,QSettings::IniFormat);

    if(settings->status()!=QSettings::NoError)
        return true;

    int tconn = settings->value("TimeoutConnect",10).toInt();
    int tcmd = settings->value("TimeoutCommand",5).toInt();
    int tcmdlong = settings->value("TimeoutCommandLong",200).toInt();

    ui->uisbTimeoutConnect->setValue(tconn);
    ui->uisbTimeoutCommand->setValue(tcmd);
    ui->uisbTimeoutCommandLong->setValue(tcmdlong);

    // Load annotations
    int size = settings->beginReadArray("annotations");
    for (int i = 0; i < size; ++i)
    {
        settings->setArrayIndex(i);
        ANNOTATION a;
        a.annotation = settings->value("annotation").toInt();
        a.timestart = settings->value("timestart").toInt();
        a.timeend = settings->value("timeend").toInt();
        Annotations.append(a);
    }
    settings->endArray();

    // Append a null annotation
    addAnnotation(0);

    return false;
}

bool WindowMain::saveSettingsAnnotations(QString ini)
{
    QSettings *settings;

    if(ini==QString())
        settings=new QSettings(QSettings::IniFormat,QSettings::UserScope,"danielroggen","bs2mgr");
    else
        settings=new QSettings(ini,QSettings::IniFormat);

    if(settings->status()!=QSettings::NoError)
        return true;

    qDebug("Storing settings to %s\n",settings->fileName().toStdString().c_str());


    settings->beginWriteArray("annotations");
     for (int i = 0; i < Annotations.size(); i++)
     {
         settings->setArrayIndex(i);
         settings->setValue("annotation", Annotations.at(i).annotation);
         settings->setValue("timestart", Annotations.at(i).timestart);
         settings->setValue("timeend", Annotations.at(i).timeend);
     }
     settings->endArray();



    delete settings;

    return false;
}




void WindowMain::on_uipbHelp_clicked()
{
    QFile file(":/howto.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray filedata = file.readAll();
    HelpWindow dialog(QString(filedata),this);
 #ifdef Q_OS_ANDROID
     dialog.setWindowState(dialog.windowState() | Qt::WindowMaximized);
 #endif
    dialog.exec();
}
int WindowMain::getTimestamp()
{
    // Millisecond timestamp of BlueSense: number of milliseconds since the first day of the month
    int timestamp=0;
    QTime t = QTime::currentTime();
    QDate d = QDate::currentDate();
    timestamp = t.msecsSinceStartOfDay();
    timestamp += (d.day()-1)*24*3600*1000;
    return timestamp;
}
void WindowMain::printAnnotations()
{
    foreach(auto a,Annotations)
    {
        printf("%d: %d - %d\n",a.annotation,a.timestart,a.timeend);
    }
}
QString WindowMain::printAnnotationsInString(bool numericalonly)
{
    QString allstr;
    foreach(auto a,Annotations)
    {
        // Don't print "null" annotations
        if(a.annotation==0)
            continue;
        QString str;
        if(numericalonly)
        {
            str = QString("%1 %2 %3\n").arg(a.annotation).arg(a.timestart).arg(a.timeend);
        }
        else
        {
            str = QString("%1: %2 - ").arg(a.annotation).arg(a.timestart);
            if(a.timeend!=-1)
                str+=QString("%1\n").arg(a.timeend);
            else
                str+="Ongoing\n";
        }
        allstr+=str;
    }
    return allstr;
}
void WindowMain::printAnnotationsInTerminal()
{
    ui->uiwtAnnotation->clear();
    QString str = printAnnotationsInString();
    ui->uiwtAnnotation->addString(str);
}
void WindowMain::printAnnotations(QTextStream &out)
{
    QString str = printAnnotationsInString(true);
    out << str;
}
void WindowMain::addAnnotation(int label)
{
    // If no annotation, create a new annotation, otherwise adjust the end time of the last annotation and create a new one, but only if the label is different than the previous one.
    // We log "null" annotations as any other annotation. Null annotations however are not printed.
    if(Annotations.size()==0 || (Annotations.size() && Annotations.last().annotation!=label))
    {
        if(Annotations.size())
        {
            Annotations.last().timeend = getTimestamp();
        }

        ANNOTATION a;
        a.annotation = label;
        a.timestart = getTimestamp();
        a.timeend = -1;
        Annotations.append(a);
    }
    printAnnotations();
    printAnnotationsInTerminal();


}
void WindowMain::on_uirbAnn1_clicked()
{
    addAnnotation(1);
}

void WindowMain::on_uirbAnn2_clicked()
{
    addAnnotation(2);
}

void WindowMain::on_uirbAnn3_clicked()
{
    addAnnotation(3);
}

void WindowMain::on_uirbAnn4_clicked()
{
    addAnnotation(4);
}

void WindowMain::on_uirbAnn5_clicked()
{
    addAnnotation(5);
}

void WindowMain::on_uirbAnn6_clicked()
{
    addAnnotation(6);
}
void WindowMain::on_uirbAnn7_clicked()
{
    addAnnotation(7);
}

void WindowMain::on_uirbAnn8_clicked()
{
    addAnnotation(8);
}

void WindowMain::on_uirbAnn9_clicked()
{
    addAnnotation(9);
}

void WindowMain::on_uirbAnn10_clicked()
{
    addAnnotation(10);
}

void WindowMain::on_uirbAnnNull_clicked()
{
    addAnnotation(0);
}

QString WindowMain::saveDialog(QString title)
{
    QString filter="All (*);; Text (*.txt)";
    QString fileName;

#ifndef Q_OS_ANDROID
    fileName = QFileDialog::getSaveFileName(this,title,QString(),filter);
#else
    QFileDialog *d = new QFileDialog(this,title,QString("/sdcard/"),filter);
    d->setWindowState(this->windowState() | Qt::WindowMaximized | Qt::WindowFullScreen);
    d->setFileMode(QFileDialog::AnyFile);
    d->setAcceptMode(QFileDialog::AcceptSave);
    if(d->exec()==QDialog::Accepted)
    {
        QStringList sl = d->selectedFiles();
        //qDebug("sl size: %d\n",sl.size());
        if(sl.size())
            fileName = sl.at(0);
    }
#endif

    return fileName;
}

void WindowMain::on_uipbAnnotationSave_clicked()
{
    // Terminate ongoing annotations when saving
    addAnnotation(0);
    ui->uirbAnnNull->setChecked(true);

    QString title("Save annotations");
    QString fileName;


    fileName = saveDialog(title);


    if(!fileName.isNull())
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            // Stream writer
            QTextStream out(&file);

            printAnnotations(out);

            file.close();
        }
        else
        {
            QMessageBox::critical(this,title, "Cannot write to file");
        }
    }


}

void WindowMain::on_uipbAnnotationClear_clicked()
{
    if(QMessageBox::critical(this, "Clear", "Confirm clear annotations",QMessageBox::Ok,QMessageBox::Cancel,QMessageBox::NoButton)==QMessageBox::Ok)
    {
        ui->uirbAnnNull->setChecked(true);
        Annotations.clear();
        printAnnotationsInTerminal();
    }
}




void WindowMain::on_uipbAnnotationCopy_clicked()
{
    // Copy annotations to clipboard
    QString str = printAnnotationsInString(true);
    QApplication::clipboard()->setText(str);
}

/*void WindowMain::on_pushButton_clicked()
{
#ifdef Q_OS_ANDROID
    AndroidFileDialog *fileDialog = new AndroidFileDialog();
    connect(fileDialog, SIGNAL(existingFileNameReady(QString)), this, SLOT(openFileNameReady(QString)));
    bool success = fileDialog->provideExistingFileName();
    if (!success) {
        qDebug() << "Problem with JNI or sth like that...";
        disconnect(fileDialog, SIGNAL(existingFileNameReady(QString)), this, SLOT(openFileNameReady(QString)));
        //or just delete fileDialog instead of disconnect
    }
#endif
}
void WindowMain::openFileNameReady(QString fileName)
{
    if (!fileName.isNull()) {
        qDebug() << "FileName: " << fileName;
    } else {
        qDebug() << "User did not choose file";
    }
}*/
