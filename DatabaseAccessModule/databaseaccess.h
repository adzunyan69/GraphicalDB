#ifndef DATABASEACCESS_H
#define DATABASEACCESS_H

#include <QObject>
#include <QtSql>

class DatabaseAccess : public QObject
{
    Q_OBJECT

private:

    QSqlDatabase db;

    bool DBExist() const;


public:

    explicit DatabaseAccess(QObject *parent = nullptr);
    ~DatabaseAccess();

    bool openDatabase(QString databasePath);
    QSqlQuery execQueryString(const QString &query);
    QSqlQuery execQueryFile(const QString &filePath);
    QSqlQuery execQueryStringBind(const QString &query, const QMap<QString, QVariant> &bindValues);
    QSqlQuery execQueryFileBind(const QString filePath, const QMap<QString, QVariant> &bindValues);

    QString lastError() { return db.lastError().text(); }

signals:
    void error(QString msg);

};

#endif // DATABASEACCESS_H
