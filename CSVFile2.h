#pragma once

//#include "csv_lib.h"
#include "Types.h"
#include <memory>
#include <QStringList>
//#include <vincentlaucsb-csv-parser/csv.hpp>
#include <csv-parser/include/csv.hpp>



class CSVFile2
{
    csvquery::Token token;
    std::shared_ptr<csv::CSVReader> f;
    //QStringList row;

    QString file_name;

public:
    CSVFile2(const std::string& file_path);

    //CSVFile(const CSVFile&) = delete; // Delete copy constructor
    //CSVFile& operator=(const CSVFile&) = delete; // Delete copy assignment operator



    bool readRow(csv::CSVRow&);

    bool end_of_file() {
        return f->eof();
    }

    QString get_file_name() const {
        return file_name;
    }

    void set_token(csvquery::Token t) { token = t; }
    csvquery::Token get_token() const { return token; }

};

