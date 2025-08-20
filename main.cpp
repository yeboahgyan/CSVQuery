#include <QCoreApplication>
#include <QHash>
#include <QList>
#include <QString>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.

    QList<QString> keywords = {"import", "select", "into", "where", "and", "or", "from", "update", "delete"};


    QString source_file;
    QTextStream source_stream(&source_file);

    unsigned int line_number = 0;

    while(!source_stream.atEnd()){

        QString line = source_stream.readLine().toLower();
        line = line.trimmed();

        if(line.isEmpty()){
            continue;
        }

        line_number = line_number + 1;

        QStringList strings = line.split(" ", Qt::SkipEmptyParts);
        QString first_token = strings.first();

        if(first_token == "import"){

        }
        else if(first_token == "select"){

        }
        else if(first_token == "update"){

        }
        else if(first_token == "delete"){

        }
        else{
            // is stoken a keyword
            if(keywords.contains(first_token)){
                //error name cannot be a keyword
            }
        }

    }



    return a.exec();
}
