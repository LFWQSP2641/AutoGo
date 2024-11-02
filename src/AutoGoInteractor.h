#ifndef AUTOGOINTERACTOR_H
#define AUTOGOINTERACTOR_H

#include <QObject>

class AutoGoInteractor : public QObject
{
    Q_OBJECT
public:
    explicit AutoGoInteractor(QObject *parent = nullptr);

signals:
};

#endif // AUTOGOINTERACTOR_H
