#include "csvfile.h"
#include <stdexcept>
#include <QFileInfo>
#include <QDir>

namespace csvquery {
	
    CSVFile::CSVFile(const QString& file_path, QIODeviceBase::OpenMode mode)
        : file_(std::make_unique<QFile>(file_path)),
        mapped_data_(nullptr),
        file_size_(0),
        pos_(0),
        file_name{ file_path }
    {
        QIODeviceBase::OpenMode flags;
        if (mode == QIODevice::ReadOnly) {
            flags = mode | QIODevice::Text;
        }
        else if (mode == QIODevice::WriteOnly) {
            flags = mode | QIODevice::Text | QIODevice::Truncate;

            QFileInfo fileInfo(file_path);
            QDir dir;

            // Extract directory part of the path (e.g., from "/home/user/data/file.txt" -> "/home/user/data")
            QString dirPath = fileInfo.path();

            // Create all missing directories in the path
            if (!dir.exists(dirPath)) {
                dir.mkpath(dirPath); // mkpath() creates all parent directories as needed
            }
        }

        if (!file_->open(flags)) {
            throw std::logic_error("Failed to open file: " + file_path.toStdString());
        }

        if (mode == QIODevice::ReadOnly) {
            file_size_ = file_->size();
            mapped_data_ = reinterpret_cast<char*>(file_->map(0, file_size_));
            if (!mapped_data_) {
                file_->close();
                throw std::logic_error("Failed to create memory map for file: " + file_path.toStdString());
            }
        }
        else {
			text_stream_ = std::make_unique<QTextStream>(file_.get());
        }
    }

    CSVFile::~CSVFile() {
        if (mapped_data_) {
            file_->unmap(reinterpret_cast<uchar*>(mapped_data_));
        }
    }

    QStringList CSVFile::readLine() {
        QStringList fields;
        if (pos_ >= file_size_) {
            return fields;
        }

        // Preallocate to reduce dynamic resizing (estimate based on typical CSV)
        fields.reserve(16);

        const char* start = mapped_data_ + pos_;
        const char* end = mapped_data_ + file_size_;
        const char* field_start = start;
        bool in_quotes = false;
        QString field;

        while (pos_ < file_size_) {
            char ch = mapped_data_[pos_];

            if (ch == '"') {
                in_quotes = !in_quotes;
                pos_++;
                continue;
            }

            if (!in_quotes && ch == ',') {
                // Extract field from field_start to current pos (excluding comma)
                fields.append(QString::fromUtf8(field_start, pos_ - (field_start - mapped_data_)));
                pos_++;
                field_start = mapped_data_ + pos_;
                continue;
            }

            if (!in_quotes && ch == '\n') {
                // End of line
                fields.append(QString::fromUtf8(field_start, pos_ - (field_start - mapped_data_)));
                pos_++;
                field_start = mapped_data_ + pos_;
                break;
            }

            pos_++;
        }

        // Handle last field if file ends without newline
        if (pos_ >= file_size_ && field_start < end) {
            fields.append(QString::fromUtf8(field_start, pos_ - (field_start - mapped_data_)));
        }

        return fields;
    }

    QString CSVFile::read_string() {
        if (pos_ >= file_size_) {
            return QString();
        }

        const char* start = mapped_data_ + pos_;
        const char* end = mapped_data_ + file_size_;
        bool in_quotes = false;

        while (pos_ < file_size_) {
            char ch = mapped_data_[pos_];
            if (ch == '"') {
                in_quotes = !in_quotes;
                pos_++;
                if (!in_quotes) {
                    // Return the string including the quotes
                    return QString::fromUtf8(start, pos_ - (start - mapped_data_));
                }
            }
            pos_++;
        }

        // Return whatever is left if no closing quote
        return QString::fromUtf8(start, pos_ - (start - mapped_data_));
    }

    QStringList CSVFile::readRow() {
        return readLine();
    }

    void CSVFile::writeLine(const QString& text) {
        // For writing, we can use a QTextStream for simplicity
        //QTextStream out(file_.get());
        //file_->write
        //out << text << "\n";

        (*text_stream_) << text <<"\n";

        //qDebug() << text << " written.";
    }

    void CSVFile::write(const QString& text) {
        //QTextStream out(file_.get());
        (*text_stream_) << text;
    }

}