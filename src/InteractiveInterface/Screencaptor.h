#ifndef SCREENCAPTOR_H
#define SCREENCAPTOR_H

#include <QImage>
#include <QObject>
#include <optional>

class Screencaptor : public QObject
{
    Q_OBJECT

public:
    explicit Screencaptor(QObject *parent = nullptr);

    virtual bool init() = 0;

    virtual std::optional<QImage> screencap() = 0;

signals:
};

#endif // SCREENCAPTOR_H
