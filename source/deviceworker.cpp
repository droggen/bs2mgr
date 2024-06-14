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

#include <QTimer>
#include <QTime>

#include "deviceworker.h"

/*================================================================================
==================================================================================
WORKER   -   WORKER   -   WORKER   -   WORKER   -   WORKER   -   WORKER   -   WORK
==================================================================================
=================================================================================*/
/*
The Worker derives from QThread and allows to handle connection and communication
with a peripheral in a non-GUI blocking manner.

The worker communicates to the controller (and ultimately to the main application)
by emitting two signals:
- signalStatus: used to indicate initiation of a connection, connection, done with success, done with error, etc
- signalPrintToTerminal: used to send bytes to be printed on the terminal

After instantiation, the worker is started by calling init to pass specific parameters and then start.
*/

Worker::Worker() :
    timeoutEnd(1000)
{
    // Connect the iodevice to our slots
    connect(&iodev,SIGNAL(readyRead(QByteArray)),this,SLOT(iodevread(QByteArray)));
    connect(&iodev,SIGNAL(connected()),this,SLOT(ioconnected()));
    connect(&iodev,SIGNAL(disconnected()),this,SLOT(iodisconnected()));
    connect(&iodev,SIGNAL(error(QString)),this,SLOT(ioerror(QString)));

    // Initialise last lines buffer
    for(int i=0;i<5;i++)
        lastLines.push_back("");


    // Initialise timeout timers
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), SLOT(doWorkTimeoutConnect()));

}
Worker::~Worker()
{
    //printf("Worker destructor\n");
    timer->stop();
    delete timer;
}
void Worker::init(const DDEVICE &d, const DEVICE_ACTION &action, const QString &action_param, int connect_timeout, int action_timeout)
{
    //printf("Worker::init\n");
    // Copy the parameters to local variables; not error checking at this stage.
    this->action=action;
    this->action_param=action_param;
    if(action_timeout==0)
        this->action_timeout=10000;
    else
        this->action_timeout=action_timeout;
    if(connect_timeout==0)
        this->connect_timeout=30000;
    else
        this->connect_timeout=connect_timeout;
    this->device = d;

    // Initialise the connection timer timeout
    qDebug(QString("Timeout: %1").arg(this->connect_timeout).toLatin1());
    timer->setInterval(this->connect_timeout);

}
int Worker::getNumCommands(const QString &action_param)
{
    //printf("getnumcommands: '%s'\n",action_param.toStdString().c_str());
    std::string cstr = action_param.toStdString();
    const char *str = cstr.c_str();
    //printf("commands again '%s'\n",str);

    // Does not process quote modes. Finds number of semicolon delimited commands. Ignores empty commands
    int numcmd=0;           // Number of commands so far
    bool iscmd=false;       // If a non-space character is seen we have a command

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
    printf("\t%d\n",numcmd);
    return numcmd;
}


void Worker::run()
{
    printf("Run\n");
    printf("remainingTime: %d\n",timer->remainingTime());
    // Initialse the DONE return value for undefined at this stage
    result = DEVICE_ACTION_RESULT_DONE_UNDEFINED;

    // Indicate that the device is starting to be processed
    emit signalStatus(DEVICE_ACTION_RESULT_START,QString("Run").toLocal8Bit());

    // Create the connectiondata and check for possible errors.
    switch(device.type)
    {
        case DEVICE_BT:
            conn.type = DevBTConnection;
            conn.ba= QBluetoothAddress(device.mac);
            printf("d.mac: %s\n",device.mac.toStdString().c_str());
            break;
        case DEVICE_SER:
            conn.type = DevSerialConnection;
            conn.port = device.port;
            printf("d.port: %s\n",device.port.toStdString().c_str());
            break;
        default:
            result = DEVICE_ACTION_RESULT_DONE_ERROR;
            return;
    }

    // Use timer to trigger ourselves in an event loop
    // Bluetooth seems to need a wait time between previous close and new opening -> add delay
    if(conn.type==DevBTConnection)
        QTimer::singleShot(3000, this, SLOT(doWorkConnect()));
    else
        QTimer::singleShot(0, this, SLOT(doWorkConnect()));

    //printf("Starting event loop\n");
    // Start event loop
    exec();
    //printf("Event loop dead\n");
}


void Worker::doWorkConnect()
{
    qDebug("Worker::doWorkConnect");
    printf("Worker::doWorkConnect");
    emit signalStatus(DEVICE_ACTION_RESULT_CONNECTING,QString("Connecting").toLocal8Bit());

    // Starts timer for connection timeout, this must be started before open.
    timer->start();
    // Start connection
    iodev.open(conn);



}
void Worker::doWorkDisonnect()
{
    iodev.close();
}

void Worker::delay(int msec)
{
    // Blocking delay
    QTime time;

    time.start();
    while(time.elapsed()<msec);
}

void Worker::ioconnected()
{
    // Connection didn't timeout - stop timer.
    printf("Worker::ioconnected: stopping the connection timeout timer\n");
    timer->stop();

    emit signalStatus(DEVICE_ACTION_RESULT_CONNECTED,QString("Connected").toLocal8Bit());

    QTimer::singleShot(1000, this, SLOT(doWorkSendCommand())); // send the command a bit later
}
void Worker::doWorkSendCommand()
{
    printf("sendCommand\n");

    // Select the actions
    switch(action)
    {
        case DEVICE_ACTION_GETTIME:
            numCommands = 1;
            iodev.send("T\n");
            break;
        case DEVICE_ACTION_IDENTIFY:
            numCommands = 1;
            printf("Sending ?\n");
            iodev.send("?\n");
            break;

        case DEVICE_ACTION_SYNCDATETIME:
        {
            numCommands = 1;
            //emit signalPrintToTerminal(QString("[Sync date/time]\n"));
            //printf("Sync date time\n");

            QTime tnew,torg;
            // Synchronisation mode
            torg = QTime::currentTime();
            // Wait until a new second occurs
            while((tnew=QTime::currentTime()).second()==torg.second());
            QDate date = QDate::currentDate();
            // Build the time string
            char buf[64];
            sprintf(buf,"Z,%02d%02d%02d%02d%02d%02d\n",tnew.hour(),tnew.minute(),tnew.second(),date.day(),date.month(),date.year()-2000);
            //printf("%s\n",buf);
            emit signalPrintToTerminal(QString("["+QString(buf)+"]\n"));
            // Send the sync command
            iodev.send(buf);
        }
            break;
        case DEVICE_ACTION_CUSTOM:
        {
            numCommands = getNumCommands(action_param);
            QString cmd  = action_param;
            iodev.send(cmd.toLocal8Bit());
            break;
        }
        default:
            break;
    }

    printf("Number of commands: %d\n",numCommands);

    printf("timeout: %d\n",action_timeout);

    // Prepare a response timeout, if for some reason the sensor doesn't reply
    QTimer::singleShot(action_timeout, this, SLOT(doWorkTimeoutResponse()));
}

void Worker::iodisconnected()
{
    qDebug("Worker::iodisconnected\n");
    emit signalStatus(DEVICE_ACTION_RESULT_DISCONNECTED,QString("Worker::iodisconnected").toLocal8Bit());
    exit(2);
}

void Worker::ioerror(QString str)
{
    printf("Worker::ioerror: %s\n",str.toStdString().c_str());

    // Flag error
    result = DEVICE_ACTION_RESULT_DONE_ERROR;

    exit(1);
}

void Worker::iodevread(QByteArray ba)
{
#if 0
    // This implementation prints data as soon as received
    // Emit timestamp
    QTime tcur = QTime::currentTime();
    emit signalPrintToTerminal(tcur.toString("hh:mm:ss.zzz")+":    ");
    // Emit received data
    emit signalPrintToTerminal(ba);
#endif
    std::string str;

    linereader.add(std::string(ba.constData()));

    while(linereader.getLine(str))
    {
#if 1
        // This implementation prints data line by line
        QTime tcur = QTime::currentTime();
        emit signalPrintToTerminal(tcur.toString("hh:mm:ss.zzz")+":    ");

        emit signalPrintToTerminal(QString::fromStdString(str)+"\n");
#endif

        // Keep a buffer of a few last lines
        lastLines.push_back(str);
        lastLines.erase(lastLines.begin());


        printf("'%s'\n",str.c_str());



        // Check if CMDOK, CMDERR, CMDINV
        // If received, wait a bit more and initiate closing the session.
        // Normally nothing more should come after CMDxxx, however if the firmware is buggy/being developed it makes sense to
        // empty the buffers before closing the session.
        if(!str.compare("CMDOK"))
        {
            // Check command modes to see if additional processing in case of success
            if(action==DEVICE_ACTION_GETTIME)
            {
                // Time is in the 2nd last line
                std::string timestr = lastLines.at(lastLines.size()-2);
                printf("Get time mode, time is: %s\n",timestr.c_str());

                // Get current time
                QTime torg = QTime::currentTime();
                // Get node time
                QTime nodetime = QTime::fromString(QString::fromStdString(timestr), "hh:mm:ss");

                // Get lead of node compared to system
                int elapsed=nodetime.msecsSinceStartOfDay()-torg.msecsSinceStartOfDay();
                // Display result in terminal
                emit signalPrintToTerminal(QString("[Node leads system by: %1 ms.]\n").arg(elapsed).toLocal8Bit());
            }
            if(action==DEVICE_ACTION_IDENTIFY)
            {
                // Identify is in the 2nd last line
                std::string identity = lastLines.at(lastLines.size()-2);
                // Skip first 11 characters
                identity = identity.substr(11);

                printf("Identity: %s\n",identity.c_str());
                // Display result in terminal
                emit signalPrintToTerminal(QString("[Identity: %1]\n").arg(QString(identity.c_str())).toLocal8Bit());

                result_param = QString(identity.c_str());
            }

            numCommands--;
            printf("Number remaining commands: %d\n",numCommands);
            if(numCommands==0)
            {
                if(result == DEVICE_ACTION_RESULT_DONE_UNDEFINED)
                    result = DEVICE_ACTION_RESULT_DONE_SUCCESS;
                QTimer::singleShot(timeoutEnd, this, SLOT(doWorkDisonnect()));
            }
        }
        // Error message: store error result and initiate disconnect
        if(!str.compare("CMDERR"))
        {
            result = DEVICE_ACTION_RESULT_DONE_ERROR;
            numCommands--;
            printf("Number remaining commands: %d\n",numCommands);
            if(numCommands==0)
            {
                QTimer::singleShot(timeoutEnd, this, SLOT(doWorkDisonnect()));
            }
        }
        // Invalid message: store error result and initiate disconnect
        if(!str.compare("CMDINV"))
        {
            result = DEVICE_ACTION_RESULT_DONE_ERROR;
            numCommands--;
            printf("Number remaining commands: %d\n",numCommands);
            if(numCommands==0)
            {
                QTimer::singleShot(timeoutEnd, this, SLOT(doWorkDisonnect()));
            }
        }

    }
}

void Worker::doWorkTimeoutConnect()
{
    printf("Worker::doWorkTimeoutConnect\n");
    result = DEVICE_ACTION_RESULT_DONE_ERROR;
    iodev.close();
    exit(4);
}

void Worker::doWorkTimeoutResponse()
{
    printf("Worker::doWorkKillerTimeout\n");
    printf("Worker::doWorkKillerTimeout. Is connected: %d\n",iodev.isConnected());

    // Currently: sets return value to error. Initiate device closing. The signal receiving the close notification terminates the thread.
    result = DEVICE_ACTION_RESULT_DONE_ERROR;
    iodev.close();
}

DEVICE_ACTION_RESULT Worker::getReturnStatus()
{
    return result;
}
QString Worker::getReturnStatusParam()
{
    return result_param;
}

/*================================================================================
==================================================================================
CONTROLLER   -   CONTROLLER   -   CONTROLLER   -   CONTROLLER   -   CONTROLLER   -
==================================================================================
=================================================================================*/
/*
The controller is a helper class which starts a worker.
It catches the signals emitted from the worker: signalStatus, signalPrintToTerminal and finished.
*/
Controller::Controller()
{
    worker=0;

}
Controller::~Controller()
{
    stop();
}
void Controller::start(const DDEVICE &d,const DEVICE_ACTION &action,const QString &action_param,int connect_timeout,int action_timeout)
{
    // If an existing worker is there, do nothing
    if(worker)
        return;

    worker = new Worker();
    connect(worker, &Worker::signalStatus, this, &Controller::handleStatus);
    connect(worker, &Worker::signalPrintToTerminal, this, &Controller::handlePrintToTerminal);
    connect(worker, &QThread::finished, this, &Controller::threadFinished);
    worker->init(d,action,action_param,connect_timeout,action_timeout);
    worker->start();

}
void Controller::stop()
{
    if(worker)
    {
        worker->quit();
        worker->wait();
        delete worker;
        worker=0;
    }
}
void Controller::threadFinished()
{
    // Get the return value of the thread, before deleting
    DEVICE_ACTION_RESULT result = worker->getReturnStatus();
    QString result_param = worker->getReturnStatusParam();
    qDebug(QString("Controller::threadFinished. Result: %1. Param: %2").arg(result).arg(result_param).toLatin1());

    // Delete the thread object
    delete worker;
    worker=0;

    // Emit completion signal
    emit signalStatus(result,result_param.toLocal8Bit());

}

void Controller::handleStatus(DEVICE_ACTION_RESULT result, const QByteArray &ba)
{
//    printf("Controller::handleStatus ba: %s\n",ba.constData());
    emit signalStatus(result,ba);
}
void Controller::handlePrintToTerminal(const QString &str)
{
//    printf("Controller::handlePrintToTerminal: %s\n",str.toStdString().c_str());
    emit signalPrintToTerminal(str);
}


