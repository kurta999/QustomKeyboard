#ifndef BALLOONTIP_H
#define BALLOONTIP_H

#include <QWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QTimerEvent>
#include <any>

using BallonParameter = std::any;
using BallonFunction = std::function<void(BallonParameter)>;

class BalloonTip : public QWidget
{
    Q_OBJECT
public:
    static void showBalloon(QMessageBox::Icon icon, const QString& title, const QString& msg, const QPoint& pos, int timeout, bool showArrow = true, 
        BallonFunction f = nullptr, int arrowDir = 5);

    static void showNotificaiton(QMessageBox::Icon icon, const QString& title, const QString& msg, int timeout, BallonFunction f = nullptr);



    static void hideBalloon();
    static bool isBalloonVisible();
    static void updateBalloonPosition(const QPoint& pos);

private:
    BalloonTip(QMessageBox::Icon icon, const QString& title,
                const QString& msg);
    ~BalloonTip();
    void balloon(const QPoint&, int, bool, int arrowDir);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *e) Q_DECL_OVERRIDE;

private:
    QPixmap pixmap;
    int timerId;
    int eventTimerId;
    bool showArrow;
    bool enablePressEvent;
    int arrowDir;
    BallonFunction func{};
};

#endif // BALLOONTIP_H
