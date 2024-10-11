#ifndef QTILELAYOUT_H
#define QTILELAYOUT_H

#include "QTileLayout_global.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QWidget>
#include <QtCore/QUuid>
#include <QtGui/QResizeEvent>

class QTileWidget;

class QTILELAYOUT_EXPORT QTileLayout : public QGridLayout
{
    Q_OBJECT
public:
    QTileLayout(const int rows, const int columns, const int verticalSpan = 128, const int horizontalSpan = 128, const int verticalSpacing = 5, const int horizontalSpacing = 5);
    QTileLayout(QWidget* const parent, const int rows, const int columns, const int verticalSpan = 128, const int horizontalSpan = 128, const int verticalSpacing = 5, const int horizontalSpacing = 5);

    void addWidget(QWidget* const widget, const int row, const int column);
    void addWidget(QWidget* const widget, const int row, const int column, const int rowSpan, const int columnSpan);
    void removeWidget(QWidget* const widget);
    void addRows(const int rows);
    void addColumns(const int columns);
    void removeRows(const int rows);
    void removeColumns(const int columns);
    void acceptDragAndDrop(const bool accept);
    void acceptResizing(const bool accept);
    void setCursorIdle(const Qt::CursorShape &cursor);
    void setCursorGrab(const Qt::CursorShape &cursor);
    void setCursorResizeHorizontal(const Qt::CursorShape &cursor);
    void setCursorResizeVertical(const Qt::CursorShape &cursor);
    void setColorIdle(const QColor &color);
    void setColorResize(const QColor &color);
    void setColorDragAndDrop(const QColor &color);
    void setColorEmptyCheck(const QColor &color);
    int rowCount(void);
    int columnCount(void);
    QRect tileRect(const int row, const int column);
    int rowsMinimumHeight(void);
    int columnsMinimumWidth(void);
    void setRowsMinimumHeight(const int height);
    void setColumnsMinimumWidth(const int width);
    void setRowsHeight(const int height);
    void setColumnsWidth(const int width);
    void setVerticalSpacing(const int spacing);
    void setHorizontalSpacing(const int spacing);
    QUuid getId(void);
    void activateFocus(const bool focus);
    QList<QWidget*> widgetList(void);
    void linkTileLayout(QTileLayout* const layout);
    void unlinkTileLayout(QTileLayout* const layout);
    void setWidgetToDrop(QWidget* const widget);
    void changeTilesColor(const QColor &color);
    void changeTilesColor(const QColor &color, const int fromRow, const int fromColumn, const int toRow, const int toColumn);
    QWidget* getWidgetToDrop(void);
    void highlightTiles(QPair<int, int>* const direction, const int row, const int column, const int tileCount);
    void resizeTile(QPair<int, int>* const direction, const int row, const int column, const int tileCount);
    bool isAreaEmpty(const int row, const int column, const int rowSpan, const int columnSpan);
    bool isAreaEmpty(const int row, const int column, const int rowSpan, const int columnSpan, const QColor &color);
    void updateGlobalSize(QResizeEvent newSize);

    int rows;
    int columns;
    int verticalSpan;
    int horizontalSpan;
    int minVerticalSpan;
    int minHorizontalSpan;

    QColor colorDragAndDrop;
    QColor colorIdle;
    QColor colorResize;
    QColor colorEmptyCheck;

    bool dragAndDrop = true;
    bool resizable = true;
    bool focus = false;
    QUuid id;
    QMap<QUuid, QTileLayout*> linkedLayouts;

    Qt::CursorShape cursorIdle = Qt::CursorShape::ArrowCursor;
    Qt::CursorShape cursorGrab = Qt::CursorShape::OpenHandCursor;
    Qt::CursorShape cursorResizeV = Qt::CursorShape::SizeVerCursor;
    Qt::CursorShape cursorResizeH = Qt::CursorShape::SizeHorCursor;

signals:
    void tileResized(QWidget const *widget, const int row, const int column, const int rowSpan, const int columnSpan);
    void tileMoved(QWidget const *widget, const QUuid fromLayoutId, const QUuid toLayoutId, const int fromRow, const int fromColumn, const int toRow, const int toColumn);

protected:
    void init(const int rows, const int columns, const int verticalSpan, const int horizontalSpan, const int verticalSpacing, const int horizontalSpacing);
    void initTileMap(void);
    QTileWidget* createTile(const int row, const int column);
    QTileWidget* createTile(const int row, const int column, const bool updateTileMap);
    QTileWidget* createTile(const int row, const int column, const int rowSpan, const int columnSpan);
    QTileWidget* createTile(const int row, const int column, const int rowSpan, const int columnSpan, const bool updateTileMap);
    void getTilesToBeResized(QTileWidget* const tile, QPair<int, int>* const direction, int &row, int &column, const int &tileCount, QSet<QPair<int, int> > &tilesToMerge, bool &increase, int &rowSpan, int &columnSpan);
    void getTilesToMerge(QPair<int, int>* const direction, const int &row, const int &column, const int &tileCount, int &tileAvailableCount, QSet<QPair<int, int> > &tilesToMerge);
    void getTilesToSplit(QPair<int, int>* const direction, const int &row, const int &column, const int &tileCount, int &tileAvailableCount, QSet<QPair<int, int> > &tilesToSplit);
    void mergeTiles(QTileWidget* const tile, const int row, const int column, const int rowSpan, const int columnSpan, const QSet<QPair<int, int> > &tilesToMerge);
    void splitTiles(QTileWidget* const tile, const int row, const int column, const int rowSpan, const int columnSpan, const QSet<QPair<int, int> > &tilesToSplit);
    QTileWidget* hardSplitTiles(const int row, const int column, const QSet<QPair<int, int>> &tileToSplit);
    void updateAllTiles(void);

    QWidget* widgetToDrop_;
    QList<QList<QTileWidget*> > tileMap_;
    QList<QWidget*> widgets_;
    QList<QTileWidget*> tiles_;

private:
    typedef QGridLayout Super;
};

#endif // QTILELAYOUT_H
