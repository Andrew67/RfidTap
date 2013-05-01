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

#include "connectionhandler.h"
#include <QDebug>

ConnectionHandler::ConnectionHandler(QextSerialPort *serial, QTcpServer *server, QObject *parent) :
    QObject(parent)
{
    this->serial = serial;
    this->server = server;
    connect(server, &QTcpServer::newConnection,
            this, &ConnectionHandler::handleConnection);
    connect(serial, &QextSerialPort::readyRead,
            this, &ConnectionHandler::broadcastData);
}

void ConnectionHandler::handleConnection() {
    qDebug() << "Accepting connection...";
    QTcpSocket *socket = server->nextPendingConnection();

    // Write HTTP header
    socket->write("HTTP/1.1 200 OK\n");
    socket->write("Content-Type: text/plain;\n");
    socket->write("Content-Length: 12\n");
    socket->write("Server: RfidTap/1.0\n");
    socket->write("X-Powered-By: Qt/");
    socket->write(QT_VERSION_STR);
    socket->write("\nAccess-Control-Allow-Origin: *\n\n");
    socket->flush();

    // Queue socket for awaiting RFID
    sockets.enqueue(socket);
}

void ConnectionHandler::broadcastData() {
    // Wait for all RFID bytes
    if (serial->bytesAvailable() < 16) {
        return;
    }
    // Send received RFID to all queued sockets and close them
    QByteArray rfid = QString(serial->readAll()).mid(1, 12).toUtf8();
    qDebug() << "Got RFID:" << rfid;

    while (!sockets.empty()) {
        QTcpSocket *socket = sockets.dequeue();
        socket->write(rfid);
        socket->flush();
        socket->close();
        socket->deleteLater();
    }
}
