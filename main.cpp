/*******************************************************************************
 * Copyright (c) 2013 Andrés Cordero
 *
 * Web: https://github.com/Andrew67/RfidTap
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#include <QCoreApplication>
#include "connectionhandler.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <qextserialport/qextserialport.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Determine serial port to use
    QString *portName;
    if (argc >= 2) {
        portName = new QString(argv[1]);
    }
    else {
#ifdef Q_OS_LINUX
        portName = new QString("/dev/ttyUSB0");
#endif
#ifdef Q_OS_WIN32
        portName = new QString("COM11");
#endif
    }

    // Connect to serial port (RFID USB adapter settings)
    PortSettings settings;
    settings.BaudRate = BAUD9600;
    settings.DataBits = DATA_8;
    settings.FlowControl = FLOW_OFF;
    settings.Parity = PAR_NONE;
    settings.StopBits = STOP_1;
    settings.Timeout_Millisec = 50;
    QextSerialPort serial(*portName, settings, QextSerialPort::EventDriven);
    if (!serial.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not connect to serial port" << *portName;
        return -1;
    }

    // Bind to port and start accepting connections
    QTcpServer server;
    ConnectionHandler handler(&serial, &server);
    if (!server.listen(QHostAddress("127.0.0.1"), 4567)) {
        qDebug() << "Error binding to 127.0.0.1:4567";
        return -1;
    }
    
    return a.exec();
}
