#include <QCoreApplication>
#include <QByteArray>
#include <QStringList>
#include <QTextStream>

inline QTextStream& qStdout()
{
    static QTextStream r(stdout);
    return r;
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    if (args.size() != 3)
    {
        qStdout() << "Usage: " << args[0] << " [input] [output]";
    }

    return 0;
}
