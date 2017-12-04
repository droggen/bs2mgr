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
} DEVICE_TYPE;

typedef struct
{
    DEVICE_TYPE type;

    QString port;
    QString mac;

    QString name;
    bool ok;
} DEVICE;

Q_DECLARE_METATYPE(DEVICE)

bool operator < (const DEVICE &d1, const DEVICE &d2);
bool operator == (const DEVICE &d1, const DEVICE &d2);

QList<DEVICE> uniqueDevices(QList<DEVICE> din);
void printDevices(const QList<DEVICE> Devices);

QString Device2Str(const DEVICE &d);
QString Device2StrShort(const DEVICE &d);


#endif // DEVICE_H
