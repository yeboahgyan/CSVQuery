#include "csvfile.h"
#include <stdexcept>

CSVFile::CSVFile(const QString& file_path, QIODeviceBase::OpenMode mode)
: QFile{file_path}
{
    QIODeviceBase::OpenMode flags;
    if(mode == QIODevice::ReadOnly){
        flags = mode | QIODevice::Text;
    }
    else if(mode == QIODevice::WriteOnly){
        flags = (mode | QIODevice::Text | QIODevice::Truncate);
    }

    if (!open(flags)) {
        throw std::logic_error("Failed to open file: "+file_path.toStdString());
    }

    if(mode == QIODevice::ReadOnly){
        uchar *mappedData = map(0, size());
        if (!mappedData) {
            // Handle error
            close();
            throw std::logic_error("Failed to create memory map for file: "+file_path.toStdString());
        }

        buffer.setData(reinterpret_cast<const char*>(mappedData), size());
        buffer.open(QIODevice::ReadOnly);
        stream.setDevice(&buffer);
    }
    else{
        stream.setDevice(this);
    }

}

QStringList CSVFile::readRow()
{
    QString line = stream.readLine();
    return line.split(',');
}

void CSVFile::writeLine(const QString& text)
{
    stream<< text <<"\n";
}
