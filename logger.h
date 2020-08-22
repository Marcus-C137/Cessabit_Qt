#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QDateTime>


class logger : public QObject
{
    Q_OBJECT
public:
    explicit logger(QObject *parent = nullptr);

    static bool logging;
    static QString filename;
    static void attach();
    static void lhandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

signals:

public slots:

};


#endif // LOGGER_H
