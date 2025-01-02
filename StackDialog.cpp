#include "StackDialog.h"


StackDialog::StackDialog(QWidget *parent)
    : QDialog(parent), stackListWidget(new QListWidget(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(stackListWidget);
    setLayout(layout);
    setWindowTitle("Stack Visualization");
    setFixedSize(200, 400);
}

void StackDialog::push(const QString &item)
{
    stackListWidget->insertItem(0, item); // Insert at the top
}

void StackDialog::pop()
{
    if (stackListWidget->count() > 0)
    {
        delete stackListWidget->takeItem(0); // Remove the top item
    }
}

void StackDialog::clearStack()
{
    stackListWidget->clear();
}
