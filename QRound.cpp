#include "QRound.h"
#include <QPainter>
#include <QMouseEvent>

QRound::QRound(QWidget *parent, int num)
    : QLabel(parent),
    number(num),
    pos_x(10),
    pos_y(10),
    m_nMsgCount(0),
    currentColor(Qt::blue),
    defaultColor(Qt::blue)
{
    setFixedSize(100, 100); // 设置固定大小，可以根据需要调整
    m_pTimer = new QTimer(this);
    this->setText(QString::number(number));
    connect(m_pTimer, &QTimer::timeout, this, &QRound::onTimeout);
    m_pTimer->start(1000); // 每秒触发一次
}

QRound::~QRound()
{
    // 释放资源
    if (m_pTimer)
    {
        m_pTimer->stop();
        delete m_pTimer;
        m_pTimer = nullptr;
    }
}

// 新增 centerPos 函数的实现
QPoint QRound::centerPos() const
{
    QPoint globalCenter = mapToParent(QPoint(width() / 2, height() / 2));
    return globalCenter;
}

int QRound::getNum() const
{
    return this->number;
}

void QRound::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 设置画刷颜色
    QBrush brush(currentColor, Qt::SolidPattern);
    painter.setBrush(brush);

    // 绘制填充颜色的圆形
    painter.drawEllipse(pos_x, pos_y, 50, 50);
}

void QRound::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->pos();
        event->accept();
    }
    else if (event->button() == Qt::RightButton)
    {
        // Handle right button click
        showContextMenu(event->globalPos());
    }
    QLabel::mousePressEvent(event);
}

void QRound::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int dx = event->pos().x() - dragPosition.x();
        int dy = event->pos().y() - dragPosition.y();
        move(x() + dx, y() + dy);
        emit moved(); // 发出信号，通知圆已移动
        event->accept();
    }
    else
    {
        // 即使没有按下左键，仍然传递事件以更新tempPoint
        QLabel::mouseMoveEvent(event); // Call base class implementation if needed
    }
}

void QRound::onTimeout()
{
    m_nMsgCount++;
    // 处理计时器事件，例如更新界面或执行其他任务
}

void QRound::showContextMenu(const QPoint &globalPos)
{
    QMenu menu;

    QAction *action1 = menu.addAction("连接节点");
    QAction *action2 = menu.addAction("删除节点");
    QAction *action3 = menu.addAction("递归dfs");
    QAction *action4 = menu.addAction("非递归dfs");
    QAction *action5 = menu.addAction("bfs");

    QAction *selectedAction = menu.exec(globalPos);

    if (selectedAction == action1)
    {
        emit connectNodesRequested(this);
    }
    else if (selectedAction == action2)
    {
        emit deleteNodesRequested(this);
    }
    else if (selectedAction == action3)
    {
        emit dfsRecursiveRequested(this);
    }
    else if (selectedAction == action4)
    {
        emit dfsNonRecursiveRequested(this);
    }
    else if (selectedAction == action5)
    {
        emit bfsRequested(this);
    }
}

void QRound::setColor(const QColor &color)
{
    qDebug() << color << number;
    currentColor = color;
    update(); // 重新绘制节点
}

void QRound::resetColor()
{
    currentColor = defaultColor;
    update(); // 重新绘制节点
}

