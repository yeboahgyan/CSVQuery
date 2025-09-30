#include "csvfile.h"
#include <stdexcept>

namespace csvquery {

    CSVFile::CSVFile(const QString& file_path, QIODeviceBase::OpenMode mode)
        :
        stream{ std::make_shared<QTextStream>() },
        f{ std::make_shared<QFile>(file_path) }
    {
        QIODeviceBase::OpenMode flags;
        if (mode == QIODevice::ReadOnly) {
            flags = mode | QIODevice::Text;
        }
        else if (mode == QIODevice::WriteOnly) {
            flags = (mode | QIODevice::Text | QIODevice::Truncate);
        }

        if (!f->open(flags)) {
            throw std::logic_error("Failed to open file: " + file_path.toStdString());
        }

        //stream = std::make_shared<QTextStream>();

        if (mode == QIODevice::ReadOnly) {
            uchar* mappedData = f->map(0, f->size());
            if (!mappedData) {
                // Handle error
                f->close();
                throw std::logic_error("Failed to create memory map for file: " + file_path.toStdString());
            }

            buffer = std::make_shared<QBuffer>();

            buffer->setData(reinterpret_cast<const char*>(mappedData), f->size());
            buffer->open(QIODevice::ReadOnly);
            stream->setDevice(buffer.get());
        }
        else {
            stream->setDevice(f.get());
        }

    }

    QString CSVFile::read_string()
    {
        QString str;
        QChar ch;

        while (!stream->atEnd()) {
            (*stream) >> ch;

            if (ch == '"') {
                str.append(ch);
                return str;
            }

            str.append(ch);
        }

        return str;
    }

    QStringList CSVFile::readLine()
    {
        QList<QString> fields;

        QChar ch;
        QString field;

        while (!stream->atEnd()) {
            (*stream) >> ch;

            if (ch == '"') {
                field.append(ch);
                field.append(read_string());
                //field = "";
                continue;
            }

            if (ch == ',') {
                fields.append(field.trimmed());
                field = "";
                continue;
            }

            if (ch == '\n') {
                fields.append(field.trimmed());
                field = "";
                break;
            }

            field.append(ch);
        }

        if (stream->atEnd()) {
            field = field.trimmed();
            if (field != "") {
                fields.append(field);
            }
        }

        return fields;
    }

    QStringList CSVFile::readRow()
    {
        //QString line = stream->readLine().trimmed();
        //return line.split(',');
        return this->readLine();
    }

    void CSVFile::writeLine(const QString& text)
    {
        (*stream) << text << "\n";
    }

    void CSVFile::write(const QString& text)
    {
        (*stream) << text;
    }

}