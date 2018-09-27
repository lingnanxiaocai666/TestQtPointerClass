#include <QDebug>
#include <QtTest>
#include <QPointer>

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
#define qInfo       qDebug
#define qWarning    qDebug
#define qFatal      qDebug
#define qCritical   qDebug
#endif

class PointerTest : public QObject
{
    Q_OBJECT
public:
    PointerTest();

private Q_SLOTS:
    void testQPointer1();
    void testQPointer2();
    void testQSharedPointer();
    void testQWeakPointer();
    void testQScopedPointer1();
    void testQScopedPointer2();
    void testQScopedPointerInClass();
    void testQScopedPointerAndQPointer();
};

class Dummy : public QObject
{
    Q_OBJECT
public:
    Dummy(int instance = 0) : _instance(instance)
    {
        if (_instance) qInfo() << "*** Dummy said: constructed" << _instance;
        else qInfo() << "*** Dummy said: constructed";
    }
    ~Dummy()
    {
        if (_instance) qInfo() << "*** Dummy said: destroyed" << _instance;
        else qInfo() << "*** Dummy said: destroyed";
    }
private:
    int _instance;
};

PointerTest::PointerTest()
{
}

void PointerTest::testQPointer1()
{
    Dummy *obj = new Dummy();
    QPointer<Dummy> pObj(obj);

    qInfo() << "Delete C++ pointer";
    delete obj;

    qInfo() << "C++ pointer not null after delete";
    QVERIFY(obj);

    qInfo() << "QPointer will be null now";
    QVERIFY(!pObj);
}

void PointerTest::testQPointer2()
{
    Dummy *obj = new Dummy();
    QPointer<Dummy> pObj(obj);

    qInfo() << "Delete QPointer";
    delete pObj;

    qInfo() << "C++ pointer not null after delete";
    QVERIFY(obj);

    qInfo() << "QPointer will be null now";
    QVERIFY(!pObj);
}

void PointerTest::testQSharedPointer()
{
    Dummy *pI = new Dummy();
    QSharedPointer<Dummy> pI1(pI);
    QSharedPointer<Dummy> pI2 = pI1;

    qInfo() << "Clear first refrence";
    pI1.clear();

    qInfo() << "Second refrence is still pointing to Dummy, so it is not deleted";
    QVERIFY(!pI1);
    QVERIFY(pI2);
    QVERIFY(pI);

    qInfo() << "Clear second refrence";
    pI2.clear();

    qInfo() << "No shared pointers anymore, Dummy is deleted";
    QVERIFY(!pI1);
    QVERIFY(!pI2);
    QVERIFY(pI);
}

void PointerTest::testQWeakPointer()
{
    Dummy *pI = new Dummy();
    QSharedPointer<Dummy> pI1(pI);
    QWeakPointer<Dummy> pI2 = pI1;

    qInfo() << "Clear first refrence";
    pI1.clear();

    qInfo() << "No shared pointers anymore, Dummy is deleted";
    QVERIFY(!pI1);
    QVERIFY(!pI2);
    QVERIFY(pI);

    qInfo() << "To use the shared pointer, we must \"lock\" it for use";
    QSharedPointer<Dummy> pI2_locked = pI2.toStrongRef();
    QVERIFY(pI2_locked.isNull());
}

void PointerTest::testQScopedPointer1()
{
    {
        qInfo() << "Create new Dummy WITHOUT QScopedPointer";
        Dummy *obj = new Dummy();
        qInfo() << "End of scope, Dummy will NOT be deleted";
        QVERIFY(obj);
    }

    {
        qInfo() << "Create new Dummy with QScopedPointer";
        Dummy *obj = new Dummy();
        QScopedPointer<Dummy> pObj(obj);
        qInfo() << "End of scope, Dummy will be delete now";
        QVERIFY(pObj);
        QVERIFY(obj);
    }
}

void PointerTest::testQScopedPointer2()
{
    qInfo() << "Create new Dummy";
    Dummy *obj = new Dummy();
    {
         qInfo() << "Assign it to QScopedPointer";
         QScopedPointer<Dummy> pObj(obj);
         qInfo() << "End of scope, Dummy will be delete now";
         QVERIFY(pObj);
         QVERIFY(obj);
    }
    qInfo() << "Dummy deleted";
    QVERIFY(obj);
}

void PointerTest::testQScopedPointerInClass()
{
    class DummyofDummy
    {
    public:
        DummyofDummy()
        {
            qInfo() << "*** DummyofDummy said: constructed";
            obj = new Dummy(1);
            sptrObj.reset(new Dummy(2));
        }
        ~DummyofDummy()
        {
            qInfo() << "*** DummyofDummy said: destroyed";
        }
    private:
        Dummy *obj;
        QScopedPointer<Dummy> sptrObj;
    };

    qInfo() << "Create new DummyofDummy";
    DummyofDummy *obj = new DummyofDummy();
    qInfo() << "Destroy DummyofDummy, Dummy(2) will be delete now, but Dummy(1) not deleted";
    delete obj;
    qInfo() << "End of scope";
}

void PointerTest::testQScopedPointerAndQPointer()
{
    qInfo() << "Create new Dummy";
    Dummy *obj = new Dummy();
    QPointer<Dummy> ptrObj(obj);
    {
         qInfo() << "Assign it to QScopedPointer";
         QScopedPointer<Dummy> sptrObj(ptrObj);
         qInfo() << "End of scope, Dummy will be delete now";
         QVERIFY(sptrObj);
         QVERIFY(ptrObj);
         QVERIFY(obj);
    }
    qInfo() << "Dummy deleted";
    QVERIFY(!ptrObj);
    QVERIFY(obj);
}

QTEST_APPLESS_MAIN(PointerTest)

#include "tst_pointertest.moc"
