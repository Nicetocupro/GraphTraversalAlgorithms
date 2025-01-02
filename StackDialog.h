#ifndef STACKDIALOG_H
#define STACKDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>

class StackDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StackDialog(QWidget *parent = nullptr);
    void push(const QString &item);
    void pop();
    void clearStack();

private:
    QListWidget *stackListWidget;
};

#endif // STACKDIALOG_H
