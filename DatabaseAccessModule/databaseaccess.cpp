#include "databaseaccess.h"

DatabaseAccess::DatabaseAccess(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database();
    if(db.isValid() == false)
    {
        qInfo() << "Add database SQLITE";
        db = QSqlDatabase::addDatabase("QSQLITE");
        if(db.isValid() == false)
        {
            emit error("Ошибка добавления драйвера SQLITE, текст ошибки: " + db.lastError().text());
        }
    }

}

DatabaseAccess::~DatabaseAccess()
{
    qInfo() << "Closing database";
    db.close();
}

bool DatabaseAccess::openDatabase(QString databasePath)
{
    if(QFile::exists(databasePath) != true)
    {
        qInfo() << "Не существует файла БД.";
        emit error("Файл базы данных не существует: " + databasePath);
        return false;
    }
    if(db.isValid() == false)
    {
        qInfo() << "Database is not valid";
        emit error("Ошибка: db.isValid().");
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
    qInfo() << "Query: " << query;

    bool isSuccess = q.exec(query);
    if(isSuccess == false)
    {
        qInfo() << q.lastError();
        emit error("Ошибка выпонления запроса " + query + "\nТекст ошибки: " + q.lastError().text());
    }

    return q;
}

QSqlQuery DatabaseAccess::execQueryFile(const QString &filePath)
{
    if(DBExist() == false)
        return QSqlQuery();

    qInfo() << "Sql file path: " << filePath;
    QFile queryFile(filePath);
    if(queryFile.exists() != true)
    {
        qInfo() << "Error: file does not exist";
        qInfo() << filePath;
        emit error("Ошибка выполнения запроса из файла " + filePath + " - файл не существует.");
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
    qInfo() << "Query: " << query;

    q.prepare(query);

    for(auto it = bindValues.begin(); it != bindValues.end(); ++it)
    {
        // qInfo() << "Bind " << it.key() << " with " << it.value().toString();
        q.bindValue(it.key(), it.value());
    }
    if(q.exec() != true)
    {
        qInfo() << q.lastError().text();
        emit error("Ошибка выполнения запроса " + query + "\nТекст ошибки: " + q.lastError().text());
    }

    return q;




}

QSqlQuery DatabaseAccess::execQueryFileBind(const QString filePath, const QMap<QString, QVariant> &bindValues)
{
    if(DBExist() == false)
        return QSqlQuery();

    qInfo() << "Sql file path: " << filePath;
    QFile queryFile(filePath);
    if(queryFile.exists() != true)
    {
        qInfo() << "Error: file does not exist";
        qInfo() << filePath;
        emit error("Ошибка выполнения запроса из файла " + filePath + " - файл не существует.");
        return QSqlQuery();
    }


    queryFile.open(QIODevice::ReadOnly);
    return execQueryStringBind(queryFile.readAll(), bindValues);

}
