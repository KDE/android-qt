/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>
#include "tst_qglthreads.h"

#ifdef Q_OS_SYMBIAN
#include <unistd.h> // for usleep
#define RUNNING_TIME 2000 // save GPU mem by running shorter time.
#else
#define RUNNING_TIME 5000
#endif




tst_QGLThreads::tst_QGLThreads(QObject *parent)
    : QObject(parent)
{
}

/*

   swapInThread

   The purpose of this testcase is to verify that it is possible to do rendering into
   a GL context from the GUI thread, then swap the contents in from a background thread.

   The usecase for this is to have the background thread do the waiting for vertical
   sync while the GUI thread is idle.

   Currently the locking is handled directly in the paintEvent(). For the actual usecase
   in Qt, the locking is done in the windowsurface before starting any drawing while
   unlocking is done after all drawing has been done.
 */


class SwapThread : public QThread
{
    Q_OBJECT
public:
    SwapThread(QGLWidget *widget)
        : m_widget(widget)
    {
        moveToThread(this);
    }

    void run() {
        QTime time;
        time.start();
        while (time.elapsed() < RUNNING_TIME) {
            lock();
            wait();

            m_widget->makeCurrent();
            m_widget->swapBuffers();
            m_widget->doneCurrent();
            unlock();
        }
    }

    void lock() { m_mutex.lock(); }
    void unlock() { m_mutex.unlock(); }

    void wait() { m_wait_condition.wait(&m_mutex); }
    void notify() { m_wait_condition.wakeAll(); }

private:
    QGLWidget *m_widget;
    QMutex m_mutex;
    QWaitCondition m_wait_condition;
};

class ForegroundWidget : public QGLWidget
{
public:
    ForegroundWidget(const QGLFormat &format)
        : QGLWidget(format), m_thread(0)
    {
        setAutoBufferSwap(false);
    }

    void paintEvent(QPaintEvent *)
    {
        m_thread->lock();
        makeCurrent();
        QPainter p(this);
        p.fillRect(rect(), QColor(rand() % 256, rand() % 256, rand() % 256));
        p.setPen(Qt::red);
        p.setFont(QFont("SansSerif", 24));
        p.drawText(rect(), Qt::AlignCenter, "This is an autotest");
        p.end();
        doneCurrent();
        m_thread->notify();
        m_thread->unlock();

        update();
    }

    void setThread(SwapThread *thread) {
        m_thread = thread;
    }

    SwapThread *m_thread;
};

void tst_QGLThreads::swapInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on mac as they were causing reboots", SkipAll);
#endif

    QGLFormat format;
    format.setSwapInterval(1);
    ForegroundWidget widget(format);
    SwapThread thread(&widget);
    widget.setThread(&thread);
    widget.show();

    QTest::qWaitForWindowShown(&widget);
    thread.start();

    while (thread.isRunning()) {
        qApp->processEvents();
    }

    widget.hide();

    QVERIFY(true);
}







/*
   textureUploadInThread

   The purpose of this testcase is to verify that doing texture uploads in a background
   thread is possible and that it works.
 */

class CreateAndUploadThread : public QThread
{
    Q_OBJECT
public:
    CreateAndUploadThread(QGLWidget *shareWidget)
    {
        m_gl = new QGLWidget(0, shareWidget);
        moveToThread(this);
    }

    ~CreateAndUploadThread()
    {
        delete m_gl;
    }

    void run() {
        m_gl->makeCurrent();
        QTime time;
        time.start();
        while (time.elapsed() < RUNNING_TIME) {
            int width = 400;
            int height = 300;
#ifdef Q_OS_SYMBIAN
            // GPU mem is very scarce resource on Symbian currently.
            // That's why we create only small textures.
            width = 50;
            height = 20;
#else
            QImage image(width, height, QImage::Format_RGB32);
            QPainter p(&image);
            p.fillRect(image.rect(), QColor(rand() % 256, rand() % 256, rand() % 256));
            p.setPen(Qt::red);
            p.setFont(QFont("SansSerif", 24));
            p.drawText(image.rect(), Qt::AlignCenter, "This is an autotest");
            p.end();
            m_gl->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, QGLContext::InternalBindOption);

            createdAndUploaded(image);
        }
    }

signals:
    void createdAndUploaded(const QImage &image);

private:
    QGLWidget *m_gl;
};

class TextureDisplay : public QGLWidget
{
    Q_OBJECT
public:
    void paintEvent(QPaintEvent *) {
        QPainter p(this);
        for (int i=0; i<m_images.size(); ++i) {
            p.drawImage(m_positions.at(i), m_images.at(i));
            m_positions[i] += QPoint(1, 1);
        }
        update();
    }

public slots:
    void receiveImage(const QImage &image) {
        m_images << image;
        m_positions << QPoint(-rand() % width() / 2, -rand() % height() / 2);

        if (m_images.size() > 100) {
            m_images.takeFirst();
            m_positions.takeFirst();
        }
    }

private:
    QList <QImage> m_images;
    QList <QPoint> m_positions;
};

void tst_QGLThreads::textureUploadInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on mac as they were causing reboots", SkipAll);
#endif

    TextureDisplay display;
    CreateAndUploadThread thread(&display);

    connect(&thread, SIGNAL(createdAndUploaded(QImage)), &display, SLOT(receiveImage(QImage)));

    display.show();
    QTest::qWaitForWindowShown(&display);

    thread.start();

    while (thread.isRunning()) {
        qApp->processEvents();
    }

    QVERIFY(true);
}






/*
   renderInThread

   This test sets up a scene and renders it in a different thread.
   For simplicity, the scene is simply a bunch of rectangles, but
   if that works, we're in good shape..
 */

static inline float qrandom() { return (rand() % 100) / 100.f; }

void renderAScene(int w, int h)
{
#ifdef QT_OPENGL_ES_2
            QGLShaderProgram program;
            program.addShaderFromSourceCode(QGLShader::Vertex, "attribute highp vec2 pos; void main() { gl_Position = vec4(pos.xy, 1.0, 1.0); }");
            program.addShaderFromSourceCode(QGLShader::Fragment, "uniform lowp vec4 color; void main() { gl_FragColor = color; }");
            program.bindAttributeLocation("pos", 0);
            program.bind();
            int colorId = program.uniformLocation("color");

            glEnableVertexAttribArray(0);

            for (int i=0; i<1000; ++i) {
                GLfloat pos[] = {
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.
                };

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, pos);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
            }
#else
            glViewport(0, 0, w, h);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glFrustum(0, w, h, 0, 1, 100);
            glTranslated(0, 0, -1);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            for (int i=0;i<1000; ++i) {
                glBegin(GL_TRIANGLES);
                glColor3f(qrandom(), qrandom(), qrandom());
                glVertex2f(qrandom() * w, qrandom() * h);
                glColor3f(qrandom(), qrandom(), qrandom());
                glVertex2f(qrandom() * w, qrandom() * h);
                glColor3f(qrandom(), qrandom(), qrandom());
                glVertex2f(qrandom() * w, qrandom() * h);
                glEnd();
            }
#endif
}

class ThreadSafeGLWidget : public QGLWidget
{
public:
    ThreadSafeGLWidget(QWidget *parent = 0) : QGLWidget(parent) {}
    void paintEvent(QPaintEvent *)
    {
        // ignored as we're anyway swapping as fast as we can
    };

    void resizeEvent(QResizeEvent *e)
    {
        mutex.lock();
        newSize = e->size();
        mutex.unlock();
    };

    QMutex mutex;
    QSize newSize;
};

class SceneRenderingThread : public QThread
{
    Q_OBJECT
public:
    SceneRenderingThread(ThreadSafeGLWidget *widget)
        : m_widget(widget)
    {
        moveToThread(this);
        m_size = widget->size();
    }

    void run() {
        QTime time;
        time.start();
        failure = false;

        m_widget->makeCurrent();

        while (time.elapsed() < RUNNING_TIME && !failure) {


            m_widget->mutex.lock();
            QSize s = m_widget->newSize;
            m_widget->mutex.unlock();

            if (s != m_size) {
                glViewport(0, 0, s.width(), s.height());
            }

            if (QGLContext::currentContext() != m_widget->context()) {
                failure = true;
                break;
            }

            glClear(GL_COLOR_BUFFER_BIT);

            int w = m_widget->width();
            int h = m_widget->height();

            renderAScene(w, h);

            int color;
            glReadPixels(w / 2, h / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);

            m_widget->swapBuffers();
        }

        m_widget->doneCurrent();
    }

    bool failure;

private:
    ThreadSafeGLWidget *m_widget;
    QSize m_size;
};

void tst_QGLThreads::renderInThread_data()
{
    QTest::addColumn<bool>("resize");
    QTest::addColumn<bool>("update");

    QTest::newRow("basic") << false << false;
    QTest::newRow("with-resize") << true << false;
    QTest::newRow("with-update") << false << true;
    QTest::newRow("with-resize-and-update") << true << true;
}

void tst_QGLThreads::renderInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on Mac as they were causing reboots", SkipAll);
#endif

#ifdef Q_OS_SYMBIAN
    QSKIP("OpenGL threading tests are disabled on Symbian as accessing RWindow from a secondary thread is not supported", SkipAll);
#endif

    QFETCH(bool, resize);
    QFETCH(bool, update);

    ThreadSafeGLWidget widget;
    widget.resize(200, 200);
    SceneRenderingThread thread(&widget);

    widget.show();
    QTest::qWaitForWindowShown(&widget);
    widget.doneCurrent();

    thread.start();

    int value = 10;
    while (thread.isRunning()) {
        if (resize)
            widget.resize(200 + value, 200 + value);
        if (update)
            widget.update(100 + value, 100 + value, 20, 20);
        qApp->processEvents();
        value = -value;

#ifdef Q_WS_WIN
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }

    QVERIFY(!thread.failure);
}

class Device
{
public:
    virtual ~Device() {}
    virtual QPaintDevice *realPaintDevice() = 0;
    virtual void prepareDevice() {}
};

class GLWidgetWrapper : public Device
{
public:
    GLWidgetWrapper() {
        widget.resize(150, 150);
        widget.show();
        QTest::qWaitForWindowShown(&widget);
        widget.doneCurrent();
    }
    QPaintDevice *realPaintDevice() { return &widget; }

    ThreadSafeGLWidget widget;
};

class PixmapWrapper : public Device
{
public:
    PixmapWrapper() { pixmap = new QPixmap(512, 512); }
    ~PixmapWrapper() { delete pixmap; }
    QPaintDevice *realPaintDevice() { return pixmap; }

    QPixmap *pixmap;
};

class PixelBufferWrapper : public Device
{
public:
    PixelBufferWrapper() { pbuffer = new QGLPixelBuffer(512, 512); }
    ~PixelBufferWrapper() { delete pbuffer; }
    QPaintDevice *realPaintDevice() { return pbuffer; }

    QGLPixelBuffer *pbuffer;
};


class FrameBufferObjectWrapper : public Device
{
public:
    FrameBufferObjectWrapper() {
        widget.makeCurrent();
        fbo = new QGLFramebufferObject(512, 512);
        widget.doneCurrent();
    }
    ~FrameBufferObjectWrapper() { delete fbo; }
    QPaintDevice *realPaintDevice() { return fbo; }
    void prepareDevice() { widget.makeCurrent(); }

    ThreadSafeGLWidget widget;
    QGLFramebufferObject *fbo;
};


class ThreadPainter : public QObject
{
    Q_OBJECT
public:
    ThreadPainter(Device *pd) : device(pd), fail(true) {
        pixmap = QPixmap(40, 40);
        pixmap.fill(Qt::green);
        QPainter p(&pixmap);
        p.drawLine(0, 0, 40, 40);
        p.drawLine(0, 40, 40, 0);
    }

public slots:
    void draw() {
        bool beginFailed = false;
        QTime time;
        time.start();
        int rotAngle = 10;
        device->prepareDevice();
        QPaintDevice *paintDevice = device->realPaintDevice();
        QSize s(paintDevice->width(), paintDevice->height());
        while (time.elapsed() < RUNNING_TIME) {
            QPainter p;
            if (!p.begin(paintDevice)) {
                beginFailed = true;
                break;
            }
            p.translate(s.width()/2, s.height()/2);
            p.rotate(rotAngle);
            p.translate(-s.width()/2, -s.height()/2);
            p.fillRect(0, 0, s.width(), s.height(), Qt::red);
            QRect rect(QPoint(0, 0), s);
            p.drawPixmap(10, 10, pixmap);
            p.drawTiledPixmap(50, 50, 100, 100, pixmap);
            p.drawText(rect.center(), "This is a piece of text");
            p.end();
            rotAngle += 2;
#ifdef Q_WS_WIN
            Sleep(20);
#else
            usleep(20 * 1000);
#endif
        }

        fail = beginFailed;
        QThread::currentThread()->quit();
    }

    bool failed() { return fail; }

private:
    QPixmap pixmap;
    Device *device;
    bool fail;
};

template <class T>
class PaintThreadManager
{
public:
    PaintThreadManager(int count) : numThreads(count)
    {
        for (int i=0; i<numThreads; ++i) {
            devices.append(new T);
            threads.append(new QThread);
            painters.append(new ThreadPainter(devices.at(i)));
            painters.at(i)->moveToThread(threads.at(i));
            painters.at(i)->connect(threads.at(i), SIGNAL(started()), painters.at(i), SLOT(draw()));
        }
    }

    ~PaintThreadManager() {
        qDeleteAll(threads);
        qDeleteAll(painters);
        qDeleteAll(devices);
    }


    void start() {
        for (int i=0; i<numThreads; ++i)
            threads.at(i)->start();
    }

    bool areRunning() {
        bool running = false;
        for (int i=0; i<numThreads; ++i){
            if (threads.at(i)->isRunning())
                running = true;
        }

        return running;
    }

    bool failed() {
        for (int i=0; i<numThreads; ++i) {
            if (painters.at(i)->failed())
                return true;
        }

        return false;
    }

private:
    QList<QThread *> threads;
    QList<Device *> devices;
    QList<ThreadPainter *> painters;
    int numThreads;
};

/*
   This test uses QPainter to draw onto different QGLWidgets in
   different threads at the same time. The ThreadSafeGLWidget is
   necessary to handle paint and resize events that might come from
   the main thread at any time while the test is running. The resize
   and paint events would cause makeCurrent() calls to be issued from
   within the QGLWidget while the widget's context was current in
   another thread, which would cause errors.
*/
void tst_QGLThreads::painterOnGLWidgetInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on Mac as they were causing reboots", SkipAll);
#endif
    if (!((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0) ||
          (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0))) {
        QSKIP("The OpenGL based threaded QPainter tests requires OpenGL/ES 2.0.", SkipAll);
    }

    PaintThreadManager<GLWidgetWrapper> painterThreads(5);
    painterThreads.start();

    while (painterThreads.areRunning()) {
        qApp->processEvents();
#ifdef Q_WS_WIN
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }
    QVERIFY(!painterThreads.failed());
}

/*
   This test uses QPainter to draw onto different QPixmaps in
   different threads at the same time.
*/
void tst_QGLThreads::painterOnPixmapInThread()
{
#ifdef Q_WS_X11
    QSKIP("Drawing text in threads onto X11 drawables currently crashes on some X11 servers.", SkipAll);
#endif
    PaintThreadManager<PixmapWrapper> painterThreads(5);
    painterThreads.start();

    while (painterThreads.areRunning()) {
        qApp->processEvents();
#ifdef Q_WS_WIN
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }
    QVERIFY(!painterThreads.failed());
}

/* This test uses QPainter to draw onto different QGLPixelBuffer
   objects in different threads at the same time.
*/
void tst_QGLThreads::painterOnPboInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on Mac as they were causing reboots", SkipAll);
#endif
    if (!((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0) ||
          (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0))) {
        QSKIP("The OpenGL based threaded QPainter tests requires OpenGL/ES 2.0.", SkipAll);
        return;
    }

    if (!QGLPixelBuffer::hasOpenGLPbuffers()) {
        QSKIP("This system doesn't support pbuffers.", SkipAll);
        return;
    }

    PaintThreadManager<PixelBufferWrapper> painterThreads(5);
    painterThreads.start();

    while (painterThreads.areRunning()) {
        qApp->processEvents();
#ifdef Q_WS_WIN
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }
    QVERIFY(!painterThreads.failed());
}

/* This test uses QPainter to draw onto different
   QGLFramebufferObjects (bound in a QGLWidget's context) in different
   threads at the same time.
*/
void tst_QGLThreads::painterOnFboInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on Mac as they were causing reboots", SkipAll);
#endif
    if (!((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0) ||
          (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0))) {
        QSKIP("The OpenGL based threaded QPainter tests requires OpenGL/ES 2.0.", SkipAll);
        return;
    }

    if (!QGLFramebufferObject::hasOpenGLFramebufferObjects()) {
        QSKIP("This system doesn't support framebuffer objects.", SkipAll);
        return;
    }

    PaintThreadManager<FrameBufferObjectWrapper> painterThreads(5);
    painterThreads.start();

    while (painterThreads.areRunning()) {
        qApp->processEvents();
#ifdef Q_WS_WIN
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }
    QVERIFY(!painterThreads.failed());
}

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_X11InitThreads);
    QApplication app(argc, argv);
    QTEST_DISABLE_KEYPAD_NAVIGATION \

    tst_QGLThreads tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_qglthreads.moc"
