#include <QContextMenuEvent>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QSaveFile>
#include <QTimer>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    changeStylesheet();
    setupShortcuts();

//    QFont font;
//    font.setFamily("Courier");
//    font.setFixedPitch(true);
//    font.setPointSize(16);
//    ui->textEdit->setFont(font);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);
    menu.addAction(ui->actionOpen);
    menu.addSeparator();
    menu.addAction(ui->actionSave);
    menu.addAction(ui->actionSave_As);
    menu.addSeparator();
    menu.addAction(ui->actionSettings);
    menu.exec(event->globalPos());
}

void MainWindow::setupShortcuts()
{
    addAction(ui->actionOpen);
    addAction(ui->actionSave);
    addAction(ui->actionSave_As);
    addAction(ui->actionSettings);
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
    QTimer::singleShot(2000, this, &MainWindow::clearStatus);
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
    if (extension == "md" || extension == "markdown") {
        //ui->textEdit->setMarkdown(content);
        ///@todo use syntax highlighting for markdown
        ui->textEdit->setText(content);
    } else {
        ui->textEdit->setText(content);
    }

    setStatus("Opened file " + info.fileName());
    ui->filenameLabel->setText(info.fileName());
    m_currentFilepath = filepath;
}

void MainWindow::on_actionSave_triggered()
{
    if (m_currentFilepath.isEmpty()) {
        on_actionSave_As_triggered();
    } else {
        saveTextFile(m_currentFilepath);
    }
}


void MainWindow::on_actionSave_As_triggered()
{
    QString lastDir;
    if (m_settings.contains("last_save_dir")) {
        lastDir = m_settings.value("last_save_dir").toString();
    } else {
        lastDir = QDir::home().path();
    }

    QString filepath = QFileDialog::getSaveFileName(this, tr("Save File"), lastDir, tr("Text Files (*.txt *.md)"));
    if (!filepath.isEmpty()) {
        saveTextFile(filepath);
        m_settings.setValue("last_save_dir", QFileInfo(filepath).canonicalPath());
    } else {
        qWarning() << Q_FUNC_INFO << "Error: invalid path";
    }
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

    QString filepath = QFileDialog::getOpenFileName(this, tr("Open File"), lastDir, tr("Text Files (*.txt *.md *.markdown)"));
    if (!filepath.isEmpty()) {
        openTextFile(filepath);
        m_settings.setValue("last_open_dir", QFileInfo(filepath).canonicalPath());
    } else {
        qWarning() << Q_FUNC_INFO << "Error: invalid path";
    }
}


void MainWindow::on_textEdit_textChanged()
{

}

