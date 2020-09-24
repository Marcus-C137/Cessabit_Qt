#include <QDebug>
#include "process.h"

Process::Process(QObject *parent) : QObject(parent)
{
    connect(&m_process, &QProcess::errorOccurred, this, &Process::errorOccured);
    connect(&m_process, &QProcess::readyReadStandardError, this, &Process::readyReadStandardError);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &Process::readyReadStandardOutput);
    connect(&m_process, &QProcess::started, this, &Process::started);
    connect(&m_process, &QProcess::stateChanged, this, &Process::stateChanged);

    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Process::finished);

}

void Process::start(QString command, QStringList args)
{
    m_process.start(command, args);
}

void Process::stop()
{
    //qInfo() << Q_FUNC_INFO << " stopping process";
    m_process.close();
}

void Process::errorOccured(QProcess::ProcessError error)
{
    qInfo() << Q_FUNC_INFO << error;
    emit output("Error",1);
}

void Process::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    //qInfo() << Q_FUNC_INFO;
    Q_UNUSED(exitStatus);
    Q_UNUSED(exitCode);
    QByteArray data = m_process.readAllStandardOutput();
    emit output(QString(data.trimmed()),0);
    this->stop();
    //emit output("Complete", exitCode);

}

void Process::readyReadStandardError()
{
    //qInfo()<<Q_FUNC_INFO;
    QByteArray data = m_process.readAllStandardError();
    QString message = "Standard Error: ";
    message.append(m_process.readAllStandardError());
    emit output(message,2);
}

void Process::readyReadStandardOutput()
{
    //qInfo()<<Q_FUNC_INFO;
//    QByteArray data = m_process.readAllStandardOutput();
//    emit output(QString(data.trimmed()),0);
}

void Process::started()
{
    //qInfo() << Q_FUNC_INFO;

}

void Process::stateChanged(QProcess::ProcessState newState)
{
    //qInfo() << Q_FUNC_INFO;
    switch (newState){
    case QProcess::NotRunning:
        //emit output("Not Running");
        break;
    case QProcess::Starting:
        //emit output("Starting...");
        break;
    case QProcess::Running:
        //emit output("Running");
        //startCommand(m_command);
        break;

    }

}


//void Process::startCommand(QString command)
//{
//     QByteArray ba_command;
//     ba_command.append(command);
//     ba_command.append("\n");
//     m_process.write(ba_command);
//}
