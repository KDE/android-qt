/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#import <QuartzCore/CAEAGLLayer.h>

#include "quikitwindow.h"

#include "quikitscreen.h"

#include <QtDebug>
#include <QtGui/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QPlatformGLContext>
#include <QtGui/QWindowSystemInterface>

#include <QtDebug>

class EAGLPlatformContext : public QPlatformGLContext
{
public:
    EAGLPlatformContext(EAGLView *view)
        : mView(view)
    {
        mFormat.setWindowApi(QPlatformWindowFormat::OpenGL);
        mFormat.setDepthBufferSize(24);
        mFormat.setAccumBufferSize(0);
        mFormat.setRedBufferSize(8);
        mFormat.setGreenBufferSize(8);
        mFormat.setBlueBufferSize(8);
        mFormat.setAlphaBufferSize(8);
        mFormat.setStencilBufferSize(8);
        mFormat.setSamples(0);
        mFormat.setSampleBuffers(false);
        mFormat.setDoubleBuffer(true);
        mFormat.setDepth(true);
        mFormat.setRgba(true);
        mFormat.setAlpha(true);
        mFormat.setAccum(false);
        mFormat.setStencil(true);
        mFormat.setStereo(false);
        mFormat.setDirectRendering(false);

#if defined(QT_OPENGL_ES_2)
        EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
#else
        EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
#endif
        [mView setContext:aContext];
    }

    ~EAGLPlatformContext() { }

    void makeCurrent()
    {
        QPlatformGLContext::makeCurrent();
        [mView makeCurrent];
    }

    void doneCurrent()
    {
        QPlatformGLContext::doneCurrent();
    }

    void swapBuffers()
    {
        [mView presentFramebuffer];
    }

    void* getProcAddress(const QString& ) { return 0; }

    QPlatformWindowFormat platformWindowFormat() const
    {
        return mFormat;
    }

private:
    EAGLView *mView;

    QPlatformWindowFormat mFormat;
};

@implementation EAGLView

@synthesize delegate;

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame])) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
        autocapitalizationType = UITextAutocapitalizationTypeNone;
        autocorrectionType = UITextAutocorrectionTypeNo;
        enablesReturnKeyAutomatically = NO;
        keyboardAppearance = UIKeyboardAppearanceDefault;
        keyboardType = UIKeyboardTypeDefault;
        returnKeyType = UIReturnKeyDone;
        secureTextEntry = NO;
    }
    return self;
}

- (void)setContext:(EAGLContext *)newContext
{
    if (mContext != newContext)
    {
        [self deleteFramebuffer];
        [mContext release];
        mContext = [newContext retain];
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)presentFramebuffer
{
    if (mContext) {
        [EAGLContext setCurrentContext:mContext];
        glBindRenderbuffer(GL_RENDERBUFFER, mColorRenderbuffer);
        [mContext presentRenderbuffer:GL_RENDERBUFFER];
    }
}

- (void)deleteFramebuffer
{
    if (mContext)
    {
        [EAGLContext setCurrentContext:mContext];
        if (mFramebuffer) {
            glDeleteFramebuffers(1, &mFramebuffer);
            mFramebuffer = 0;
        }
        if (mColorRenderbuffer) {
            glDeleteRenderbuffers(1, &mColorRenderbuffer);
            mColorRenderbuffer = 0;
        }
        if (mDepthRenderbuffer) {
            glDeleteRenderbuffers(1, &mDepthRenderbuffer);
            mDepthRenderbuffer = 0;
        }
    }
}

- (void)createFramebuffer
{
    if (mContext && !mFramebuffer)
    {
        [EAGLContext setCurrentContext:mContext];
        glGenFramebuffers(1, &mFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

        glGenRenderbuffers(1, &mColorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mColorRenderbuffer);
        [mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &mFramebufferWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &mFramebufferHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mColorRenderbuffer);

        glGenRenderbuffers(1, &mDepthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, mFramebufferWidth, mFramebufferHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        if (delegate && [delegate respondsToSelector:@selector(eaglView:usesFramebuffer:)]) {
            [delegate eaglView:self usesFramebuffer:mFramebuffer];
        }
    }
}

- (void)makeCurrent
{
    if (mContext)
    {
        [EAGLContext setCurrentContext:mContext];
        if (!mFramebuffer)
            [self createFramebuffer];
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        glViewport(0, 0, mFramebufferWidth, mFramebufferHeight);
    }
}

- (GLint)fbo
{
    return mFramebuffer;
}

- (void)setWindow:(QPlatformWindow *)window
{
    mWindow = window;
}

- (void)sendMouseEventForTouches:(NSSet *)touches withEvent:(UIEvent *)event fakeButtons:(Qt::MouseButtons)buttons
{
    UITouch *touch = [touches anyObject];
    CGPoint locationInView = [touch locationInView:self];
    QPoint p(locationInView.x, locationInView.y);
    // TODO handle global touch point? for status bar?
    QWindowSystemInterface::handleMouseEvent(mWindow->widget(), (ulong)(event.timestamp*1000),
        p, p, buttons);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self sendMouseEventForTouches:touches withEvent:event fakeButtons:Qt::LeftButton];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self sendMouseEventForTouches:touches withEvent:event fakeButtons:Qt::LeftButton];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self sendMouseEventForTouches:touches withEvent:event fakeButtons:Qt::NoButton];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self sendMouseEventForTouches:touches withEvent:event fakeButtons:Qt::NoButton];
}

// ------- Text Input ----------

@synthesize autocapitalizationType;
@synthesize autocorrectionType;
@synthesize enablesReturnKeyAutomatically;
@synthesize keyboardAppearance;
@synthesize keyboardType;
@synthesize returnKeyType;
@synthesize secureTextEntry;

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (BOOL)hasText
{
    return YES;
}

- (void)insertText:(NSString *)text
{
    QString string = QString::fromUtf8([text UTF8String]);
    int key = 0;
    if ([text isEqualToString:@"\n"])
        key = (int)Qt::Key_Return;

    // Send key event to window system interface
    QWindowSystemInterface::handleKeyEvent(
        0, QEvent::KeyPress, key, Qt::NoModifier, string, false, int(string.length()));
    QWindowSystemInterface::handleKeyEvent(
        0, QEvent::KeyRelease, key, Qt::NoModifier, string, false, int(string.length()));
}

- (void)deleteBackward
{
    // Send key event to window system interface
    QWindowSystemInterface::handleKeyEvent(
        0, QEvent::KeyPress, (int)Qt::Key_Backspace, Qt::NoModifier);
    QWindowSystemInterface::handleKeyEvent(
        0, QEvent::KeyRelease, (int)Qt::Key_Backspace, Qt::NoModifier);
}

@end

QT_BEGIN_NAMESPACE

QUIKitWindow::QUIKitWindow(QWidget *tlw) :
    QPlatformWindow(tlw),
    mWindow(nil),
    mContext(0)
{
    mScreen = static_cast<QUIKitScreen *>(QPlatformScreen::platformScreenForWidget(tlw));
    CGRect screenBounds = [mScreen->uiScreen() bounds];
    QRect geom(screenBounds.origin.x, screenBounds.origin.y, screenBounds.size.width, screenBounds.size.height);
    QPlatformWindow::setGeometry(geom);
    mView = [[EAGLView alloc] initWithFrame:CGRectMake(geom.x(), geom.y(), geom.width(), geom.height())];
}

QUIKitWindow::~QUIKitWindow()
{
    delete mContext; mContext = 0;
    [mView release];
    [mWindow release];
}

void QUIKitWindow::setGeometry(const QRect &rect)
{
    if (mWindow && rect != geometry()) {
        mWindow.frame = CGRectMake(rect.x(), rect.y(), rect.width(), rect.height());
        mView.frame = CGRectMake(0, 0, rect.width(), rect.height());
        [mView deleteFramebuffer];
        [mWindow setNeedsDisplay];
    }
    QPlatformWindow::setGeometry(rect);
}

UIWindow *QUIKitWindow::ensureNativeWindow()
{
    if (!mWindow) {
        // window
        CGRect frame = [mScreen->uiScreen() applicationFrame];
        QRect geom = QRect(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
        widget()->setGeometry(geom);
        mWindow = [[UIWindow alloc] init];
        mWindow.screen = mScreen->uiScreen();
        mWindow.frame = frame; // for some reason setting the screen resets frame.origin, so we need to set the frame afterwards

        // view
        [mView deleteFramebuffer];
        mView.frame = CGRectMake(0, 0, frame.size.width, frame.size.height); // fill
        [mView setMultipleTouchEnabled:YES];
        [mView setWindow:this];
        [mWindow addSubview:mView];
        [mWindow setNeedsDisplay];
        [mWindow makeKeyAndVisible];
    }
    return mWindow;
}

QPlatformGLContext *QUIKitWindow::glContext() const
{
    if (!mContext) {
        mContext = new EAGLPlatformContext(mView);
    }
    return mContext;
}

QT_END_NAMESPACE
