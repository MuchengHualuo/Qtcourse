#include "studentinfo.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStringList>

StudentInfo StudentInfo::defaults()
{
    StudentInfo info;
    info.title       = QStringLiteral("关于本程序");
    info.application = QStringLiteral("示例 2.4  混合方式 UI 设计");

    // ------------------------------------------------------------------
    //  个人信息（与 test/about.json 保持一致）
    // ------------------------------------------------------------------
    info.name        = QStringLiteral("陈良宇");
    info.studentId   = QStringLiteral("2024414290302");
    info.className   = QStringLiteral("软件工程三班");
    info.major       = QStringLiteral("软件工程");
    info.school      = QStringLiteral("计算机学院");

    return info;
}

StudentInfo StudentInfo::fromJsonFile(const QString &filePath)
{
    StudentInfo info = defaults();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return info;

    QJsonParseError error{};
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return info;

    const QJsonObject obj = doc.object();
    auto pick = [&obj](const char *key, QString &target) {
        const QJsonValue value = obj.value(QLatin1String(key));
        if (value.isString() && !value.toString().trimmed().isEmpty())
            target = value.toString().trimmed();
    };

    pick("title",       info.title);
    pick("application", info.application);
    pick("name",        info.name);
    pick("studentId",   info.studentId);
    pick("className",   info.className);
    pick("major",       info.major);
    pick("school",      info.school);

    return info;
}

StudentInfo StudentInfo::load()
{
    QStringList candidates;
    if (QCoreApplication::instance())
        candidates << QCoreApplication::applicationDirPath() + QStringLiteral("/test/about.json");
    candidates << QDir::currentPath() + QStringLiteral("/test/about.json");

    for (const QString &path : candidates) {
        if (QFileInfo::exists(path))
            return fromJsonFile(path);
    }
    return defaults();
}

QString StudentInfo::toString() const
{
    return QStringLiteral("StudentInfo{name=%1, studentId=%2, className=%3}")
        .arg(name, studentId, className);
}
