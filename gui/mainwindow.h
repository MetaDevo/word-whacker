#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void changeStylesheet();
    void setStatus(const QString& status);
    void clearStatus();
    void saveTextFile(const QString& filepath);
    void openTextFile(const QString& filepath);

    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionSettings_triggered();

    void on_actionOpen_triggered();

    void on_textEdit_textChanged();

signals:


private:
    void contextMenuEvent(QContextMenuEvent* event);
    void setupShortcuts();

    Ui::MainWindow* ui;
    QSettings m_settings;
    QThread m_workerThread;

    QString m_currentFilepath;
};
#endif // MAINWINDOW_H
