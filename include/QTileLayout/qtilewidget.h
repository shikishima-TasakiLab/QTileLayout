#ifndef QTILEWIDGET_H
#define QTILEWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QDrag>
#include <QtCore/QMimeData>
#include <QtCore/QJsonDocument>

class QTileLayout;

class QTileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QTileWidget(QTileLayout* const tileLayout, const int row, const int column, const int rowSpan, const int columnSpan, const int verticalSpan, const int horizontalSpan, QWidget* const parent = nullptr);
    void addWidget(QWidget* const widget);
    void updateSize(const int verticalSpan, const int horizontalSpan);
    void updateSize(const int row, const int column, const int rowSpan, const int columnSpan);
    void updateSize(const int row, const int column, const int rowSpan, const int columnSpan, const int verticalSpan, const int horizontalSpan);
    void changeColor(const QPalette &palette);
    int getRow(void);
    int getColumn(void);
    int getRowSpan(void);
    int getColumnSpan(void);
    bool isFilled(void);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

protected:
    void updateSizeLimit(void);
    void prepareDropData(QMouseEvent* const event, QDrag &drag);
    void dragAndDropProcess(QDrag &drag);
    void removeWidget(void);
    int getResizeTileCount(const QPoint &position);
    int getResizeTileCount(const int x, const int y);
    bool isDropPossible(QDragEnterEvent *event);
    template <typename T>
    bool canConvert(const QVariantMap &map, const QString &key);

    QTileLayout* tileLayout_;
    QTileLayout* originTileLayout_;
    int row_;
    int column_;
    int rowSpan_;
    int columnSpan_;
    int verticalSpan_;
    int horizontalSpan_;
    int resizeMargin_;
    bool filled_;
    QWidget* widget_;
    QPair<int, int>* lock_;
    bool dragInProcess_;
    int currentTileCount_;
    QPoint* mouseMovePos_;

    const QString MIME_TYPE_TILE_DATA = "TileData";
    const QString MIME_KEY_ID = "id";
    const QString MIME_KEY_ROW = "row";
    const QString MIME_KEY_COLUMN = "column";
    const QString MIME_KEY_ROW_SPAN = "rowSpan";
    const QString MIME_KEY_COLUMN_SPAN = "columnSpan";
    const QString MIME_KEY_ROW_OFFSET = "rowOffset";
    const QString MIME_KEY_COLUMN_OFFSET = "columnOffset";

signals:

private:
    typedef QWidget Super;
};

#endif // QTILEWIDGET_H
