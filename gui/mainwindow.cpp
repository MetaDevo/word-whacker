#include <QContextMenuEvent>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontInfo>
#include <QSaveFile>
#include <QScreen>
#include <QTimer>
#include <QWindow>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    changeStylesheet();
    setupShortcuts();

    ui->textEditRich->setVisible(false);
    ui->hidePreviewButton->setVisible(false);
    ui->actionMarkdown_Preview->setVisible(false);

    m_font.setFamily("Courier");
    m_font.setFixedPitch(true);
    m_font.setPointSize(16);
    //m_font.setWeight(QFont::Bold);
    ui->textEdit->setFont(m_font);

    updateEditorSize();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateEditorSize()
{
    int screenWidth = 1920;
    QScreen* s = screen();
    if (s) {
        screenWidth = s->size().width();
    }
    const qreal devicePixelRatio = this->devicePixelRatio();
    QFontInfo fontInfo(m_font);
    qDebug() << "pixel size:" << fontInfo.pixelSize();
    qDebug() << "point size:" << fontInfo.pointSize();
    int textEditWidth = (fontInfo.pixelSize() * m_charsPerLine) / devicePixelRatio;
    int maxWidth = screenWidth - (MIN_BORDER * 2);
    if (textEditWidth > maxWidth) {
        textEditWidth = maxWidth;
    }

    ui->textEdit->setFixedWidth(textEditWidth);
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);
    menu.addAction(ui->actionOpen);
    menu.addSeparator();
    menu.addAction(ui->actionSave);
    menu.addAction(ui->actionSave_As);
    menu.addSeparator();
    menu.addAction(ui->actionFull_Screen);
    menu.addAction(ui->actionMarkdown_Preview);
    menu.addAction(ui->actionSettings);
    menu.exec(event->globalPos());
}

void MainWindow::setupShortcuts()
{
    addAction(ui->actionOpen);
    addAction(ui->actionSave);
    addAction(ui->actionSave_As);
    addAction(ui->actionSettings);
    addAction(ui->actionFull_Screen);
}

void MainWindow::changeStylesheet()
{
    QString stylesheet;
    QString fileName = ":/gui/stylesheets/dark_default.qss";
    qInfo() << "Loading stylesheet " <<  fileName;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        stylesheet = file.readAll();
        setStyleSheet(stylesheet);
    } else {
        qWarning() << "Error opening stylesheet resource.";
        return;
    }
}

void MainWindow::setStatus(const QString& status)
{
    ui->statusLabel->setText(status);
    QTimer::singleShot(STATUS_MILLISECONDS, this, &MainWindow::clearStatus);
}

void MainWindow::clearStatus()
{
    ui->statusLabel->clear();
}

void MainWindow::saveTextFile(const QString& filepath)
{
    QSaveFile file(filepath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString content = ui->textEdit->toPlainText();
        QTextStream out(&file);
        out << content;
        file.commit();
    } else {
        qWarning() << Q_FUNC_INFO << "Error opening file.";
        return;
    }

    QFileInfo info(filepath);
    setStatus("Saved file " + info.fileName());
    ui->filenameLabel->setText(info.fileName());
    m_currentFilepath = filepath;
}

void MainWindow::openTextFile(const QString& filepath)
{
    QString content;
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly)) {
        content = file.readAll();
    } else {
        qWarning() << Q_FUNC_INFO << "Error opening file.";
        return;
    }

    QFileInfo info(filepath);
    QString extension = info.completeSuffix();
    ui->textEdit->setText(content);
    if (extension == "md" || extension == "markdown")
    {
        ui->textEditRich->setMarkdown(content);
        m_docIsMarkdown = true;
        ui->textEditRich->setVisible(true);
        ui->hidePreviewButton->setVisible(true);
        ui->actionMarkdown_Preview->setVisible(true);
    }
    else
    {
        ui->textEditRich->clear();
        m_docIsMarkdown = false;
        ui->textEditRich->setVisible(false);
        ui->hidePreviewButton->setVisible(false);
        ui->actionMarkdown_Preview->setVisible(false);
    }

    setStatus("Opened file " + info.fileName());
    ui->filenameLabel->setText(info.fileName());
    m_currentFilepath = filepath;
}

void MainWindow::on_actionSave_triggered()
{
#ifdef IS_WEBASM
    on_actionSave_As_triggered();
#else
    if (m_currentFilepath.isEmpty()) {
        on_actionSave_As_triggered();
    } else {
        saveTextFile(m_currentFilepath);
    }
#endif
}


void MainWindow::on_actionSave_As_triggered()
{
    QString lastDir;
    if (m_settings.contains("last_save_dir")) {
        lastDir = m_settings.value("last_save_dir").toString();
    } else {
        lastDir = QDir::home().path();
    }

#ifdef IS_WEBASM
    QString nameHint = "untitled.txt";
    if (!m_currentFilepath.isEmpty()) {
        nameHint = QFileInfo(m_currentFilepath).fileName();
    }
    QByteArray contentArray = ui->textEdit->toPlainText().toLocal8Bit();
    QFileDialog::saveFileContent(contentArray, nameHint);
    ///@todo we don't know the file name running via webasm?
    setStatus("Saved file");
    //ui->filenameLabel->setText(info.fileName());
#else
    QString filepath = QFileDialog::getSaveFileName(this, tr("Save File"), lastDir, tr("Text Files (*.txt *.md)"));
    if (!filepath.isEmpty()) {
        saveTextFile(filepath);
        m_settings.setValue("last_save_dir", QFileInfo(filepath).canonicalPath());
    } else {
        qWarning() << Q_FUNC_INFO << "Error: invalid path";
    }
#endif

}


void MainWindow::on_actionSettings_triggered()
{

}


void MainWindow::on_actionOpen_triggered()
{
    QString lastDir;
    if (m_settings.contains("last_open_dir")) {
        lastDir = m_settings.value("last_open_dir").toString();
    } else {
        lastDir = QDir::home().path();
    }

#ifdef IS_WEBASM
    auto fileContentReady = [this](const QString &filepath, const QByteArray &fileContent) {
        if (filepath.isEmpty()) {
            qWarning() << Q_FUNC_INFO << "Error: invalid path";
        } else {
            QString content(fileContent);
            ui->textEdit->setText(content);
            setStatus("Opened file " + filepath);
            ui->filenameLabel->setText(filepath);
            m_currentFilepath = filepath;
            m_settings.setValue("last_open_dir", QFileInfo(filepath).canonicalPath());
        }
    };
    QFileDialog::getOpenFileContent("Text Files (*.txt *.md *.markdown)",  fileContentReady);
#else
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open File"), lastDir, tr("Text Files (*.txt *.md *.markdown)"));
    if (!filepath.isEmpty()) {
        openTextFile(filepath);
        m_settings.setValue("last_open_dir", QFileInfo(filepath).canonicalPath());
    } else {
        qWarning() << Q_FUNC_INFO << "Error: invalid path";
    }
#endif

}

void MainWindow::on_textEdit_textChanged()
{
    if (m_docIsMarkdown) {
        ui->textEditRich->setMarkdown(ui->textEdit->toPlainText());
    }
}

void MainWindow::on_hidePreviewButton_clicked()
{
    ui->textEditRich->setVisible(false);
    ui->hidePreviewButton->setVisible(false);
    ui->textEditRich->clear();
}

void MainWindow::on_actionMarkdown_Preview_triggered()
{
    ui->textEditRich->setVisible(true);
    ui->hidePreviewButton->setVisible(true);
}

void MainWindow::on_actionFull_Screen_triggered(bool checked)
{
    if (checked) {
        setWindowState(Qt::WindowFullScreen);
    } else {
        setWindowState(Qt::WindowMaximized);
    }
}

