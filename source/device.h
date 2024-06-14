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


#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QMetaType>

typedef enum {
    DEVICE_BT,DEVICE_SER
} DDEVICE_TYPE;

typedef struct
{
    DDEVICE_TYPE type;

    QString port;
    QString mac;

    QString name;
    bool ok;
} DDEVICE;

Q_DECLARE_METATYPE(DDEVICE)

bool operator < (const DDEVICE &d1, const DDEVICE &d2);
bool operator == (const DDEVICE &d1, const DDEVICE &d2);

QList<DDEVICE> uniqueDevices(QList<DDEVICE> din);
void printDevices(const QList<DDEVICE> Devices);

QString Device2Str(const DDEVICE &d);
QString Device2StrShort(const DDEVICE &d);


#endif // DEVICE_H
