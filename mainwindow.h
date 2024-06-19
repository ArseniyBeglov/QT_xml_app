#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDomDocument>
#include <QMessageBox>
#include <QInputDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString filename_;

private slots:
    void on_chooseFileButton_clicked();

    void on_deleteItemButton_clicked();

    void on_addItemButton_clicked();

    void on_saveFileButton_clicked();

private:
    Ui::MainWindow *ui;
    QDomDocument doc;
    void loadXmlFile(const QString &fileName);
    QString formatDomElement(const QDomElement &element, int indent = 0);
};
#endif // MAINWINDOW_H
