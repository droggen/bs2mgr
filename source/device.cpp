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
#include <QList>
#include "device.h"


bool operator < (const DDEVICE &d1, const DDEVICE &d2)
{
    if(d1.type == DEVICE_BT && d2.type == DEVICE_SER)
        return 1;
    if(d1.type == DEVICE_SER && d2.type == DEVICE_BT)
        return 0;

    if(d1.type == DEVICE_SER && d2.type == DEVICE_SER)
    {
        return d1.port<d2.port;
    }
    return d1.mac<d2.mac;

}

bool operator == (const DDEVICE &d1, const DDEVICE &d2)
{
    if(d1.type != d2.type)
        return false;

    if(d1.type==DEVICE_SER)
        return d1.port==d2.port;
    return d1.mac==d2.mac;
}


QList<DDEVICE> uniqueDevices(QList<DDEVICE> din)
{
    QList<DDEVICE> dout;
    foreach(auto d,din)
    {
        if(!dout.contains(d))
            dout.append(d);
    }
    return dout;
}

void printDevices(const QList<DDEVICE> Devices)
{
    printf("Devices: %d\n",Devices.size());
    foreach (auto d, Devices)
    {
        switch(d.type)
        {
            case DEVICE_SER:
                printf("\tser: %s\n",d.port.toStdString().c_str());
                break;
            case DEVICE_BT:
                printf("\tbt: %s\n",d.mac.toStdString().c_str());
                break;
            default:
                printf("\tunknown\n");
        }
    }
}

QString Device2Str(const DDEVICE &d)
{
    QString str;
    switch(d.type)
    {
        case DEVICE_SER:
            str="SER:";
            str+=d.port;
            break;
        case DEVICE_BT:
            str="BT:";
            str += d.mac;
            break;
        default:
            str="UNK";
    }
    if(!d.name.isEmpty())
        str += " ("+d.name+")";
    return str;
}
QString Device2StrShort(const DDEVICE &d)
{
    QString str;
    switch(d.type)
    {
        case DEVICE_SER:
            str=d.port;
            break;
        case DEVICE_BT:
            str=d.mac;
            break;
        default:
            str="UNK";
    }
    if(!d.name.isEmpty())
        str += "\n("+d.name+")";
    return str;
}

