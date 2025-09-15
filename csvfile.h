#ifndef CSVFILE_H
#define CSVFILE_H

#include <QTextStream>
#include <QBuffer>
#include <QFile>

class CSVFile : QFile
{
    QBuffer buffer;
    QTextStream stream;

public:
    CSVFile(const QString& file_path, QIODeviceBase::OpenMode m = QIODevice::ReadOnly);


    QStringList readRow();

    void writeLine(const QString& text);

};

#endif // CSVFILE_H
