#include "databaseaccess.h"

DatabaseAccess::DatabaseAccess(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database();
    if(db.isValid() == false)
    {
        qDebug() << "Add database SQLITE";
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

}

DatabaseAccess::~DatabaseAccess()
{
    qDebug() << "Closing database";
    db.close();
}

bool DatabaseAccess::openDatabase(QString databasePath)
{
    if(db.isValid() == false)
    {
        qDebug() << "Database is not valid";
        return false;
    }

    db.setDatabaseName(databasePath);
    return db.open();
}

bool DatabaseAccess::DBExist() const
{
    return db.isValid();
}

QSqlQuery DatabaseAccess::execQueryString(const QString &query)
{
    if(DBExist() == false)
        return QSqlQuery();

    QSqlQuery q(db);
    q.setForwardOnly(true);
    qDebug() << "Query: " << query;

    bool isSuccess = q.exec(query);
    if(isSuccess == false)
        qDebug() << q.lastError();

    return q;
}

QSqlQuery DatabaseAccess::execQueryFile(const QString &filePath)
{
    if(DBExist() == false)
        return QSqlQuery();

    qDebug() << "Sql file path: " << filePath;
    QFile queryFile(filePath);
    if(queryFile.exists() != true)
    {
        qDebug() << "Error: file does not exist";
        qDebug() << filePath;
        return QSqlQuery();
    }

    queryFile.open(QIODevice::ReadOnly);
    QTextStream fileStream(&queryFile);
    QString query = fileStream.readAll();
    return execQueryString(query);
}

QSqlQuery DatabaseAccess::execQueryStringBind(const QString &query, const QMap<QString, QVariant> &bindValues)
{
    if(DBExist() == false)
        return QSqlQuery();

    QSqlQuery q(db);
    q.setForwardOnly(true);
    qDebug() << "Query: " << query;

    q.prepare(query);

    for(auto it = bindValues.begin(); it != bindValues.end(); ++it)
    {
        // qDebug() << "Bind " << it.key() << " with " << it.value().toString();
        q.bindValue(it.key(), it.value());
    }
    if(q.exec() != true)
    {
        error = q.lastError().text();
        qDebug() << error;
    }

    // qDebug() << "last query: " << q.lastQuery();
    qDebug() << "last error: " << q.lastError();

    return q;




}

QSqlQuery DatabaseAccess::execQueryFileBind(const QString filePath, const QMap<QString, QVariant> &bindValues)
{
    if(DBExist() == false)
        return QSqlQuery();

    qDebug() << "Sql file path: " << filePath;
    QFile queryFile(filePath);
    if(queryFile.exists() != true)
    {
        qDebug() << "Error: file does not exist";
        qDebug() << filePath;
        return QSqlQuery();
    }


    queryFile.open(QIODevice::ReadOnly);
    return execQueryStringBind(queryFile.readAll(), bindValues);

}
