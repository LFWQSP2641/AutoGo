#ifndef SCREENCAPTOR_H
#define SCREENCAPTOR_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <optional>

class Screencaptor : public QObject
{
    Q_OBJECT

public:
    explicit Screencaptor(QObject *parent = nullptr);

    virtual bool init() = 0;

    virtual std::optional<cv::Mat> screencap() = 0;

signals:
};

#endif // SCREENCAPTOR_H
