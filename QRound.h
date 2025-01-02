#ifndef QROUND_H
#define QROUND_H

#include <QLabel>
#include <QPoint>
#include <QTimer>
#include <QMenu>


class QRound : public QLabel
{
    Q_OBJECT
public:
    explicit QRound(QWidget *parent = nullptr, int number = 0);
    // 新增函数声明
    QPoint centerPos() const;
    void setColor(const QColor &color); // 设置节点颜色
    void resetColor(); // 恢复节点颜色
    int getNum() const;
    ~QRound();  // 新增析构函数声明

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void showContextMenu(const QPoint &globalPos);


signals:
    void connectNodesRequested(QRound *round);
    void deleteNodesRequested(QRound *round);
    void dfsRecursiveRequested(QRound *round);
    void dfsNonRecursiveRequested(QRound *round);
    void bfsRequested(QRound *round);
    void moved(); // 新增信号声明

private slots:
    void onTimeout();

private:
    int number;
    QPoint dragPosition;
    int pos_x;
    int pos_y;
    QTimer *m_pTimer;
    int m_nMsgCount;
    QColor currentColor; // 当前颜色
    QColor defaultColor; // 默认颜色
};

#endif // QROUND_H
