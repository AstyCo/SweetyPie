#include "kamemorylegend.h"
#include "kamemorypart.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace Memory;

KaMemoryLegend::KaMemoryLegend(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    this->setSizePolicy(sizePolicy);

//    verticalLayout->setSpacing(0);
//    verticalLayout->setMargin(0);

    for(int i = 0; i<MemoryState_count; ++i)
    {
        MemoryState state = static_cast<MemoryState>(i);
        QHBoxLayout *horizontalLayout = new QHBoxLayout;

        QLabel *labelColor = new QLabel(this);
        labelColor->setSizePolicy(sizePolicy);
        labelColor->setObjectName(MemoryState_to_QString(state)+"Color");
        labelColor->setStyleSheet(
           QString::fromLatin1("background-color: %1").arg(MemoryState_to_QColor(state).name()));
        labelColor->setMinimumSize(QSize(16, 16));
        labelColor->setMaximumSize(QSize(16, 16));
        labelColor->setFrameShape(QFrame::Box);

        horizontalLayout->addWidget(labelColor);

        QLabel *labelText = new QLabel(this);
        labelText->setObjectName(MemoryState_to_QString(state)+"Text");

        QFont font;
        font.setBold(true);
        font.setWeight(75);

        labelText->setFont(font);
        labelText->setText(MemoryState_to_trQString(state));
        labelText->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(labelText);

        verticalLayout->addLayout(horizontalLayout);
    }
}

