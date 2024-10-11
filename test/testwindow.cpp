#include "testwindow.h"

TestWindow::TestWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->rand_ = new QRandomGenerator();
    this->font_ = new QFont("consolas", 14);

    int rows = 6;
    int columns = 8;
    int vertical_span = 96;
    int horizontal_span = 128;
    int spacing = 5;
    bool static_layout = true;

    this->tileLayout = new QTileLayout(rows, columns, vertical_span, horizontal_span, spacing, spacing);

    this->tileLayout->acceptDragAndDrop(true);
    this->tileLayout->acceptResizing(true);

    this->tileLayout->setCursorIdle(Qt::CursorShape::ArrowCursor);
    this->tileLayout->setCursorGrab(Qt::CursorShape::OpenHandCursor);
    this->tileLayout->setCursorResizeHorizontal(Qt::CursorShape::SizeHorCursor);
    this->tileLayout->setCursorResizeVertical(Qt::CursorShape::SizeVerCursor);

    this->tileLayout->setColorIdle(QColor(240, 240, 240));
    this->tileLayout->setColorResize(QColor(211, 211, 211));
    this->tileLayout->setColorDragAndDrop(QColor(211, 211, 211));
    this->tileLayout->setColorEmptyCheck(QColor(150, 150, 150));

    for (int r = 0; r < rows - 2; r++) {
        for (int c = 0; c < columns; c++) {
            QLabel *label = new QLabel(this);
            this->spawnRandomLabel(*label);
            this->tileLayout->addWidget(label, r, c);
        }
    }
    QLabel *label = new QLabel(this);
    this->spawnRandomLabel(*label);
    this->tileLayout->addWidget(label, rows - 2, 1, 2, 2);

    label = new QLabel(this);
    this->spawnRandomLabel(*label);
    this->tileLayout->addWidget(label, rows - 1, 0, 1, 1);
    this->tileLayout->removeWidget(label);

    this->tileLayout->setVerticalSpacing(spacing);
    this->tileLayout->setHorizontalSpacing(spacing);
    this->tileLayout->setRowsMinimumHeight(vertical_span);
    this->tileLayout->setColumnsMinimumWidth(horizontal_span);
    this->tileLayout->setRowsHeight(vertical_span);
    this->tileLayout->setColumnsWidth(horizontal_span);

    this->tileLayout->activateFocus(false);

    this->tileLayout->addRows(1);
    this->tileLayout->addColumns(1);
    this->tileLayout->removeRows(1);
    this->tileLayout->removeColumns(1);

    QWidget *central_widget = new QWidget();
    central_widget->setContentsMargins(0, 0, 0, 0);
    central_widget->setLayout(this->tileLayout);

    if (static_layout) {
        this->setCentralWidget(central_widget);
    } else {
        this->scrollArea = new QScrollArea();
        this->scrollArea->setWidgetResizable(true);
        this->scrollArea->setContentsMargins(0, 0, 0, 0);
        this->scrollArea->setWidget(central_widget);

        int vertical_margins = this->tileLayout->contentsMargins().top() + this->tileLayout->contentsMargins().bottom();
        int horizontal_margins = this->tileLayout->contentsMargins().left() + this->tileLayout->contentsMargins().right();

        this->scrollArea->setMinimumHeight(rows * vertical_span + (rows - 1) * spacing + vertical_margins + 2);
        this->scrollArea->setMinimumWidth(columns * horizontal_span + (columns - 1) * spacing + horizontal_margins + 2);
    }

}

TestWindow::~TestWindow()
{
}

void TestWindow::spawnRandomLabel(QLabel &label)
{
    label.setText(this->POSSIBLE_TEXT[this->rand_->bounded(this->POSSIBLE_TEXT.count())]);
    label.setAlignment(Qt::AlignmentFlag::AlignCenter);
    label.setAutoFillBackground(true);
    label.setFont(*this->font_);

    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, this->POSSIBLE_COLOR[this->rand_->bounded(this->POSSIBLE_COLOR.count())]);
    label.setPalette(palette);
}
