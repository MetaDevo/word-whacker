#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <QObject>

class Document : public QObject
{
    Q_OBJECT

public:
    Document();

public slots:

signals:

private:
    QString m_filepath;
};

#endif // DOCUMENT_HPP
