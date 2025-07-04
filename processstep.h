#include <QString>
#include <QJsonObject>
#include <QUuid>

struct ProcessStep
{
    int formIndex;
    QString stepName;
    QJsonObject formData;
    QUuid id;

    ProcessStep() : id(QUuid::createUuid()) {}
};