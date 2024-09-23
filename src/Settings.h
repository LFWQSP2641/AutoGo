#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class Settings : public QObject
{
    Q_OBJECT

public:
    static Settings *getSingletonSettings();
    explicit Settings(QObject *parent = nullptr);

    QString mumuPath() const;
    void setMumuPath(const QString &newMumuPath);

    int mumuIndex() const;
    void setMumuIndex(int newMumuIndex);

    int mumuDisplayId() const;
    void setMumuDisplayId(int newMumuDisplayId);

    QString adbPath() const;
    void setAdbPath(const QString &newAdbPath);

    QString adbSerial() const;
    void setAdbSerial(const QString &newAdbSerial);

    QString kataGoPath() const;
    void setKataGoPath(const QString &newKataGoPath);

    QString kataGoCommand() const;
    void setKataGoCommand(const QString &newKataGoCommand);

public slots:
    void saveToFile() const;

protected:
    QString m_mumuPath;
    int m_mumuIndex;
    int m_mumuDisplayId;
    QString m_adbPath;
    QString m_adbSerial;
    QString m_kataGoPath;
    QString m_kataGoCommand;

signals:
    void mumuPathChanged();

    void mumuIndexChanged();

    void mumuDisplayIdChanged();

    void adbPathChanged();

    void adbSerialChanged();

    void kataGoPathChanged();

    void kataGoCommandChanged();

private:
    Q_PROPERTY(QString mumuPath READ mumuPath WRITE setMumuPath NOTIFY mumuPathChanged FINAL)
    Q_PROPERTY(int mumuIndex READ mumuIndex WRITE setMumuIndex NOTIFY mumuIndexChanged FINAL)
    Q_PROPERTY(int mumuDisplayId READ mumuDisplayId WRITE setMumuDisplayId NOTIFY mumuDisplayIdChanged FINAL)
    Q_PROPERTY(QString adbPath READ adbPath WRITE setAdbPath NOTIFY adbPathChanged FINAL)
    Q_PROPERTY(QString adbSerial READ adbSerial WRITE setAdbSerial NOTIFY adbSerialChanged FINAL)
    Q_PROPERTY(QString kataGoPath READ kataGoPath WRITE setKataGoPath NOTIFY kataGoPathChanged FINAL)
    Q_PROPERTY(QString kataGoCommand READ kataGoCommand WRITE setKataGoCommand NOTIFY kataGoCommandChanged FINAL)
};

#endif // SETTINGS_H
