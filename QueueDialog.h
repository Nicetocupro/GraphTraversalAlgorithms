#ifndef QUEUEDIALOG_H
#define QUEUEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>

class QueueDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QueueDialog(QWidget *parent = nullptr);
    void enqueue(const QString &item);
    void dequeue();
    void clearQueue();

private:
    QListWidget *queueListWidget;
};

#endif // QUEUEDIALOG_H
