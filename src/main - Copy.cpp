#include <iostream>
#include <tabulate/table.hpp>
#include <replxx.hxx>
#include "csv.hpp"
#include <boost/lockfree/spsc_queue.hpp>

using namespace tabulate;

int main()
{
    Table table;

    table.add_row({ "Name", "Age", "Country" });
    table.add_row({ "Kwame", "42", "Ghana" });
    table.add_row({ "John", "35", "USA" });

    std::cout << table << '\n';

    csv::CSVReader reader("D:\\software\\test_csv\\customers_102.csv");

    for (auto& row : reader) {
        std::cout << row[0].get<std::string>() << '\n';
    }

    std::unique_ptr<boost::lockfree::spsc_queue<std::vector<csv::CSVRow>, boost::lockfree::capacity<256>>> queue;

    replxx::Replxx rx;

    auto input = rx.input("csvq> ");

    if (input) {
        std::cout << *input << '\n';
    }


    return 0;
}