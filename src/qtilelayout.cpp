#include "QTileLayout/qtilelayout.h"
#include "QTileLayout/qtilewidget.h"

QTileLayout::QTileLayout(const int rows, const int columns, const int verticalSpan, const int horizontalSpan, const int verticalSpacing, const int horizontalSpacing)
    :   QGridLayout(nullptr)
{
    this->init(rows, columns, verticalSpan, horizontalSpan, verticalSpacing, horizontalSpacing);
}

QTileLayout::QTileLayout(QWidget* const parent, const int rows, const int columns, const int verticalSpan, const int horizontalSpan, const int verticalSpacing, const int horizontalSpacing)
    :   QGridLayout(parent)
{
    this->init(rows, columns, verticalSpan, horizontalSpan, verticalSpacing, horizontalSpacing);
}

void QTileLayout::addWidget(QWidget* const widget, const int row, const int column)
{
    this->addWidget(widget, row, column, 1, 1);
}

void QTileLayout::addWidget(QWidget* const widget, const int row, const int column, const int rowSpan, const int columnSpan){
    assert(!this->widgets_.contains(widget));
    assert(this->isAreaEmpty(row, column, rowSpan, columnSpan));

    QTileWidget *tile = this->tileMap_[row][column];
    this->widgets_.append(widget);
    this->tiles_.append(tile);

    if ((rowSpan > 1) || (columnSpan > 1)) {
        QSet<QPair<int, int>> tileToMerge = {};
        bool ini = true;
        for (int r = row; r < row + rowSpan; r++) {
            for (int c = column; c < column + columnSpan; c++) {
                if (ini == true) {
                    ini = false;
                } else {
                    tileToMerge.insert(QPair<int, int>(r, c));
                }
            }
        }
        this->mergeTiles(tile, row, column, rowSpan, columnSpan, tileToMerge);
    }

    widget->setMouseTracking(true);
    tile->addWidget(widget);
}

void QTileLayout::removeWidget(QWidget* const widget)
{
    assert(this->widgets_.contains(widget));

    qsizetype index = this->widgets_.indexOf(widget);
    QTileWidget *tile = this->tiles_[index];

    int row = tile->getRow();
    int column = tile->getColumn();
    int rowSpan = tile->getRowSpan();
    int columnSpan = tile->getColumnSpan();
    QSet<QPair<int, int>> tileToSplit = {};
    for (int r = row; r < row + rowSpan; r++) {
        for (int c = column; c < column + columnSpan; c++) {
            tileToSplit.insert(QPair<int, int>(r, c));
        }
    }

    widget->setMouseTracking(false);
    this->hardSplitTiles(row, column, tileToSplit);
    this->widgets_.removeAt(index);
    this->tiles_.removeAt(index);
    this->changeTilesColor(this->colorIdle);
}

void QTileLayout::addRows(const int rows)
{
    assert(rows > 0);
    this->setRowStretch(this->rows, 0);

    for (int r = this->rows; r < this->rows + rows; r++) {
        this->tileMap_.append(QList<QTileWidget*>());
        for (int c = 0; c < this->columns; c++) {
            QTileWidget* tile = this->createTile(r, c);
            this->tileMap_[r].append(tile);
        }
    }
    this->rows += rows;
    this->setRowStretch(this->rows, 1);
}

void QTileLayout::addColumns(const int columns)
{
    assert(columns > 0);
    this->setColumnStretch(this->columns, 0);

    for (int r = 0; r < this->rows; r++) {
        for (int c = this->columns; c < this->columns + columns; c++) {
            QTileWidget* tile = this->createTile(r, c);
            this->tileMap_[r].append(tile);
        }
    }
    this->columns += columns;
    this->setColumnStretch(this->columns, 1);
}

void QTileLayout::removeRows(const int rows) {
    assert(this->isAreaEmpty(this->rows - rows, 0, rows, this->columns));

    for (int r = this->rows - rows; r < this->rows; r++) {
        for (int c = 0; c < this->columns; c++) {
            QTileWidget* tile = this->tileMap_[r][c];
            Super::removeWidget(tile);
            tile->deleteLater();
        }
        this->setRowMinimumHeight(r, 0);
        this->setRowStretch(r, 0);
    }
    this->rows -= rows;

    for (int r = 0; r < rows; r++) {
        this->tileMap_.removeLast();
    }
}

void QTileLayout::removeColumns(const int columns) {
    assert(this->isAreaEmpty(0, this->columns - columns, this->rows, columns));

    for (int c = this->columns - columns; c < this->columns; c++) {
        for (int r = 0; r < this->rows; r++) {
            QTileWidget *tile = this->tileMap_[r][c];
            Super::removeWidget(tile);
            tile->deleteLater();
        }
        this->setColumnMinimumWidth(c, 0);
        this->setColumnStretch(c, 0);
    }
    this->columns -= columns;

    for (int r = 0; r < this->rows; r++) {
        QList<QTileWidget* > *tileMapRow = &this->tileMap_[r];
        for (int c = 0; c < columns; c++) {
            tileMapRow->removeLast();
        }
    }
}

void QTileLayout::acceptDragAndDrop(const bool accept)
{
    this->dragAndDrop = accept;
}

void QTileLayout::acceptResizing(const bool accept)
{
    this->resizable = accept;
}

void QTileLayout::setCursorIdle(const Qt::CursorShape &cursor)
{
    this->cursorIdle = cursor;
}

void QTileLayout::setCursorGrab(const Qt::CursorShape &cursor)
{
    this->cursorGrab = cursor;
}

void QTileLayout::setCursorResizeHorizontal(const Qt::CursorShape &cursor)
{
    this->cursorResizeH = cursor;
}

void QTileLayout::setCursorResizeVertical(const Qt::CursorShape &cursor)
{
    this->cursorResizeV = cursor;
}

void QTileLayout::setColorIdle(const QColor &color)
{
    this->colorIdle = color;
    this->changeTilesColor(color);
}

void QTileLayout::setColorResize(const QColor &color)
{
    this->colorResize = color;
}

void QTileLayout::setColorDragAndDrop(const QColor &color)
{
    this->colorDragAndDrop = color;
}

void QTileLayout::setColorEmptyCheck(const QColor &color)
{
    this->colorEmptyCheck = color;
}

int QTileLayout::rowCount(void)
{
    return this->rows;
}

int QTileLayout::columnCount(void)
{
    return this->columns;
}

QRect QTileLayout::tileRect(const int row, const int column)
{
    return this->tileMap_[row][column]->rect();
}

int QTileLayout::rowsMinimumHeight(void)
{
    return this->minVerticalSpan;
}

int QTileLayout::columnsMinimumWidth(void)
{
    return this->minHorizontalSpan;
}

void QTileLayout::setRowsMinimumHeight(const int height)
{
    this->minVerticalSpan = height;
    if (this->minVerticalSpan > this->verticalSpan) {
        this->verticalSpan = this->minVerticalSpan;
        this->updateAllTiles();
    }
}

void QTileLayout::setColumnsMinimumWidth(const int width)
{
    this->minHorizontalSpan = width;
    if (this->minHorizontalSpan > this->horizontalSpan) {
        this->horizontalSpan = this->minHorizontalSpan;
        this->updateAllTiles();
    }
}

void QTileLayout::setRowsHeight(const int height)
{
    assert(this->minVerticalSpan <= height);
    this->verticalSpan = height;
    this->updateAllTiles();
}

void QTileLayout::setColumnsWidth(const int width)
{
    assert(this->minHorizontalSpan <= width);
    this->horizontalSpan = width;
    this->updateAllTiles();
}

void QTileLayout::setVerticalSpacing(const int spacing)
{
    Super::setVerticalSpacing(spacing);
    this->updateAllTiles();
}

void QTileLayout::setHorizontalSpacing(const int spacing)
{
    Super::setHorizontalSpacing(spacing);
    this->updateAllTiles();
}

QUuid QTileLayout::getId(void)
{
    return this->id;
}

void QTileLayout::activateFocus(const bool focus)
{
    this->focus = focus;
}

QList<QWidget*> QTileLayout::widgetList(void)
{
    return this->widgets_;
}

void QTileLayout::linkTileLayout(QTileLayout* const layout)
{
    assert(!this->linkedLayouts.contains(layout->id));
    assert(!layout->linkedLayouts.contains(this->id));
    this->linkedLayouts[layout->id] = layout;
    layout->linkedLayouts[this->id] = this;
}

void QTileLayout::unlinkTileLayout(QTileLayout* const layout)
{
    assert(layout->id != this->id);
    assert(this->linkedLayouts.contains(layout->id));
    assert(layout->linkedLayouts.contains(this->id));
    this->linkedLayouts.remove(layout->id);
    layout->linkedLayouts.remove(this->id);
}

void QTileLayout::setWidgetToDrop(QWidget* const widget)
{
    this->widgetToDrop_ = widget;
}

void QTileLayout::changeTilesColor(const QColor &color)
{
    this->changeTilesColor(color, 0, 0, this->rows, this->columns);
}

void QTileLayout::changeTilesColor(const QColor &color, const int fromRow, const int fromColumn, const int toRow, const int toColumn)
{
    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, color);
    QPalette paletteIdle;
    paletteIdle.setColor(QPalette::ColorRole::Window, this->colorIdle);

    for (int row = fromRow; row < fromRow + toRow; row++) {
        for (int column = fromColumn; column < fromColumn + toColumn; column++) {
            QTileWidget* tile = this->tileMap_[row][column];
            if (tile->isFilled()) {
                tile->changeColor(paletteIdle);
            } else {
                tile->changeColor(palette);
            }
        }
    }
}

QWidget* QTileLayout::getWidgetToDrop(void)
{
    QWidget* widget = this->widgetToDrop_;
    this->widgetToDrop_ = nullptr;
    return widget;
}

void QTileLayout::highlightTiles(QPair<int, int>* const direction, const int row, const int column, const int tileCount)
{
    int tmpRow = row;
    int tmpColumn = column;
    QSet<QPair<int, int> > tilesToMerge;
    bool increase;
    int rowSpan;
    int columnSpan;
    this->getTilesToBeResized(
        this->tileMap_[row][column], direction,
        tmpRow, tmpColumn,
        tileCount, tilesToMerge,
        increase, rowSpan, columnSpan
        );

    if (!tilesToMerge.empty()) {
        this->changeTilesColor(
            this->colorEmptyCheck,
            tmpRow, tmpColumn,
            rowSpan, columnSpan
            );
    }
}

void QTileLayout::resizeTile(QPair<int, int>* const direction, const int row, const int column, const int tileCount)
{
    QTileWidget *tile = this->tileMap_[row][column];
    int tmpRow = row;
    int tmpColumn = column;
    QSet<QPair<int, int> > tilesToMerge;
    bool increase;
    int rowSpan;
    int columnSpan;
    this->getTilesToBeResized(
        tile, direction,
        tmpRow, tmpColumn,
        tileCount, tilesToMerge,
        increase, rowSpan, columnSpan
        );

    if (!tilesToMerge.empty()) {
        if (increase) {
            this->mergeTiles(tile, tmpRow, tmpColumn, rowSpan, columnSpan, tilesToMerge);
        } else {
            this->splitTiles(tile, tmpRow, tmpColumn, rowSpan, columnSpan, tilesToMerge);
        }
        int index = this->tiles_.indexOf(tile);
        QWidget *widget = this->widgets_[index];
        emit this->tileResized(widget, row, column, rowSpan, columnSpan);
    }
}

bool QTileLayout::isAreaEmpty(const int row, const int column, const int rowSpan, const int columnSpan)
{
    if ((row + rowSpan > this->rows) || (column + columnSpan > this->columns) || (row < 0) || (column < 0)) {
        return false;
    } else {
        bool isEmpty = true;
        for (int r = row; r < row + rowSpan; r++) {
            for (int c = column; c < column + columnSpan; c++) {
                isEmpty &= !(this->tileMap_[r][c]->isFilled());
            }
        }
        return isEmpty;
    }
}

bool QTileLayout::isAreaEmpty(const int row, const int column, const int rowSpan, const int columnSpan, const QColor &color)
{
    if (color != nullptr) {
        this->changeTilesColor(color);
    }
    bool isEmpty = this->isAreaEmpty(row, column, rowSpan, columnSpan);
    if (isEmpty == true) {
        this->changeTilesColor(this->colorEmptyCheck, row, column, rowSpan, columnSpan);
    }
    return isEmpty;
}

void QTileLayout::updateGlobalSize(QResizeEvent newSize)
{
    int verticalMargins = this->contentsMargins().top() + this->contentsMargins().bottom();
    int verticalSpan = (newSize.size().height() - (this->rows - 1) * this->verticalSpacing() - verticalMargins) / this->rows;

    int horizontalMargins = this->contentsMargins().left() + this->contentsMargins().right();
    int horizontalSpan = (newSize.size().width() - (this->columns - 1) * this->horizontalSpacing() - horizontalMargins) / this->columns;

    this->verticalSpan = qMax(verticalSpan, this->minVerticalSpan);
    this->horizontalSpan = qMax(horizontalSpan, this->minHorizontalSpan);
    this->updateAllTiles();
}

void QTileLayout::init(const int rows, const int columns, const int verticalSpan, const int horizontalSpan, const int verticalSpacing, const int horizontalSpacing)
{
    Super::setVerticalSpacing(verticalSpacing);
    Super::setHorizontalSpacing(horizontalSpacing);

    this->rows = rows;
    this->columns = columns;
    this->verticalSpan = verticalSpan;
    this->horizontalSpan = horizontalSpan;
    this->minVerticalSpan = verticalSpan;
    this->minHorizontalSpan = horizontalSpan;

    this->colorDragAndDrop = QColor(211, 211, 211);
    this->colorIdle = QColor(240, 240, 240);
    this->colorResize = QColor(211, 211, 211);
    this->colorEmptyCheck = QColor(150, 150, 150);

    this->cursorIdle = Qt::CursorShape::ArrowCursor;
    this->cursorGrab = Qt::CursorShape::OpenHandCursor;
    this->cursorResizeV = Qt::CursorShape::SizeVerCursor;
    this->cursorResizeH = Qt::CursorShape::SizeHorCursor;

    this->dragAndDrop = true;
    this->resizable = true;
    this->focus = false;
    this->widgetToDrop_ = nullptr;
    this->widgets_ = {};
    this->tiles_ = {};
    this->id = QUuid::createUuid();
    this->linkedLayouts = {};
    this->linkedLayouts.insert(this->id, this);

    this->setRowStretch(this->rows, 1);
    this->setColumnStretch(this->columns, 1);
    this->initTileMap();
}

void QTileLayout::initTileMap(void)
{
    this->tileMap_ = {};

    for (int row = 0; row < this->rows; row++) {
        this->tileMap_.append(QList<QTileWidget*>());
        for (int column = 0; column < this->columns; column++) {
            this->tileMap_[row].append(this->createTile(row, column));
        }
    }
}

QTileWidget* QTileLayout::createTile(const int row, const int column)
{
    return this->createTile(row, column, 1, 1);
}

QTileWidget* QTileLayout::createTile(const int row, const int column, const bool updateTileMap)
{
    return this->createTile(row, column, 1, 1, updateTileMap);
}

QTileWidget* QTileLayout::createTile(const int row, const int column, const int rowSpan, const int columnSpan)
{
    return this->createTile(row, column, rowSpan, columnSpan, false);
}

QTileWidget* QTileLayout::createTile(const int row, const int column, const int rowSpan, const int columnSpan, const bool updateTileMap)
{
    QTileWidget* tile = new QTileWidget(this, row, column, rowSpan, columnSpan, this->verticalSpan, this->horizontalSpan);
    Super::addWidget(tile, row, column, rowSpan, columnSpan);

    if (updateTileMap == true) {
        for (int r = row; r < row + rowSpan; r++) {
            for (int c = column; c < column + columnSpan; c++) {
                this->tileMap_[r][c] = tile;
            }
        }
    }
    return tile;
}

void QTileLayout::getTilesToBeResized(QTileWidget* const tile, QPair<int, int>* const direction, int &row, int &column, const int &tileCount, QSet<QPair<int, int> > &tilesToMerge, bool &increase, int &rowSpan, int &columnSpan)
{
    rowSpan = tile->getRowSpan();
    columnSpan = tile->getColumnSpan();
    increase = (tileCount * (direction->first + direction->second) > 0);

    int tileAvailableCount = 0;
    if (increase) {
        this->getTilesToMerge(
            direction,
            row, column,
            tileCount, tileAvailableCount,
            tilesToMerge
            );
    } else {
        this->getTilesToSplit(
            direction,
            row, column,
            tileCount, tileAvailableCount,
            tilesToMerge
            );
    }

    rowSpan += tileAvailableCount * direction->second;
    row += (direction->second == -1)? tileAvailableCount : 0;
    columnSpan += tileAvailableCount * direction->first;
    column += (direction->first == -1)? tileAvailableCount : 0;
}

void QTileLayout::getTilesToMerge(QPair<int, int>* const direction, const int &row, const int &column, const int &tileCount, int &tileAvailableCount, QSet<QPair<int, int> > &tilesToMerge)
{
    QTileWidget *tile = this->tileMap_[row][column];
    int rowSpan = tile->getRowSpan();
    int columnSpan = tile->getColumnSpan();
    tileAvailableCount = 0;
    int swX = (direction->first != 0)? 1 : 0;
    int swY = (direction->second != 0)? 1 : 0;

    int tmpTileCount = (direction->first + direction->second == -1)?
                           qMax<int>(tileCount, -column * swX - row * swY) :
                           qMin<int>(tileCount, (this->columns - column - columnSpan) * swX + (this->rows - row - rowSpan) * swY);

    if (direction->first != 0) {
        int swP = (direction->first == 1)? 1 : 0;
        int swN = (direction->first == -1)? 1 : 0;

        for (int c = 0; c < tmpTileCount * direction->first; c++) {
            QSet<QPair<int, int> > tilesToCheck;
            int columnDelta = (columnSpan + c) * swP + (-c - 1) * swN;
            for (int r = 0; r < rowSpan; r++) {
                int rowCheck = row + r;
                int columnCheck = column + columnDelta;
                if (this->tileMap_[rowCheck][columnCheck]->isFilled()) {
                    return;
                }
                tilesToCheck.insert(QPair<int, int>(rowCheck, columnCheck));
            }
            tileAvailableCount += direction->first;
            tilesToMerge.unite(tilesToCheck);
        }
    } else {
        int swP = (direction->second == 1)? 1 : 0;
        int swN = (direction->second == -1)? 1 : 0;

        for (int r = 0; r < tileCount * direction->second; r++) {
            QSet<QPair<int, int> > tilesToCheck;
            int rowDelta = (rowSpan + r) * swP + (-r - 1) * swN;
            for (int c = 0; c < columnSpan; c++) {
                int rowCheck = row + rowDelta;
                int columnCheck = column + c;
                if (this->tileMap_[rowCheck][columnCheck]->isFilled()) {
                    return;
                }
                tilesToCheck.insert(QPair<int, int>(rowCheck, columnCheck));
            }
            tileAvailableCount += direction->second;
            tilesToMerge.unite(tilesToCheck);
        }
    }
}

void QTileLayout::getTilesToSplit(QPair<int, int>* const direction, const int &row, const int &column, const int &tileCount, int &tileAvailableCount, QSet<QPair<int, int> > &tilesToSplit)
{
    QTileWidget *tile = this->tileMap_[row][column];
    int rowSpan = tile->getRowSpan();
    int columnSpan = tile->getColumnSpan();

    int swX0 = (direction->first != 0)? 1 : 0;
    int swY0 = (direction->second != 0)? 1 : 0;
    int swX1 = (direction->first == 1)? 1 : 0;
    int swY1 = (direction->second == 1)? 1 : 0;

    if (-tileCount * (direction->first + direction->second) < columnSpan * swX0 + rowSpan * swY0) {
        tileAvailableCount = tileCount;
    } else {
        tileAvailableCount = (1 - columnSpan) * direction->first + (1 - rowSpan) * direction->second;
    }
    for (int r = 0; r < -tileCount * direction->second + rowSpan * swX0; r++) {
        for (int c = 0; c < -tileCount * direction->first + columnSpan * swY0; c++) {
            tilesToSplit.insert(QPair<int, int>(
                row + r + (rowSpan - 2 * r - 1) * swY1,
                column + c + (columnSpan - 2 * c - 1) * swX1
                ));
        }
    }
}

void QTileLayout::mergeTiles(QTileWidget* const tile, const int row, const int column, const int rowSpan, const int columnSpan, const QSet<QPair<int, int> > &tilesToMerge)
{
    QSetIterator<QPair<int, int> > itr(tilesToMerge);
    while (itr.hasNext()) {
        QPair<int, int> xy = itr.next();
        QTileWidget *xTile = this->tileMap_[xy.first][xy.second];
        Super::removeWidget(xTile);
        xTile->deleteLater();
        this->tileMap_[xy.first][xy.second] = tile;
    }

    Super::removeWidget(tile);
    Super::addWidget(tile, row, column, rowSpan, columnSpan);
    tile->updateSize(row, column, rowSpan, columnSpan);
}

void QTileLayout::splitTiles(QTileWidget* const tile, const int row, const int column, const int rowSpan, const int columnSpan, const QSet<QPair<int, int> > &tilesToSplit)
{
    QSetIterator<QPair<int, int> > itr(tilesToSplit);
    while (itr.hasNext()) {
        QPair<int, int> xy = itr.next();
        this->createTile(xy.first, xy.second, true);
    }
    Super::removeWidget(tile);
    Super::addWidget(tile, row, column, rowSpan, columnSpan);
    tile->updateSize(row, column, rowSpan, columnSpan);
}

QTileWidget* QTileLayout::hardSplitTiles(const int row, const int column, const QSet<QPair<int, int>> &tileToSplit)
{
    assert(tileToSplit.contains(QPair<int, int>(row, column)));

    QSet<QTileWidget*> tileToRecycle;
    QSetIterator<QPair<int, int> > splitItr(tileToSplit);
    while (splitItr.hasNext()) {
        QPair<int, int> xy = splitItr.next();
        tileToRecycle.insert(this->tileMap_[xy.first][xy.second]);
        this->createTile(xy.first, xy.second, true);
    }

    QSetIterator<QTileWidget*> recycleItr(tileToRecycle);
    while (recycleItr.hasNext()) {
        QTileWidget *rmTile = recycleItr.next();
        Super::removeWidget(rmTile);
        rmTile->deleteLater();
    }

    QTileWidget* tile = this->tileMap_[row][column];
    Super::addWidget(tile, row, column);
    return tile;
}

void QTileLayout::updateAllTiles(void)
{
    QListIterator<QList<QTileWidget*> > tileMapRowItr(this->tileMap_);
    while (tileMapRowItr.hasNext()) {
        QListIterator<QTileWidget*> tileMapRowColItr(tileMapRowItr.next());
        while (tileMapRowColItr.hasNext()) {
            tileMapRowColItr.next()->updateSize(this->verticalSpan, this->horizontalSpan);
        }
    }
}
