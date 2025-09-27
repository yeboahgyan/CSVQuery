#ifndef CSVFILE_H
#define CSVFILE_H

#include <QTextStream>
#include <QBuffer>
#include <QFile>
#include "Types.h"

class CSVFile
{
    std::shared_ptr<QBuffer> buffer;
    std::shared_ptr<QTextStream> stream;
    Token token;
    std::shared_ptr<QFile> f;

public:
    CSVFile(const QString& file_path, QIODeviceBase::OpenMode m = QIODevice::ReadOnly);

    //CSVFile(const CSVFile&) = delete; // Delete copy constructor
    //CSVFile& operator=(const CSVFile&) = delete; // Delete copy assignment operator



    QStringList readRow();

    void writeLine(const QString& text);

    void write(const QString& text);

    bool end_of_file(){
        return stream->atEnd();
    }

    void set_token(Token t){token =t;}
    Token get_token() const {return token;}

    qint64 get_pos(){
        return stream->pos();
    }

    void seek_to(qint64 pos){
        stream->seek(pos);
    }

    QString read_string();

    QStringList readLine();

};

#endif // CSVFILE_H
