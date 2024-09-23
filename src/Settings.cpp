#include "Settings.h"

#include "Global.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

Q_GLOBAL_STATIC(Settings, singletonSettings)

Settings *Settings::getSingletonSettings()
{
    return singletonSettings;
}

Settings::Settings(QObject *parent)
    : QObject{ parent }
{
    QByteArray fileData;
    QFile file{ Global::configPath().append(QStringLiteral("/setting.json")) };
    if (file.exists())
    {
        file.open(QFile::ReadOnly);
        fileData = file.readAll();
    }
    else
    {
        file.open(QFile::NewOnly);
    }
    file.close();

    const auto settingJsonObject{ QJsonDocument::fromJson(fileData).object() };

    m_mumuPath = settingJsonObject.value(QStringLiteral("mumuPath")).toString();
    m_mumuIndex = settingJsonObject.value(QStringLiteral("mumuIndex")).toInt(0);
    m_mumuDisplayId = settingJsonObject.value(QStringLiteral("mumuDisplayId")).toInt(0);
    m_adbPath = settingJsonObject.value(QStringLiteral("adbPath")).toString();
    m_adbSerial = settingJsonObject.value(QStringLiteral("adbSerial")).toString();
    m_kataGoPath = settingJsonObject.value(QStringLiteral("kataGoPath")).toString();
    m_kataGoCommand = settingJsonObject.value(QStringLiteral("kataGoCommand")).toString();
}

QString Settings::mumuPath() const
{
    return m_mumuPath;
}

void Settings::setMumuPath(const QString &newMumuPath)
{
    if (m_mumuPath == newMumuPath)
        return;
    m_mumuPath = newMumuPath;
    emit mumuPathChanged();
}

int Settings::mumuIndex() const
{
    return m_mumuIndex;
}

void Settings::setMumuIndex(int newMumuIndex)
{
    if (m_mumuIndex == newMumuIndex)
        return;
    m_mumuIndex = newMumuIndex;
    emit mumuIndexChanged();
}

int Settings::mumuDisplayId() const
{
    return m_mumuDisplayId;
}

void Settings::setMumuDisplayId(int newMumuDisplayId)
{
    if (m_mumuDisplayId == newMumuDisplayId)
        return;
    m_mumuDisplayId = newMumuDisplayId;
    emit mumuDisplayIdChanged();
}

QString Settings::adbPath() const
{
    return m_adbPath;
}

void Settings::setAdbPath(const QString &newAdbPath)
{
    if (m_adbPath == newAdbPath)
        return;
    m_adbPath = newAdbPath;
    emit adbPathChanged();
}

QString Settings::adbSerial() const
{
    return m_adbSerial;
}

void Settings::setAdbSerial(const QString &newAdbSerial)
{
    if (m_adbSerial == newAdbSerial)
        return;
    m_adbSerial = newAdbSerial;
    emit adbSerialChanged();
}

QString Settings::kataGoPath() const
{
    return m_kataGoPath;
}

void Settings::setKataGoPath(const QString &newKataGoPath)
{
    if (m_kataGoPath == newKataGoPath)
        return;
    m_kataGoPath = newKataGoPath;
    emit kataGoPathChanged();
}

QString Settings::kataGoCommand() const
{
    return m_kataGoCommand;
}

void Settings::setKataGoCommand(const QString &newKataGoCommand)
{
    if (m_kataGoCommand == newKataGoCommand)
        return;
    m_kataGoCommand = newKataGoCommand;
    emit kataGoCommandChanged();
}

void Settings::saveToFile() const
{
    qDebug() << Q_FUNC_INFO;
    QJsonObject settingJsonObject;
    settingJsonObject.insert(QStringLiteral("mumuPath"), m_mumuPath);
    settingJsonObject.insert(QStringLiteral("mumuIndex"), m_mumuIndex);
    settingJsonObject.insert(QStringLiteral("mumuDisplayId"), m_mumuDisplayId);
    settingJsonObject.insert(QStringLiteral("adbPath"), m_adbPath);
    settingJsonObject.insert(QStringLiteral("adbSerial"), m_adbSerial);
    settingJsonObject.insert(QStringLiteral("kataGoPath"), m_kataGoPath);
    settingJsonObject.insert(QStringLiteral("kataGoCommand"), m_kataGoCommand);

    QFile file{ Global::configPath().append(QStringLiteral("/setting.json")) };
    file.open(QFile::WriteOnly);
    file.write(QJsonDocument(settingJsonObject).toJson(QJsonDocument::Compact));
    file.close();
}
