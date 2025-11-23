#pragma once

#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace QtLogging
{
    void fileLogMessageHandler(QtMsgType type,
                            const QMessageLogContext &context,
                            const QString &msg)
    {
        Q_UNUSED(context);

        static QFile file(LOG_FILENAME);
        static bool initialized = false;

        if (!initialized) {
            file.open(QIODevice::Append | QIODevice::Text);
            initialized = true;
        }

        if (!file.isOpen())
            return;

        QTextStream out(&file);

        const char* typeStr = "";
        switch (type) {
        case QtDebugMsg:    typeStr = "DEBUG"; break;
        case QtInfoMsg:     typeStr = "INFO "; break;
        case QtWarningMsg:  typeStr = "WARN "; break;
        case QtCriticalMsg: typeStr = "ERROR"; break;
        case QtFatalMsg:    typeStr = "FATAL"; break;
        }

        out << QDateTime::currentDateTime().toString(Qt::ISODate)
            << " [" << typeStr << "] "
            << msg << "\n";

        out.flush();

        if (type == QtFatalMsg) {
            abort();
        }
    }
}