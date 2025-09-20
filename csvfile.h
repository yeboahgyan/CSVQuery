#ifndef CSVFILE_H
#define CSVFILE_H

#include <QTextStream>
#include <QBuffer>
#include <QFile>
#include "Types.h"

class CSVFile : QFile
{
    QBuffer buffer;
    QTextStream stream;
    Token token;

public:
    CSVFile(const QString& file_path, QIODeviceBase::OpenMode m = QIODevice::ReadOnly);


    QStringList readRow();

    void writeLine(const QString& text);

    void write(const QString& text);

    bool end_of_file(){
        return stream.atEnd();
    }

    void set_token(Token t){token =t;}
    Token get_token() const {return token;}

    void seek_to(qint64 pos){
        stream.seek(pos);
    }

};

#endif // CSVFILE_H
