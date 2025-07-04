#include <QList>
#include "ProcessStep.h"

struct ProcessStage
{
    QString stageName;
    QUuid id;
    QList<ProcessStep> steps; // 一个阶段包含一个步骤列表

    ProcessStage() : id(QUuid::createUuid()) {}
};