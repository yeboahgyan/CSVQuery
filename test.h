// CSVQuery - An SQL-like query language for CSV files
// Copyright (c) 2025-2026 Kwame Yeboah-Gyan
// Distributed under the MIT License.

#pragma once
#include "Types.h"
#include "tokenizer.h"
#include <QHash>
#include <QList>
#include <QString>
#include <QTextStream>
#include <QMap>
#include <QDebug>
#include <algorithm> // For std::transform
#include <cctype>    // For std::toupper
#include <regex>

#include "tokenizer.h"
#include "functions.h"
#include "Expression.h"
#include "conditionalexpression.h"
#include "importstatement.h"
#include "assignstatement.h"
#include "csvfile.h"
#include "selectstatement.h"
#include "updatestatement.h"
#include "types.h"
#include "parser.h"
#include <QTextStream>
//#include "pretty.h"

#include <tabulate/table.hpp>
//using namespace tabulate;

//#include "tabulate.h"

#include <QFileInfo>


//import pretty;
#include <replxx.hxx>


void test_main();

void test_tokenizer();

void test_expression(QString& source, const QMap<QString, QStringList>& data_rows);

void test_conditional_expression(QString& source, const QMap<QString, QStringList>& data_rows);

void test_import(QString& source);

void test_import_and_assigment();

void test_csv_parser(const QString& csv);

QList<csvquery::Token> read_statement(csvquery::Tokenizer& t);

void test_select_statement();

//void print(const std::optional<QList<QStringList>>& res);

void print_with_margin(tabulate::Table& table, int margin = 4);


