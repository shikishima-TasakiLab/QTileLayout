#include "QTileLayout/qtilelayout.h"
#include "QTileLayout/qtilewidget.h"


QTileWidget::QTileWidget(QTileLayout* const tileLayout, const int row, const int column, const int rowSpan, const int columnSpan, const int verticalSpan, const int horizontalSpan, QWidget* const parent)
    : QWidget{parent}
{
    this->tileLayout_ = tileLayout;
    this->originTileLayout_ = this->tileLayout_;
    this->row_ = row;
    this->column_ = column;
    this->rowSpan_ = rowSpan;
    this->columnSpan_ = columnSpan;
    this->verticalSpan_ = verticalSpan;
    this->horizontalSpan_ = horizontalSpan;
    this->resizeMargin_ = 5;

    this->filled_ = false;
    this->widget_ = nullptr;
    this->lock_ = nullptr;
    this->dragInProcess_ = false;
    this->currentTileCount_ = 0;

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    this->mouseMovePos_ = nullptr;
    this->updateSizeLimit();
    this->setAcceptDrops(true);
    this->setMouseTracking(true);
}

void QTileWidget::addWidget(QWidget* const widget) {
    this->layout()->addWidget(widget);
    this->widget_ = widget;
    this->filled_ = true;
}

void QTileWidget::updateSize(const int verticalSpan, const int horizontalSpan) {
    this->verticalSpan_ = verticalSpan;
    this->horizontalSpan_ = horizontalSpan;
    this->updateSizeLimit();
}

void QTileWidget::updateSize(const int row, const int column, const int rowSpan, const int columnSpan) {
    this->row_ = row;
    this->column_ = column;
    this->rowSpan_ = rowSpan;
    this->columnSpan_ = columnSpan;
    this->updateSizeLimit();
}

void QTileWidget::updateSize(const int row, const int column, const int rowSpan, const int columnSpan, const int verticalSpan, const int horizontalSpan) {
    this->verticalSpan_ = verticalSpan;
    this->horizontalSpan_ = horizontalSpan;
    this->updateSize(row, column, rowSpan, columnSpan);
}

void QTileWidget::changeColor(const QPalette &palette)
{
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}

int QTileWidget::getRow(void)
{
    return this->row_;
}

int QTileWidget::getColumn(void)
{
    return this->column_;
}

int QTileWidget::getRowSpan(void)
{
    return this->rowSpan_;
}

int QTileWidget::getColumnSpan(void)
{
    return this->columnSpan_;
}

bool QTileWidget::isFilled(void)
{
    return this->filled_;
}

void QTileWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::MouseButton::LeftButton) {
        if (this->mouseMovePos_ != nullptr && !this->dragInProcess_ && this->lock_ == nullptr) {
            QPointF globalPos = event->globalPosition();
            QPointF lastPos = this->mapToGlobal(QPointF(*this->mouseMovePos_));
            QPointF diff = globalPos - lastPos;

            if (
                diff.manhattanLength() > 3 &&
                this->filled_ &&
                this->tileLayout_->dragAndDrop
                ) {
                QDrag drag = QDrag(this);
                this->prepareDropData(event, drag);
                this->dragAndDropProcess(drag);

                QMapIterator<QUuid, QTileLayout*> itr(this->tileLayout_->linkedLayouts);
                while (itr.hasNext()) {
                    auto item = itr.next();
                    item.value()->changeTilesColor(this->tileLayout_->colorIdle);
                }
                if (this->filled_ && this->tileLayout_->focus) {
                    this->widget_->setFocus();
                }
            }
        }
    }

    QPoint position = event->position().toPoint();

    if (!this->filled_) {
        this->setCursor(this->tileLayout_->cursorIdle);
    } else if (this->lock_ == nullptr) {
        bool leftCondition = (0 <= position.x()) && (position.x() < this->resizeMargin_);
        bool rightCondition = (this->width() >= position.x()) && (position.x() > this->width() - this->resizeMargin_);
        bool topCondition = (0 <= position.y()) && (position.y() < this->resizeMargin_);
        bool bottomCondition = (this->height() >= position.y()) && (position.y() > this->height() - this->resizeMargin_);

        if ((leftCondition || rightCondition) && this->tileLayout_->resizable) {
            this->setCursor(this->tileLayout_->cursorResizeH);
        } else if ((topCondition|| bottomCondition) && this->tileLayout_->resizable) {
            this->setCursor(this->tileLayout_->cursorResizeV);
        } else if (this->tileLayout_->dragAndDrop && this->rect().contains(position)) {
            this->setCursor(this->tileLayout_->cursorGrab);
        } else {
            this->setCursor(this->tileLayout_->cursorIdle);
        }
    } else {
        int tileCount = this->getResizeTileCount(position);
        if (tileCount != this->currentTileCount_) {
            this->currentTileCount_ = tileCount;
            this->tileLayout_->changeTilesColor(this->tileLayout_->colorResize);
            this->tileLayout_->highlightTiles(this->lock_, this->row_, this->column_, tileCount);
        }
    }

    Super::mouseMoveEvent(event);
}

void QTileWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::MouseButton::LeftButton) {
        this->mouseMovePos_ = new QPoint(event->position().toPoint());

        if (
            (this->mouseMovePos_->x() < this->resizeMargin_) &&
            this->tileLayout_->resizable
            ) {
            this->lock_ = new QPair<int, int>(-1, 0);
        } else if (
            (this->mouseMovePos_->x() > this->width() - this->resizeMargin_) &&
            this->tileLayout_->resizable
            ) {
            this->lock_ = new QPair<int, int>(1, 0);
        } else if (
            (this->mouseMovePos_->y() < this->resizeMargin_) &&
            this->tileLayout_->resizable
            ) {
            this->lock_ = new QPair<int, int>(0, -1);
        } else if (
            (this->mouseMovePos_->y() > this->height() - this->resizeMargin_) &&
            this->tileLayout_->resizable
            ) {
            this->lock_ = new QPair<int, int>(0, 1);
        }
        if (this->lock_ != nullptr) {
            this->tileLayout_->changeTilesColor(this->tileLayout_->colorResize);
        }
    } else {
        this->mouseMovePos_ = nullptr;
    }

    return Super::mousePressEvent(event);
}

void QTileWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (this->lock_ == nullptr) {
        return Super::mouseReleaseEvent(event);
    }

    int tileCount = this->getResizeTileCount(event->position().toPoint());

    this->tileLayout_->resizeTile(this->lock_, this->row_, this->column_, tileCount);
    this->tileLayout_->changeTilesColor(this->tileLayout_->colorIdle);
    this->currentTileCount_ = 0;
    this->lock_ = nullptr;

    return Super::mouseReleaseEvent(event);
}

void QTileWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (
        this->tileLayout_->dragAndDrop &&
        event->mimeData()->hasFormat(this->MIME_TYPE_TILE_DATA) &&
        this->isDropPossible(event)
        ) {
        event->acceptProposedAction();
    }
}

void QTileWidget::dropEvent(QDropEvent *event)
{
    QJsonDocument dropData = QJsonDocument::fromJson(event->mimeData()->data(this->MIME_TYPE_TILE_DATA));
    QVariantMap dropDataVariantMap = dropData.toVariant().toMap();

    QUuid id = dropDataVariantMap[this->MIME_KEY_ID].toUuid();
    int row = dropDataVariantMap[this->MIME_KEY_ROW].toInt();
    int column = dropDataVariantMap[this->MIME_KEY_COLUMN].toInt();
    int rowSpan = dropDataVariantMap[this->MIME_KEY_ROW_SPAN].toInt();
    int columnSpan = dropDataVariantMap[this->MIME_KEY_COLUMN_SPAN].toInt();
    int rowOffset = dropDataVariantMap[this->MIME_KEY_ROW_OFFSET].toInt();
    int columnOffset = dropDataVariantMap[this->MIME_KEY_COLUMN_OFFSET].toInt();

    QWidget* widget = this->originTileLayout_->getWidgetToDrop();

    this->tileLayout_->addWidget(
        widget,
        this->row_ - rowOffset, this->column_ - columnOffset,
        rowSpan, columnSpan
        );
    emit this->tileLayout_->tileMoved(
        widget, id, this->tileLayout_->id,
        row, column,
        this->row_ - rowOffset, this->column_ - columnOffset
        );
    event->acceptProposedAction();
}

void QTileWidget::updateSizeLimit(void)
{
    this->setFixedSize(
        this->columnSpan_ * this->horizontalSpan_ + (this->columnSpan_ - 1) * this->tileLayout_->horizontalSpacing(),
        this->rowSpan_ * this->verticalSpan_ + (this->rowSpan_ - 1) * this->tileLayout_->verticalSpacing()
        );
}

void QTileWidget::prepareDropData(QMouseEvent* const event, QDrag &drag)
{
    QMimeData *dropData = new QMimeData();
    QVariantMap data = {
        {this->MIME_KEY_ID, this->tileLayout_->id},
        {this->MIME_KEY_ROW, this->row_},
        {this->MIME_KEY_COLUMN, this->column_},
        {this->MIME_KEY_ROW_SPAN, this->rowSpan_},
        {this->MIME_KEY_COLUMN_SPAN, this->columnSpan_},
        {this->MIME_KEY_ROW_OFFSET, event->position().toPoint().y() / (this->verticalSpan_ + this->tileLayout_->verticalSpacing())},
        {this->MIME_KEY_COLUMN_OFFSET, event->position().toPoint().x() / (this->horizontalSpan_ + this->tileLayout_->horizontalSpacing())}
    };
    QJsonDocument dataToText = QJsonDocument::fromVariant(data);
    dropData->setData(this->MIME_TYPE_TILE_DATA, QByteArray(dataToText.toJson()));
    QPixmap dragIcon = this->widget_->grab();

    drag.setPixmap(dragIcon);
    drag.setMimeData(dropData);
    drag.setHotSpot(event->position().toPoint() - this->rect().topLeft());
}

void QTileWidget::dragAndDropProcess(QDrag &drag)
{
    this->dragInProcess_ = true;
    int prevRowSpan = this->rowSpan_;
    int prevColumnSpan = this->columnSpan_;

    this->tileLayout_->setWidgetToDrop(this->widget_);
    this->widget_->clearFocus();
    this->tileLayout_->removeWidget(this->widget_);
    this->setVisible(false);

    QMapIterator<QUuid, QTileLayout*> itr(this->tileLayout_->linkedLayouts);
    while (itr.hasNext()) {
        auto item = itr.next();
        if (item.value()->dragAndDrop) {
            item.value()->changeTilesColor(this->tileLayout_->colorDragAndDrop);
        }
    }

    if (drag.exec() != Qt::DropAction::MoveAction) {
        this->removeWidget();
        QWidget *widget = this->tileLayout_->getWidgetToDrop();
        this->tileLayout_->addWidget(widget, this->row_, this->column_, prevRowSpan, prevColumnSpan);
        if (this->tileLayout_->focus) {
            widget->setFocus();
        }
    }

    this->originTileLayout_ = this->tileLayout_;
    this->setVisible(true);
    this->dragInProcess_ = false;
}

void QTileWidget::removeWidget(void)
{
    this->layout()->removeWidget(this->widget_);
    this->widget_ = nullptr;
    this->filled_ = false;
}

int QTileWidget::getResizeTileCount(const QPoint &position)
{
    return this->getResizeTileCount(position.x(), position.y());
}

int QTileWidget::getResizeTileCount(const int x, const int y)
{
    int swX = (this->lock_->first != 0)? 1 : 0;
    int swY = (this->lock_->second != 0)? 1 : 0;
    int swP = (this->lock_->first + this->lock_->second == 1)? 1 : 0;
    int span = this->horizontalSpan_ * swX + this->verticalSpan_ * swY;
    int tileSpan = this->columnSpan_ * swX + this->rowSpan_ * swY;
    int spacing = this->tileLayout_->verticalSpacing() * swX + this->tileLayout_->horizontalSpacing() * swY;

    assert(span + spacing != 0);
    int tileCount = (x * swX + y * swY + span / 2 - span * tileSpan * swP);
    int swN = (tileCount < 0)? 1 : 0;
    tileCount /= (span + spacing);
    return tileCount - swN;
}

bool QTileWidget::isDropPossible(QDragEnterEvent *event)
{
    QJsonParseError *jsonError = new QJsonParseError();
    QJsonDocument dropData = QJsonDocument::fromJson(event->mimeData()->data(this->MIME_TYPE_TILE_DATA), jsonError);

    if (jsonError->error != QJsonParseError::NoError) return false;

    QVariant dropDataVariant = dropData.toVariant();

    if (!dropDataVariant.canConvert<QVariantMap>()) return false;

    QVariantMap dropDataVariantMap = dropDataVariant.toMap();

    if (!this->canConvert<QUuid>(dropDataVariantMap, this->MIME_KEY_ID)) return false;
    if (!this->canConvert<int>(dropDataVariantMap, this->MIME_KEY_ROW)) return false;
    if (!this->canConvert<int>(dropDataVariantMap, this->MIME_KEY_COLUMN)) return false;
    if (!this->canConvert<int>(dropDataVariantMap, this->MIME_KEY_ROW_SPAN)) return false;
    if (!this->canConvert<int>(dropDataVariantMap, this->MIME_KEY_COLUMN_SPAN)) return false;
    if (!this->canConvert<int>(dropDataVariantMap, this->MIME_KEY_ROW_OFFSET)) return false;
    if (!this->canConvert<int>(dropDataVariantMap, this->MIME_KEY_COLUMN_OFFSET)) return false;

    QUuid id = dropDataVariantMap[this->MIME_KEY_ID].toUuid();

    if (!this->tileLayout_->linkedLayouts.contains(id)) return false;

    this->originTileLayout_ = this->tileLayout_->linkedLayouts[id];

    QMapIterator<QUuid, QTileLayout*> itr(this->originTileLayout_->linkedLayouts);
    while (itr.hasNext()) {
        auto item = itr.next();
        if (item.value()->dragAndDrop) {
            item.value()->changeTilesColor(this->tileLayout_->colorDragAndDrop);
        }
    }

    return this->tileLayout_->isAreaEmpty(
        this->row_ - dropDataVariantMap[this->MIME_KEY_ROW_OFFSET].toInt(),
        this->column_ - dropDataVariantMap[this->MIME_KEY_COLUMN_OFFSET].toInt(),
        dropDataVariantMap[this->MIME_KEY_ROW_SPAN].toInt(),
        dropDataVariantMap[this->MIME_KEY_COLUMN_SPAN].toInt(),
        this->tileLayout_->colorDragAndDrop
        );
}

template <typename T>
bool QTileWidget::canConvert(const QVariantMap &map, const QString &key)
{
    if (!map.contains(key)) return false;
    if (!map[key].canConvert<T>()) return false;
    return true;
}
