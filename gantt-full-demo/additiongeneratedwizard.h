#ifndef ADDITIONGENERATEDWIZARD_H
#define ADDITIONGENERATEDWIZARD_H

#include "gantttreemodel.h"
#include <QWizard>
#include <QSettings>

namespace Ui {
class AdditionGeneratedWizard;
}

class AdditionGeneratedWizard : public QWizard
{
    Q_OBJECT

public:
    explicit AdditionGeneratedWizard(QWidget *parent = 0);
    ~AdditionGeneratedWizard();

//    int nextId() const;
    void initializePage(int id);
    void done(int result);

    QList<GanttInfoItem *> generatedItems;

private:
    void addTest();
    void addGenerated();

    void saveSettings();
    void loadSettings();

private:
    Ui::AdditionGeneratedWizard *ui;
    QSettings _settings;
};

#endif // ADDITIONGENERATEDWIZARD_H
