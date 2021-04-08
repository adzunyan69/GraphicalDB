#include "databaseaccess.h"

DatabaseAccess::DatabaseAccess(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database();
    if(db.isValid() == false)
    {
        qDebug() << "Add database SQLITE";
        db = QSqlDatabase::addDatabase("QSQLITE");
        if(db.isValid() == false)
        {
            emit error("Ошибка добавления драйвера SQLITE, текст ошибки: " + db.lastError().text());
        }
    }

}

DatabaseAccess::~DatabaseAccess()
{
    qDebug() << "Closing database";
    db.close();
}

bool DatabaseAccess::openDatabase(QString databasePath)
{
    if(QFile::exists(databasePath) != true)
    {
        qDebug() << "Не существует файла БД.";
        emit error("Файл базы данных не существует: " + databasePath);
        return false;
    }
    if(db.isValid() == false)
    {
        qDebug() << "Database is not valid";
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
    qDebug() << "Query: " << query;

    bool isSuccess = q.exec(query);
    if(isSuccess == false)
    {
        qDebug() << q.lastError();
        emit error("Ошибка выпонления запроса " + query + "\nТекст ошибки: " + q.lastError().text());
    }

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
    qDebug() << "Query: " << query;

    q.prepare(query);

    for(auto it = bindValues.begin(); it != bindValues.end(); ++it)
    {
        // qDebug() << "Bind " << it.key() << " with " << it.value().toString();
        q.bindValue(it.key(), it.value());
    }
    if(q.exec() != true)
    {
        qDebug() << q.lastError().text();
        emit error("Ошибка выполнения запроса " + query + "\nТекст ошибки: " + q.lastError().text());
    }

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
        emit error("Ошибка выполнения запроса из файла " + filePath + " - файл не существует.");
        return QSqlQuery();
    }


    queryFile.open(QIODevice::ReadOnly);
    return execQueryStringBind(queryFile.readAll(), bindValues);

}
