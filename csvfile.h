// CSVQuery - An SQL-like query language for CSV files
// Copyright (c) 2025-2026 Kwame Yeboah-Gyan
// Distributed under the MIT License.

#ifndef CSVFILE_H
#define CSVFILE_H

#include <QTextStream>
#include <QBuffer>
#include <QFile>
#include "Types.h"
#include <fstream>
//#include <csv-parser/include/csv.hpp>
#include <vincentlaucsb-csv-parser/csv.hpp> // uses single header code in exterl_lib folder in the project directory; this path is set under Additional Include Directories in the project settings

namespace csvquery {

    class CSVFile {
        std::shared_ptr<QFile> file_;
        char* mapped_data_;
        qint64 file_size_;
        qint64 pos_;
        Token token_;

        std::unique_ptr<QTextStream> text_stream_ = nullptr;
        QString file_name;

        std::unique_ptr<std::ofstream> file_stream_;
        std::unique_ptr < csv::CSVWriter<std::ofstream>> csv_writer_ = nullptr;

    public:
        CSVFile(const QString& file_path, QIODeviceBase::OpenMode mode = QIODevice::ReadOnly);
        ~CSVFile();

        QStringList readRow();
        void writeLine(const QString& text);
        void writeLine(const QStringList& row);
        void write(const QString& text);

        bool end_of_file() const {
            return pos_ >= file_size_;
        }

        void set_token(Token t) { token_ = t; }
        Token get_token() const { return token_; }

        qint64 get_pos() const {
            return pos_;
        }

        void seek_to(qint64 pos) {
            pos_ = pos;
        }

        QString get_file_name() const {
            return file_name;
        }

        QString read_string(); // Kept for compatibility, but optimized below
        QStringList readLine(); // Optimized parsing method
    };

}

#endif // CSVFILE_H
