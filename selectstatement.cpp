#include "selectstatement.h"

SelectStatement::SelectStatement(const QList<QString>& in_files, const QString& out_file)
    :
    out_file(out_file)
    ,csv_files(in_files)
{}


Result SelectStatement::eval()
{
    //open files
    //iterate over files
    //pass row to conditionalexpression
    //if true then pass row to columnexpression
    //save result to result object or out_file
    //return
    Result result;
    return result;
}
