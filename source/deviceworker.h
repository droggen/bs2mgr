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

#ifndef DEVICEWORKER_H
#define DEVICEWORKER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "device.h"
#include "iodev.h"
#include "dlinereader.h"


typedef enum {
    DEVICE_ACTION_SYNCDATETIME,DEVICE_ACTION_CUSTOM,DEVICE_ACTION_GETTIME,DEVICE_ACTION_IDENTIFY
} DEVICE_ACTION;

typedef enum {
    DEVICE_ACTION_RESULT_START,DEVICE_ACTION_RESULT_DONE_SUCCESS,DEVICE_ACTION_RESULT_DONE_ERROR,DEVICE_ACTION_RESULT_DONE_UNDEFINED,DEVICE_ACTION_RESULT_CONNECTING,DEVICE_ACTION_RESULT_CONNECTED,DEVICE_ACTION_RESULT_DISCONNECTED
} DEVICE_ACTION_RESULT;

class Worker : public QThread
{
    Q_OBJECT

public:
    Worker();
    ~Worker();

    void init(const DDEVICE &d, const DEVICE_ACTION &action, const QString &action_param, int connect_timeout=0, int action_timeout=0);
    void run();
    DEVICE_ACTION_RESULT getReturnStatus();
    QString getReturnStatusParam();

private:
    void delay(int msec);

    ConnectionData conn;
    IoDevice iodev;
    DDEVICE device;
    DEVICE_ACTION action;
    QString action_param;
    int action_timeout;
    int connect_timeout;
    DLineReader linereader;

    DEVICE_ACTION_RESULT result;
    QString result_param;

    std::vector<std::string> lastLines;     // Buffer of a few last received lines
    int numCommands;                        // Number of commands we await replies (e.g. when semicolon-separated commands are sent)

    QTimer *timer;

    int getNumCommands(const QString &action_param);

    const int timeoutEnd;

private slots:
    void iodevread(QByteArray ba);
    void ioconnected();
    void iodisconnected();
    void ioerror(QString str);
    void doWorkConnect();
    void doWorkDisonnect();
    void doWorkTimeoutResponse();
    void doWorkTimeoutConnect();

    void doWorkSendCommand();

public slots:


signals:
    //void signalDone(DEVICE_ACTION_RESULT result,const QString &str);
    void signalStatus(DEVICE_ACTION_RESULT result,const QByteArray &ba);
    void signalPrintToTerminal(const QString &str);
};

class Controller : public QObject
{
    Q_OBJECT
    //QThread workerThread;
    Worker *worker;
public:
    Controller();
    ~Controller();

    void start(const DDEVICE &d, const DEVICE_ACTION &action, const QString &action_param, int connect_timeout=0, int action_timeout=0);
    void stop();

public slots:
    void handleStatus(DEVICE_ACTION_RESULT result, const QByteArray &ba);
    void handlePrintToTerminal(const QString &str);
    void threadFinished();
signals:
    void signalStatus(DEVICE_ACTION_RESULT result,const QByteArray &ba);
    void signalPrintToTerminal(const QString &str);
};





#endif // DEVICEWORKER_H
