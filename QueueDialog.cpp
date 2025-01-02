#include "QueueDialog.h"


QueueDialog::QueueDialog(QWidget *parent)
    : QDialog(parent), queueListWidget(new QListWidget(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(queueListWidget);
    setLayout(layout);
    setWindowTitle("Queue Visualization");
    setFixedSize(200, 400);
}

void QueueDialog::enqueue(const QString &item)
{
    queueListWidget->addItem(item); // Insert at the end
}

void QueueDialog::dequeue()
{
    if (queueListWidget->count() > 0)
    {
        delete queueListWidget->takeItem(0); // Remove the first item
    }
}

void QueueDialog::clearQueue()
{
    queueListWidget->clear();
}
