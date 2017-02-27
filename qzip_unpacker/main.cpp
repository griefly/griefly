#include <QCoreApplication>
#include <QByteArray>
#include <QStringList>
#include <QTextStream>
#include <QFile>

#include "FastSerializer.h"

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
        return -1;
    }

    QFile input(args[1]);
    if (!input.open(QIODevice::ReadOnly))
    {
        qStdout() << "Unable to open file to read";
        return -1;
    }
    QByteArray compressed = input.readAll();
    QByteArray uncompressed = qUncompress(compressed);

    FastDeserializer deserializer(uncompressed.data(), uncompressed.size());

    QString humanized = Humanize(&deserializer);

    QByteArray tail = uncompressed.mid(deserializer.GetIndex());

    humanized.append("\r\n\r\n\r\n");
    humanized.append(tail);

    QFile output(args[2]);
    if (!output.open(QIODevice::WriteOnly))
    {
        qStdout() << "Unable to open file to write";
        return -1;
    }
    if (output.write(humanized.toUtf8()) == -1)
    {
        qStdout() << "Unable to write data to file";
        return -1;
    }

    return 0;
}
