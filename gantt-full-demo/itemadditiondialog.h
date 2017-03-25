#ifndef ITEMADDITIONDIALOG_H
#define ITEMADDITIONDIALOG_H

#include "extensions/utcdatetime.h"

#include <QDialog>
#include <QSettings>

namespace Ui {
class ItemAdditionDialog;
}

class ItemAdditionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemAdditionDialog(QWidget *parent = 0);
    ~ItemAdditionDialog();

    void moveStart(const UtcDateTime &dt);

    QString title() const;
    QColor color() const;
    bool isDot() const;
    UtcDateTime start() const;
    UtcDateTime finish() const;

private slots:
    void onBeginChanged(const QDateTime &dt);
    void onEndChanged(const QDateTime &dt);

    void on_pushButtonColor_clicked();

    void on_checkBoxInterval_stateChanged(int arg1);

private:
    void saveSettings();
    void loadSettings();

    void setColor(const QColor &color);
    void hideFinish();
    void showFinish();

private:
    QColor _color;
    Ui::ItemAdditionDialog *ui;
    QSettings _settings;
};

#endif // ITEMADDITIONDIALOG_H
