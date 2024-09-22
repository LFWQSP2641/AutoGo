#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QRect>

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent = nullptr);

    virtual bool click(const QPoint &p) = 0;
    virtual bool click(const QRect &rect);

protected:
    static QPoint normDistributionGenerator(const QRect &rect);
    static int normDistributionGenerator(int minVal, int maxVal);

signals:
};

#endif // CONTROLLER_H
