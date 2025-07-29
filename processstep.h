#include <QString>
#include <QJsonObject>
#include <QUuid>
#include <QJsonArray>

// ProcessStep 结构体定义
struct ProcessStep
{
    int formIndex;          // 表单索引
    QString stepName;       // 步骤名称
    QJsonObject formData;   // 表单数据
    QUuid id;               // 唯一标识符

    // 构造函数，自动生成UUID
    ProcessStep() : id(QUuid::createUuid()) {}

    // 从 QJsonObject 加载数据
    static ProcessStep fromJson(const QJsonObject &json) {
        ProcessStep step;
        if (json.contains("id") && json["id"].isString()) {
            step.id = QUuid(json["id"].toString());
        }
        step.formIndex = json["formIndex"].toInt();
        step.stepName = json["stepName"].toString();
        step.formData = json["formData"].toObject();
        return step;
    }

    // 将数据转换为 QJsonObject
    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = id.toString();
        json["formIndex"] = formIndex;
        json["stepName"] = stepName;
        json["formData"] = formData;
        return json;
    }
};
