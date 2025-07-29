#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include "processstep.h"

// ProcessStage 结构体定义
struct ProcessStage
{
    QString stageName;      // 阶段名称
    QUuid id;               // 唯一标识符
    QList<ProcessStep> steps; // 一个阶段包含一个步骤列表

    // 构造函数，自动生成UUID
    ProcessStage() : id(QUuid::createUuid()) {}

    // 从 QJsonObject 加载数据
    static ProcessStage fromJson(const QJsonObject &json) {
        ProcessStage stage;
        if (json.contains("id") && json["id"].isString()) {
            stage.id = QUuid(json["id"].toString());
        }
        stage.stageName = json["stageName"].toString();
        if (json.contains("steps") && json["steps"].isArray()) {
            QJsonArray stepsArray = json["steps"].toArray();
            for (const QJsonValue &stepValue : stepsArray) {
                stage.steps.append(ProcessStep::fromJson(stepValue.toObject()));
            }
        }
        return stage;
    }

    // 将数据转换为 QJsonObject
    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = id.toString();
        json["stageName"] = stageName;
        QJsonArray stepsArray;
        for (const ProcessStep &step : steps) {
            stepsArray.append(step.toJson());
        }
        json["steps"] = stepsArray;
        return json;
    }
};
