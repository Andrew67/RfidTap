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

#define DEFAULT_TCP     4567
#ifdef Q_OS_WIN32
#define DEFAULT_TTY     "COM11"
#else
#define DEFAULT_TTY     "/dev/ttyUSB0"
#endif

#include <QCoreApplication>
#include <QStringList>
#include "connectionhandler.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <qextserialport/qextserialport.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Determine serial port to use
    QString portName(DEFAULT_TTY);
    if (2 <= a.arguments().size()) {
        portName = a.arguments().at(1);
    }

    // Determine TCP port to bind to
    quint16 port = DEFAULT_TCP;
    if (3 <= a.arguments().size()) {
        bool ok;
        quint16 possiblePort = a.arguments().at(2).toUInt(&ok);
        if (ok) {
            port = possiblePort;
        }
    }


    // Connect to serial port (RFID USB adapter settings)
    PortSettings settings;
    settings.BaudRate = BAUD9600;
    settings.DataBits = DATA_8;
    settings.FlowControl = FLOW_OFF;
    settings.Parity = PAR_NONE;
    settings.StopBits = STOP_1;
    settings.Timeout_Millisec = 50;
    QextSerialPort serial(portName, settings, QextSerialPort::EventDriven);
    if (!serial.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not connect to serial port" << portName;
        return -1;
    }

    // Bind to port and start accepting connections
    QTcpServer server;
    ConnectionHandler handler(&serial, &server);
    if (!server.listen(QHostAddress("127.0.0.1"), port)) {
        qDebug().nospace() << "Error binding to 127.0.0.1:" << port;
        return -1;
    }

    qDebug().nospace() << "Ready (TTY " << portName << "; port " << port << ")";
    
    return a.exec();
}
