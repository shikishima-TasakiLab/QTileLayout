#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QtWidgets/QtWidgets>
#include <QRandomGenerator>
#include "QTileLayout/qtilelayout.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    TestWindow(QWidget *parent = nullptr);
    ~TestWindow();

protected:
    void spawnRandomLabel(QLabel &label);

    QRandomGenerator *rand_;
    QFont *font_;
    QTileLayout *tileLayout;
    QScrollArea *scrollArea;

    QList<QString> POSSIBLE_TEXT = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
    };
    QList<QColor> POSSIBLE_COLOR = {
        QColor(0xFF, 0xFF, 0x00), QColor(0x00, 0xFF, 0x00),
        QColor(0xFF, 0x00, 0xFF), QColor(0x00, 0xFF, 0xFF),
    };

private:
};
#endif // TESTWINDOW_H
