#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>

class Process : public QObject
{
    Q_OBJECT
public:
    explicit Process(QObject *parent = nullptr);

signals:
    void output(QString result, int code);

public slots:
    void start(QString command, QStringList);
    void stop();

private slots:
    void errorOccured(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();
    void stateChanged(QProcess::ProcessState newState);

private:
    QProcess m_process;
    QString m_command;
    QString getProcess();
    //void startCommand(QString command);

};

#endif // PROCESS_H
