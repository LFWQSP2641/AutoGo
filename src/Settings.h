#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class Settings : public QObject
{
    Q_OBJECT

public:
    static Settings *getSingletonSettings();
    explicit Settings(QObject *parent = nullptr);

    bool mumuEnable() const;
    void setMumuEnable(bool newMumuEnable);

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

    QString kataGoAnalysisCommand() const;
    void setKataGoAnalysisCommand(const QString &newKataGoAnalysisCommand);

    QString kataGoGTPCommand() const;
    void setKataGoGTPCommand(const QString &newKataGoGTPCommand);

    QString kataGoMode() const;
    void setKataGoMode(const QString &newKataGoMode);

    bool kataGoSearchLimit() const;
    void setKataGoSearchLimit(bool newKataGoSearchLimit);

    int kataGoMaxVisits() const;
    void setKataGoMaxVisits(int newKataGoMaxVisits);

public slots:
    void saveToFile() const;

protected:
    bool m_mumuEnable;
    QString m_mumuPath;
    int m_mumuIndex;
    int m_mumuDisplayId;
    QString m_adbPath;
    QString m_adbSerial;
    QString m_kataGoPath;
    QString m_kataGoAnalysisCommand;
    QString m_kataGoGTPCommand;
    QString m_kataGoMode;
    bool m_kataGoSearchLimit;
    int m_kataGoMaxVisits;

signals:
    void mumuEnableChanged();

    void mumuPathChanged();

    void mumuIndexChanged();

    void mumuDisplayIdChanged();

    void adbPathChanged();

    void adbSerialChanged();

    void kataGoPathChanged();

    void kataGoAnalysisCommandChanged();

    void kataGoGTPCommandChanged();

    void kataGoModeChanged();

    void kataGoSearchLimitChanged();

    void kataGoMaxVisitsChanged();

private:
    Q_PROPERTY(bool mumuEnable READ mumuEnable WRITE setMumuEnable NOTIFY mumuEnableChanged FINAL)
    Q_PROPERTY(QString mumuPath READ mumuPath WRITE setMumuPath NOTIFY mumuPathChanged FINAL)
    Q_PROPERTY(int mumuIndex READ mumuIndex WRITE setMumuIndex NOTIFY mumuIndexChanged FINAL)
    Q_PROPERTY(int mumuDisplayId READ mumuDisplayId WRITE setMumuDisplayId NOTIFY mumuDisplayIdChanged FINAL)
    Q_PROPERTY(QString adbPath READ adbPath WRITE setAdbPath NOTIFY adbPathChanged FINAL)
    Q_PROPERTY(QString adbSerial READ adbSerial WRITE setAdbSerial NOTIFY adbSerialChanged FINAL)
    Q_PROPERTY(QString kataGoPath READ kataGoPath WRITE setKataGoPath NOTIFY kataGoPathChanged FINAL)
    Q_PROPERTY(QString kataGoAnalysisCommand READ kataGoAnalysisCommand WRITE setKataGoAnalysisCommand NOTIFY kataGoAnalysisCommandChanged FINAL)
    Q_PROPERTY(QString kataGoGTPCommand READ kataGoGTPCommand WRITE setKataGoGTPCommand NOTIFY kataGoGTPCommandChanged FINAL)
    Q_PROPERTY(QString kataGoMode READ kataGoMode WRITE setKataGoMode NOTIFY kataGoModeChanged FINAL)
    Q_PROPERTY(bool kataGoSearchLimit READ kataGoSearchLimit WRITE setKataGoSearchLimit NOTIFY kataGoSearchLimitChanged FINAL)
    Q_PROPERTY(int kataGoMaxVisits READ kataGoMaxVisits WRITE setKataGoMaxVisits NOTIFY kataGoMaxVisitsChanged FINAL)
};

#endif // SETTINGS_H
