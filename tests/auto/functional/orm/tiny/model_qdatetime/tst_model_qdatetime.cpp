#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"

#include "databases.hpp"

#include "models/datetime.hpp"

using Orm::Constants::ID;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::UTC;

using Orm::DB;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;

using Orm::Tiny::ConnectionOverride;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

using Models::Datetime;

class tst_Model_QDateTime : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    /* QDateTime with/without timezone */
    /* Server timezone UTC */
    void create_QDateTime_UtcTimezone_DatetimeAttribute_UtcOnServer() const;
    void create_QDateTime_0200Timezone_DatetimeAttribute_UtcOnServer() const;
    void create_QString_DatetimeAttribute_UtcOnServer() const;
    void create_QDateTime_UtcTimezone_TimestampAttribute_UtcOnServer() const;
    void create_QDateTime_0200Timezone_TimestampAttribute_UtcOnServer() const;
    void create_QString_TimestampAttribute_UtcOnServer() const;

    /* Server timezone +02:00 */
    void create_QDateTime_UtcTimezone_DatetimeAttribute_0200OnServer() const;
    void create_QDateTime_0200Timezone_DatetimeAttribute_0200OnServer() const;
    void create_QString_DatetimeAttribute_0200OnServer() const;
    void create_QDateTime_UtcTimezone_TimestampAttribute_0200OnServer() const;
    void create_QDateTime_0200Timezone_TimestampAttribute_0200OnServer() const;
    void create_QString_TimestampAttribute_0200OnServer() const;

    /* QDate */
    /* Server timezone UTC */
    void create_QDate_UtcTimezone_DateColumn_UtcOnServer() const;
    void create_QString_DateColumn_UtcOnServer() const;

    /* Server timezone +02:00 */
    void create_QDate_UtcTimezone_DateColumn_0200OnServer() const;
    void create_QString_DateColumn_0200OnServer() const;

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
    private:
        /*! Generate a call wrapped for the QVariant::typeId/userType for Qt5/6. */
        inline static auto typeIdWrapper(const QVariant &attribute);

        /*! Set the MySQL/PostgreSQL timezone session variable to the UTC value. */
        inline void setUtcTimezone(const QString &connection = {}) const;

        /*! Set the database timezone session variable to +02:00 value by connection. */
        inline void set0200Timezone(const QString &connection = {}) const;
        /*! Set the MySQL timezone session variable to the +02:00 value. */
        inline void set0200TimezoneForMySQL(const QString &connection = {}) const;
        /*! Set the PostgreSQL timezone session variable to the +02:00 value. */
        inline void setEUBratislavaTimezoneForPSQL(const QString &connection = {}) const;

        /*! Set the MySQL/PostgreSQL timezone session variable to the given value. */
        static void setTimezone(const QString &timezone, QtTimeZoneConfig &&qtTimeZone,
                                const QString &connection);
        /*! Get a SQL query string to set a database time zone session variable. */
        static QString getSetTimezoneQueryString(const QString &connection);

        /*! Get the QTimeZone +02:00 instance for MySQL/PostgreSQL. */
        inline const QTimeZone &timezone0200(const QString &connection = {}) const;

        /*! Restore the database after a QDateTime-related test. */
        void restore(quint64 lastId, bool restoreTimezone = false,
                     const QString &connection = {}) const;
};

/* private */

auto tst_Model_QDateTime::typeIdWrapper(const QVariant &attribute)
{
    /* It helps to avoid #ifdef-s for QT_VERSION in all test methods
       for the QVariant::typeId/userType for Qt5/6. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return std::bind_front(&QVariant::typeId, attribute);
#else
    return std::bind_front(&QVariant::userType, attribute);
#endif
}

/* private slots */

void tst_Model_QDateTime::initTestCase_data() const
{
    const auto &connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

/* QDateTime with/without timezone */

/* Server timezone UTC */

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["datetime"] = QDateTime::fromString("2022-08-28 13:14:15z",
                                                     Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute("datetime");

        auto typeId = typeIdWrapper(datetimeDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15z",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["datetime"] = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                     Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute("datetime");

        auto typeId = typeIdWrapper(datetimeDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 11:14:15z",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::create_QString_DatetimeAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["datetime"] = QString("2022-08-28 13:14:15");
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute("datetime");

        auto typeId = typeIdWrapper(datetimeDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15z",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["timestamp"] = QDateTime::fromString("2022-08-28 13:14:15z",
                                                      Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute("timestamp");

        auto typeId = typeIdWrapper(timestampDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15z",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["timestamp"] = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                      Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute("timestamp");

        auto typeId = typeIdWrapper(timestampDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 11:14:15z",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::create_QString_TimestampAttribute_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["timestamp"] = QString("2022-08-28 13:14:15");
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute("timestamp");

        auto typeId = typeIdWrapper(timestampDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15z",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["datetime"] = QDateTime::fromString("2022-08-28 13:14:15z",
                                                     Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute("datetime");

        auto typeId = typeIdWrapper(datetimeDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 15:14:15+02:00",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["datetime"] = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                     Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute("datetime");

        auto typeId = typeIdWrapper(datetimeDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_DatetimeAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["datetime"] = QString("2022-08-28 13:14:15");
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto datetimeDbVariant = datetime->getAttribute("datetime");

        auto typeId = typeIdWrapper(datetimeDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto datetimeActual = datetimeDbVariant.value<QDateTime>();
        const auto datetimeExpected = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                            Qt::ISODate);
        QCOMPARE(datetimeActual, datetimeExpected);
        QCOMPARE(datetimeActual, datetimeExpected.toLocalTime());
        QCOMPARE(datetimeActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_UtcTimezone_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["timestamp"] = QDateTime::fromString("2022-08-28 13:14:15z",
                                                      Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute("timestamp");

        auto typeId = typeIdWrapper(timestampDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 15:14:15+02:00",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::
create_QDateTime_0200Timezone_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["timestamp"] = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                      Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute("timestamp");

        auto typeId = typeIdWrapper(timestampDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_TimestampAttribute_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("SQLite database doesn't support setting a time zone on the database "
              "server side.", );

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["timestamp"] = QString("2022-08-28 13:14:15");
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto timestampDbVariant = datetime->getAttribute("timestamp");

        auto typeId = typeIdWrapper(timestampDbVariant);
        QCOMPARE(typeId(), QMetaType::QDateTime);

        /* The time zone must be as is defined in the qt_timezone connection
           configuration, TinyORM TinyBuilder fixes and unifies the buggy time zone
           behavior of all QtSql drivers. */
        const auto timestampActual = timestampDbVariant.value<QDateTime>();
        const auto timestampExpected = QDateTime::fromString("2022-08-28 13:14:15+02:00",
                                                             Qt::ISODate);
        QCOMPARE(timestampActual, timestampExpected);
        QCOMPARE(timestampActual, timestampExpected.toLocalTime());
        QCOMPARE(timestampActual.timeZone(), timezone0200(connection));
    }

    // Restore
    restore(lastId, true, connection);
}

/* QDate */

/* Server timezone UTC */

void tst_Model_QDateTime::create_QDate_UtcTimezone_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["date"] = QDate::fromString("2022-08-28", Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute("date");

        auto typeId = typeIdWrapper(dateDbVariant);
        QCOMPARE(typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

void tst_Model_QDateTime::create_QString_DateColumn_UtcOnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        // Will use isStandardDateFormat()
        datetime["date"] = QString("2022-08-28");
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute("date");

        auto typeId = typeIdWrapper(dateDbVariant);
        QCOMPARE(typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId);
}

/* Server timezone +02:00 */

void tst_Model_QDateTime::create_QDate_UtcTimezone_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        datetime["date"] = QDate::fromString("2022-08-28", Qt::ISODate);
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute("date");

        auto typeId = typeIdWrapper(dateDbVariant);
        QCOMPARE(typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true, connection);
}

void tst_Model_QDateTime::create_QString_DateColumn_0200OnServer() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    set0200Timezone(connection);

    quint64 lastId = 0;

    // Insert
    {
        Datetime datetime;
        // Will use isStandardDateFormat()
        datetime["date"] = QString("2022-08-28");
        QVERIFY(datetime.save());

        lastId = datetime[ID]->value<quint64>();
        QVERIFY(lastId != 0);
    }

    // Verify
    {
        // Get the fresh model from the database
        auto datetime = Datetime::find(lastId);
        QVERIFY(datetime);
        QVERIFY(datetime->exists);

        const auto dateDbVariant = datetime->getAttribute("date");

        auto typeId = typeIdWrapper(dateDbVariant);
        QCOMPARE(typeId(), QMetaType::QDate);

        const auto dateActual = dateDbVariant.value<QDate>();
        const auto dateExpected = QDate::fromString("2022-08-28", Qt::ISODate);
        QCOMPARE(dateActual, dateExpected);
    }

    // Restore
    restore(lastId, true, connection);
}

/* private */

void tst_Model_QDateTime::setUtcTimezone(const QString &connection) const
{
    setTimezone(UTC, {QtTimeZoneType::QtTimeSpec, QVariant::fromValue(Qt::UTC)},
                connection);
}

void tst_Model_QDateTime::set0200Timezone(const QString &connection) const
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL)
        set0200TimezoneForMySQL(connection);

    else if (driverName == QPSQL)
        setEUBratislavaTimezoneForPSQL(connection);

    // Do nothing for QSQLITE
}

namespace
{
    /*! Time zone +02:00 for MySQL. */
    Q_GLOBAL_STATIC_WITH_ARGS(QTimeZone, TimeZone0200, (QByteArray("UTC+02:00")));

    /*! Time zone Europe/Bratislava for PostgreSQL. */
    Q_GLOBAL_STATIC_WITH_ARGS(QTimeZone, TimeZoneEUBratislava,
                              (QByteArray("Europe/Bratislava")));
} // namespace

void tst_Model_QDateTime::set0200TimezoneForMySQL(const QString &connection) const
{
    setTimezone(QStringLiteral("+02:00"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZone0200)},
                connection);
}

void tst_Model_QDateTime::setEUBratislavaTimezoneForPSQL(const QString &connection) const
{
    setTimezone(QStringLiteral("Europe/Bratislava"),
                {QtTimeZoneType::QTimeZone, QVariant::fromValue(*TimeZoneEUBratislava)},
                connection);
}

void tst_Model_QDateTime::setTimezone(
        const QString &timezone, Orm::QtTimeZoneConfig &&qtTimeZone,
        const QString &connection)
{
    const auto qtQuery = DB::unprepared(
                             getSetTimezoneQueryString(connection).arg(timezone),
                             connection);

    QVERIFY(!qtQuery.isValid() && qtQuery.isActive() && !qtQuery.isSelect());

    DB::setQtTimeZone(std::move(qtTimeZone), connection);
}

QString tst_Model_QDateTime::getSetTimezoneQueryString(const QString &connection)
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL)
        return QStringLiteral("set time_zone=\"%1\";");

    if (driverName == QPSQL)
        return QStringLiteral("set time zone '%1';");

    Q_UNREACHABLE();
}

const QTimeZone &tst_Model_QDateTime::timezone0200(const QString &connection) const
{
    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL)
        return *TimeZone0200;

    if (driverName == QPSQL)
        return *TimeZoneEUBratislava;

    Q_UNREACHABLE();
}

void tst_Model_QDateTime::restore(
        const quint64 lastId, const bool restoreTimezone,
        const QString &connection) const
{
    const auto affected = Datetime::destroy(lastId);

    QCOMPARE(affected, 1);

    if (!restoreTimezone)
        return;

    // Restore also the MySQL timezone session variable to auto tests default UTC value
    if (!connection.isEmpty() && DB::driverName(connection) != QSQLITE)
        setUtcTimezone(connection);
}

QTEST_MAIN(tst_Model_QDateTime)

#include "tst_model_qdatetime.moc"
