QT = core

CONFIG += c++20 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        #columnexpression.cpp \
        assignstatement.cpp \
        conditionalexpression.cpp \
        csvfile.cpp \
        expression.cpp \
        functions.cpp \
        importstatement.cpp \
        main.cpp \
        parser.cpp \
        selectstatement.cpp \
        term.cpp \
        tokenizer.cpp \
        types.cpp \
        updatestatement.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Types.h \
    assignstatement.h \
    columnexpression.h \
    #columnterm.h \
    conditionalexpression.h \
    #conditionalterm.h \
    csvfile.h \
    expression.h \
    functions.h \
    parser.h \
    importstatement.h \
    selectstatement.h \
    term.h \
    tokenizer.h \
    updatestatement.h
