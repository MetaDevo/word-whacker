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

public slots:
    void updateEditorSize();

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

    void on_hidePreviewButton_clicked();

    void on_actionMarkdown_Preview_triggered();

signals:


private:
    void contextMenuEvent(QContextMenuEvent* event);
    void setupShortcuts();

    static const int STATUS_MILLISECONDS = 2000;
    static const int MIN_BORDER = 50;
    static const int NORMAL_CHARS_PER_LINE = 60;

    Ui::MainWindow* ui;
    QSettings m_settings;
    QThread m_workerThread;

    bool m_docIsMarkdown = false;
    QString m_currentFilepath;
    QFont m_font;
    unsigned int m_charsPerLine = NORMAL_CHARS_PER_LINE;
};
#endif // MAINWINDOW_H
