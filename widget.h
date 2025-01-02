#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QPointer>
#include <QTableView>
#include <QScrollBar>
#include <QStandardItemModel>
#include <random>
#include <QVBoxLayout>
#include <QStack>
#include <QThread>
#include <QTimer>
#include <QQueue>

#include "StackDialog.h"
#include "QRound.h"
#include "QueueDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

struct Edge
{
    QRound *src;
    QRound *dest;
};

struct DfsState
{
    QRound *currentNode;
    QSet<QRound*> visited;
    QList<QRound*> stack;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateAdjacencyListView();

private slots:
    void on_AddNodeButton_clicked();
    void onRoundMoved();

    // Slot functions to handle QRound signals
    void handleConnectNodesRequested(QRound *round);
    void handleDeleteNodesRequested(QRound *round);
    void handleDfsRecursiveRequested(QRound *round);
    void handleDfsNonRecursiveRequested(QRound *round);
    void handleBfsRequested(QRound *round);

private:
    void resetAllNodesColor();
    void dfsRecursive(QRound *round, QSet<QRound*> &visited);
    void dfsNonRecursive(QRound *round);
    void bfs(QRound *round);


    QRound *currentRound; // The node from which to draw the line
    QPoint tempPoint;    // Temporary point to store mouse position while drawing the line
    Ui::Widget *ui;
    QList<QRound*> rounds; // 存储顶点V
    QList<Edge> E; // 存储边E
    int nextNum;
    bool isDrawingLine; // Flag to indicate if a line is being drawn
    bool handleConnect;
    QTableView *adjacencyListView; // 用于显示邻接链表的视图
    QStandardItemModel *adjacencyListModel; // 用于存储邻接链表的模型

    StackDialog *stackDialog; // 用于显示栈操作的对话框
    QueueDialog *queueDialog; // 用于显示队列入出情况的对话框
};

#endif // WIDGET_H
