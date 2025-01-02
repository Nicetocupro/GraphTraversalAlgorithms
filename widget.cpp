#include "Widget.h"
#include "ui_Widget.h"
#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , currentRound(nullptr)
    , ui(new Ui::Widget)
    , isDrawingLine(false)
    , handleConnect(false)
    , adjacencyListView(new QTableView(this))
    , adjacencyListModel(new QStandardItemModel(this))
    , stackDialog(new StackDialog(this))
{
    setMouseTracking(true); // 确保窗口接受鼠标移动事件
    ui->setupUi(this);
    nextNum = 0;

    // 初始化邻接链表视图
    adjacencyListView->setModel(adjacencyListModel);
    adjacencyListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    adjacencyListView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    adjacencyListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 隐藏行号列
    adjacencyListView->verticalHeader()->hide();

    // 创建一个容器 QWidget 用于包含 QTableView
    QWidget *container = new QWidget(this);
    container->setGeometry(800, 0, 390, 600); // 设置容器的位置和大小

    // 创建布局管理器并将 QTableView 添加到容器中
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->addWidget(adjacencyListView);

    updateAdjacencyListView(); // 更新邻接链表视图
}

Widget::~Widget()
{
    delete ui;
    // 删除QRound对象
    qDeleteAll(rounds);
    rounds.clear();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << event->button();

    if (event->button() == Qt::RightButton && handleConnect)
    {
        // Check if a node is clicked
        bool nodeClicked = false;
        for (auto *round : rounds)
        {
            if (round && round->geometry().contains(event->pos()))
            {
                currentRound = round;
                isDrawingLine = true; // Start drawing line
                tempPoint = event->pos();
                nodeClicked = true;
                update(); // Request repaint to show the line
                updateAdjacencyListView(); // 更新邻接链表视图
                break;
            }
        }
        if (!nodeClicked) {
            // If no node is clicked, cancel drawing line
            isDrawingLine = false;
            currentRound = nullptr;
            handleConnect = false;
            update(); // Request repaint to clear the line
            updateAdjacencyListView(); // 更新邻接链表视图
        }
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (isDrawingLine && handleConnect)
    {
        bool nearNode = false;
        for (auto *round : rounds)
        {
            if (round && round->geometry().adjusted(-10, -10, 10, 10).contains(event->pos())) // 调整检测范围
            {
                tempPoint = round->centerPos();
                nearNode = true;
                break;
            }
        }
        if (!nearNode) {
            tempPoint = event->pos(); // Update the temporary point while moving
        }
        update(); // Request a repaint to show the line
        updateAdjacencyListView(); // 更新邻接链表视图
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (isDrawingLine && event->button() == Qt::LeftButton && handleConnect)
    {
        for (auto *round : rounds)
        {
            if (round && round != currentRound && round->geometry().contains(event->pos()))
            {
                bool edgeExists = false;
                for (const Edge &edge : std::as_const(E))
                {
                    if ((edge.src == currentRound && edge.dest == round) ||
                        (edge.src == round && edge.dest == currentRound))
                    {
                        edgeExists = true;
                        break;
                    }
                }
                if (!edgeExists)
                {
                    Edge edge;
                    edge.src = currentRound;
                    edge.dest = round;
                    E.append(edge); // Add the edge to the list
                }
                handleConnect = false;
                break;
            }
        }
        update(); // Request a repaint
        updateAdjacencyListView(); // 更新邻接链表视图
    }
}

void Widget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 绘制所有QRound对象
    for (QRound* round : std::as_const(rounds)) {
        if (round) {
            round->show();
        }
    }

    // 绘制所有线
    for (const Edge &edge : E)
    {
        if (edge.src && edge.dest)
        {
            painter.drawLine(edge.src->centerPos(), edge.dest->centerPos());
        }
    }

    // Draw the temporary line if a node is selected and drawing is in progress
    if (isDrawingLine && currentRound && handleConnect)
    {
        painter.setPen(QPen(Qt::DashLine));
        painter.drawLine(currentRound->centerPos(), tempPoint);
    }
}


void Widget::on_AddNodeButton_clicked()
{
    int width = 800 , height = 600;
    QRound *round = new QRound(this , nextNum);
    nextNum++;

    // 使用随机设备和 Mersenne Twister 随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, width - round->width());
    std::uniform_int_distribution<> disY(0, height - round->height());

    int x = disX(gen); // 生成随机的X坐标
    int y = disY(gen); // 生成随机的Y坐标

    round->move(x, y); // 设置圆的位置
    rounds.append(round); // 将QRound对象添加到列表中
    update(); // 请求重新绘制窗口，会触发paintEvent

    // Connect QRound signals to slots
    connect(round, &QRound::moved, this, &Widget::onRoundMoved); // 连接 moved 信号到槽函数
    connect(round, &QRound::connectNodesRequested, this, &Widget::handleConnectNodesRequested);
    connect(round, &QRound::deleteNodesRequested, this, &Widget::handleDeleteNodesRequested);
    connect(round, &QRound::dfsRecursiveRequested, this, &Widget::handleDfsRecursiveRequested);
    connect(round, &QRound::dfsNonRecursiveRequested, this, &Widget::handleDfsNonRecursiveRequested);
    connect(round, &QRound::bfsRequested, this, &Widget::handleBfsRequested);

    updateAdjacencyListView(); // 更新邻接链表视图
}

void Widget::onRoundMoved()
{
    update(); // 请求重新绘制窗口
}

void Widget::handleConnectNodesRequested(QRound *round)
{
    qDebug() << "Connect nodes requested";
    if (round)
    {
        handleConnect = true;
        currentRound = round; // Set the selected node to currentRound
        isDrawingLine = true; // Start drawing line
    }
}


void Widget::handleDeleteNodesRequested(QRound *round)
{
    qDebug() << "Delete nodes requested";

    if (round)
    {
        round->disconnect();

        // 使用 QMutableListIterator 安全地遍历并移除所有与该节点相关的边
        QMutableListIterator<Edge> i(E);
        while (i.hasNext())
        {
            const Edge &edge = i.next();
            if (edge.src == round || edge.dest == round)
            {
                i.remove();
            }
        }

        // 从 rounds 列表中移除该节点
        rounds.removeAll(round);

        // 使用 deleteLater 安全地删除节点
        round->deleteLater();

        // 更新窗口以重新绘制
        update();

        updateAdjacencyListView(); // 更新邻接链表视图
    }
}

void Widget::dfsRecursive(QRound *round, QSet<QRound*> &visited)
{
    if (!round || visited.contains(round))
        return;

    round->setColor(Qt::red); // 设置正在遍历的节点颜色为红色
    qDebug() << "Visiting node:" << round->text() << "Stack push:" << round->text();

    // 短暂延迟以模拟动画效果
    QThread::msleep(500);
    QCoreApplication::processEvents();

    visited.insert(round);
    round->setColor(Qt::green); // 将遍历过的节点颜色设置为绿色

    for (const Edge &edge : E)
    {
        if (edge.src == round && !visited.contains(edge.dest))
        {
            dfsRecursive(edge.dest, visited);
        }
        else if (edge.dest == round && !visited.contains(edge.src))
        {
            dfsRecursive(edge.src, visited);
        }
    }

    qDebug() << "Stack pop:" << round->text();
    QThread::msleep(100);
    QCoreApplication::processEvents();
}


// 遍历完成后调用该函数将所有节点颜色重置为蓝色
void Widget::resetAllNodesColor()
{
    for (QRound* round : std::as_const(rounds)) {
        if (round)
        {
            round->setColor(Qt::blue);
        }
    }
    update(); // 请求重新绘制窗口
}

void Widget::handleDfsRecursiveRequested(QRound *round)
{
    qDebug() << "DFS Recursive requested";

    QSet<QRound*> visited;
    dfsRecursive(round, visited);

    // 确保遍历结束后将所有节点颜色重置为蓝色
    resetAllNodesColor();
}

void Widget::dfsNonRecursive(QRound *round)
{
    if (!round)
        return;

    QSet<QRound*> visited;
    QStack<QRound*> stack;
    stack.push(round);

    stackDialog->push(round->text()); // 更新对话框显示栈入情况
    QThread::msleep(500); // 短暂延迟以模拟动画效果
    QCoreApplication::processEvents(); // 处理事件，使 UI 能够更新

    while (!stack.isEmpty())
    {
        QThread::msleep(500); // 短暂延迟以模拟动画效果
        QCoreApplication::processEvents(); // 处理事件，使 UI 能够更新

        QRound *node = stack.pop();
        stackDialog->pop(); // 更新对话框显示栈出情况
        qDebug() << "Stack pop:" << node->text();

        QThread::msleep(500); // 短暂延迟以模拟动画效果
        QCoreApplication::processEvents(); // 处理事件，使 UI 能够更新

        if (!visited.contains(node))
        {
            node->setColor(Qt::red); // 设置节点颜色为红色
            qDebug() << "Visiting node:" << node->text();

            // 短暂延迟以模拟动画效果
            QThread::msleep(500);
            QCoreApplication::processEvents(); // 处理事件，使 UI 能够更新

            visited.insert(node);
            node->setColor(Qt::green); // 将遍历过的节点颜色设置为绿色

            // 短暂延迟以模拟动画效果
            QThread::msleep(300);
            QCoreApplication::processEvents(); // 处理事件，使 UI 能够更新

            // 确保邻接节点按照与递归调用顺序一致的顺序入栈
            QList<QRound*> neighbors;
            for (const Edge &edge : E)
            {
                if (edge.src == node && !visited.contains(edge.dest))
                {
                    neighbors.append(edge.dest);
                }
                else if (edge.dest == node && !visited.contains(edge.src))
                {
                    neighbors.append(edge.src);
                }
            }
            // 逆序入栈，以确保顺序与递归一致
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it)
            {
                QThread::msleep(500); // 短暂延迟以模拟动画效果
                QCoreApplication::processEvents(); // 处理事件，使 UI 能够更新

                stack.push(*it);
                stackDialog->push((*it)->text()); // 更新对话框显示栈入情况
                qDebug() << "Stack push:" << (*it)->text();
            }
        }
    }
}


void Widget::handleDfsNonRecursiveRequested(QRound *round)
{
    qDebug() << "DFS Non-Recursive requested";

    stackDialog = new StackDialog(this); // 创建对话框实例
    stackDialog->show(); // 显示对话框

    dfsNonRecursive(round);

    stackDialog->close(); // 关闭对话框
    delete stackDialog;

    // 确保遍历结束后将所有节点颜色重置为蓝色
    resetAllNodesColor();
}

void Widget::handleBfsRequested(QRound *round)
{
    qDebug() << "BFS requested";

    queueDialog = new QueueDialog(this); // 创建对话框实例
    queueDialog->show(); // 显示对话框

    bfs(round);

    queueDialog->close(); // 关闭对话框
    delete queueDialog;
    queueDialog = nullptr;

    // 确保遍历结束后将所有节点颜色重置为蓝色
    resetAllNodesColor();
}

void Widget::bfs(QRound *round)
{
    if (!round)
        return;

    QSet<QRound*> visited;

    QQueue<QRound*> queue;
    queue.enqueue(round);
    queueDialog->enqueue(round->text()); // 更新对话框显示队列入情况

    QThread::msleep(500); // 短暂延迟以模拟动画效果
    QCoreApplication::processEvents();

    while (!queue.isEmpty())
    {
        QThread::msleep(500); // 短暂延迟以模拟动画效果
        QCoreApplication::processEvents();

        QRound *node = queue.dequeue();
        queueDialog->dequeue(); // 更新对话框显示队列出情况
        qDebug() << "Queue dequeue:" << node->text();

        QThread::msleep(500); // 短暂延迟以模拟动画效果
        QCoreApplication::processEvents();

        if (!visited.contains(node))
        {
            node->setColor(Qt::red); // 设置节点颜色为红色
            qDebug() << "Visiting node:" << node->text();

            // 短暂延迟以模拟动画效果
            QThread::msleep(500);
            QCoreApplication::processEvents();

            visited.insert(node);
            node->setColor(Qt::green); // 将遍历过的节点颜色设置为绿色

            // 短暂延迟以模拟动画效果
            QThread::msleep(300);
            QCoreApplication::processEvents();

            // 将邻接节点入队
            for (const Edge &edge : E)
            {
                if (edge.src == node && !visited.contains(edge.dest))
                {
                    QThread::msleep(500); // 短暂延迟以模拟动画效果
                    QCoreApplication::processEvents();

                    queue.enqueue(edge.dest);
                    queueDialog->enqueue(edge.dest->text()); // 更新对话框显示队列入情况
                    qDebug() << "Queue enqueue:" << edge.dest->text();
                }
                else if (edge.dest == node && !visited.contains(edge.src))
                {
                    QThread::msleep(500); // 短暂延迟以模拟动画效果
                    QCoreApplication::processEvents();

                    queue.enqueue(edge.src);
                    queueDialog->enqueue(edge.src->text()); // 更新对话框显示队列入情况
                    qDebug() << "Queue enqueue:" << edge.src->text();
                }
            }
        }
    }
}


void Widget::updateAdjacencyListView()
{
    adjacencyListModel->clear();

    // 设置模型的列标题
    adjacencyListModel->setHorizontalHeaderLabels({"Node", "Adjacent Nodes"});

    for (QRound *round : std::as_const(rounds))
    {
        if (round)
        {
            QList<QStandardItem*> row;
            row.append(new QStandardItem(QString::number(round->getNum())));

            QString adjacentNodes;
            for (const Edge &edge : std::as_const(E))
            {
                if (edge.src == round)
                {
                    adjacentNodes += QString::number(edge.dest->getNum()) + " ";
                }
                else if(edge.dest == round)
                {
                    adjacentNodes += QString::number(edge.src->getNum()) + " ";
                }
            }
            row.append(new QStandardItem(adjacentNodes.trimmed()));

            adjacencyListModel->appendRow(row);
        }
    }

    adjacencyListView->resizeColumnsToContents(); // 自动调整列宽
    adjacencyListView->resizeRowsToContents(); // 自动调整行高
    adjacencyListView->viewport()->update(); // 强制视图更新
}
