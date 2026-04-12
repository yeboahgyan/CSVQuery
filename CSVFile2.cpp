#include "CSVFile2.h"
#include <QFile>
#include <QFileInfo>
#include <stdexcept>
//#include <csv-parser/include/csv.hpp>


CSVFile2::CSVFile2(const std::string& file_path)
	//:f{ std::make_shared<csv::CSVReader>(file_path) }
	: file_name{QString::fromStdString(file_path)}
{
	//f = std::make_shared<csv::CSVReader>(file_path);
	//row.reserve(20);

	//qDebug() << "file path: " << file_path;

	if (!QFile::exists(QString::fromStdString(file_path))) {
		throw std::logic_error(file_path +" does not exist!");
	}

	QFile file(QString::fromStdString(file_path));
	if (!file.open(QIODevice::ReadOnly)) {
		throw std::logic_error(file_path + " could not be opened!");
	}

	csv::CSVFormat format;
	format.no_header();
	f = std::make_shared<csv::CSVReader>(file_path, format);
}

bool CSVFile2::readRow(csv::CSVRow& row)
{
	//csv::CSVRow row;
	//QStringList r;
	//r.reserve(30);
	//std::vector<csv::CSVRow> v_row(10);

	return f->read_row(row);

	//if (f->read_row(row)) {

		/*
		for (csv::CSVField& field : row) {

			
			try {
				r.append(QString::fromStdString(field.get<std::string>()));
				v_row.emplace_back(field.get<std::string>());
				
			}
			catch (const std::exception& ) {
				// Handle invalid field (e.g., append empty string or log error)
				r.append("");
				v_row.emplace_back("");
			}
			
		}
		*/
		//qDebug() << "..\n";
	//}
	//else {
	//	qDebug() << ".\n";
	//}
	
	//return row; // Returns empty list if no row was read
}