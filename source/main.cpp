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
#include "windowmain.h"
#include <QApplication>

#include "device.h"

int main(int argc, char *argv[])
{
    // Android
    qputenv("QT_USE_ANDROID_NATIVE_STYLE", "1");

    //qRegisterMetaType
    qRegisterMetaType<DDEVICE>("DDEVICE");
    qRegisterMetaType<DEVICE_ACTION>("DEVICE_ACTION");
    qRegisterMetaType<DEVICE_ACTION_RESULT>("DEVICE_ACTION_RESULT");

    QApplication a(argc, argv);
    WindowMain w;
    w.show();

    return a.exec();
}
