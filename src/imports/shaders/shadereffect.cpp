/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
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

#include "shadereffect.h"
#include "shadereffectbuffer.h"
#include "shadereffectsource.h"

#include <QDeclarativeItem>
#include <QPainter>
#include <QtOpenGL>

static QTransform savedWorldTransform;

ShaderEffect::ShaderEffect(QObject *parent)
    : QGraphicsEffect(parent)
    , m_changed(true)
{
}

ShaderEffect::~ShaderEffect()
{
}

void ShaderEffect::prepareBufferedDraw(QPainter *painter)
{
    // This workaround needed because QGraphicsEffect seems to always utilize default painters worldtransform
    // instead of the active painters worldtransform.
    const ShaderEffectBuffer *effectBuffer = dynamic_cast<ShaderEffectBuffer*> (painter->device());
    if (effectBuffer) {
        savedWorldTransform = painter->worldTransform() * savedWorldTransform;
        painter->setWorldTransform(savedWorldTransform);
    } else {
        savedWorldTransform = painter->worldTransform();
    }
}

void ShaderEffect::draw (QPainter *painter)
{
    const QGLContext *context = QGLContext::currentContext();

    prepareBufferedDraw(painter);

    if (context) {
        updateRenderTargets();
    }

    if (!context || m_renderTargets.count() == 0 || !hideOriginal())
        drawSource(painter);
}

void ShaderEffect::updateRenderTargets()
{
    if (!m_changed)
        return;

    m_changed = false;

    int count = m_renderTargets.count();
    for (int i = 0; i < count; i++) {
        if (m_renderTargets[i]->isLive() || m_renderTargets[i]->isDirtyTexture()) {
            m_renderTargets[i]->updateBackbuffer();
            ShaderEffectBuffer* target = m_renderTargets[i]->fbo();
            if (target && target->isValid() && target->width() > 0 && target->height() > 0) {
                QPainter p(target);
                p.setCompositionMode(QPainter::CompositionMode_Clear);
                p.fillRect(QRect(QPoint(0, 0), target->size()), Qt::transparent);
                p.setCompositionMode(QPainter::CompositionMode_SourceOver);

                QRectF sourceRect = m_renderTargets[i]->sourceRect();
                QSize textureSize = m_renderTargets[i]->textureSize();

                qreal yflip = m_renderTargets[i]->isMirrored() ? -1.0 : 1.0; // flip y to match scenegraph, it also flips texturecoordinates
                qreal xscale = 1.0;
                qreal yscale = 1.0 * yflip;

                qreal leftMargin = 0.0;
                qreal rightMargin = 0.0;
                qreal topMargin = 0.0;
                qreal bottomMargin = 0.0;

                qreal width = m_renderTargets[i]->sourceItem()->width();
                qreal height = m_renderTargets[i]->sourceItem()->height();

                if (!sourceRect.isEmpty()) {
                    leftMargin = -sourceRect.left();
                    rightMargin = sourceRect.right() - width;
                    topMargin = -sourceRect.top();
                    bottomMargin = sourceRect.bottom() - height;
                }

                if ((width + leftMargin + rightMargin) > 0 && (height + topMargin + bottomMargin) > 0) {
                    if (!textureSize.isEmpty()) {
                        qreal textureWidth = textureSize.width();
                        qreal textureHeight = textureSize.height();

                        xscale = width / (width + leftMargin + rightMargin);
                        yscale = height / (height + topMargin + bottomMargin);

                        p.translate(textureWidth / 2, textureHeight / 2);
                        p.scale(xscale, yscale * yflip);
                        p.translate(-textureWidth / 2, -textureHeight / 2);
                        p.scale(textureWidth / width, textureHeight / height);
                    } else {
                        xscale = width / (width + leftMargin + rightMargin);
                        yscale = height / (height + topMargin + bottomMargin);

                        p.translate(width / 2, height / 2);
                        p.scale(xscale, yscale * yflip);
                        p.translate(-width / 2, -height / 2);
                    }
                }

                drawSource(&p);
                p.end();
                m_renderTargets[i]->markSceneGraphDirty();
            }
        }
    }
}

void ShaderEffect::sourceChanged (ChangeFlags flags)
{
    Q_UNUSED(flags);
    m_changed = true;
}

void ShaderEffect::addRenderTarget(ShaderEffectSource *target)
{
    if (!m_renderTargets.contains(target))
        m_renderTargets.append(target);
}

void ShaderEffect::removeRenderTarget(ShaderEffectSource *target)
{
    int index = m_renderTargets.indexOf(target);
    if (index >= 0)
        m_renderTargets.remove(index);
    else
        qWarning() << "ShaderEffect::removeRenderTarget - did not find target.";
}

bool ShaderEffect::hideOriginal() const
{
    if (m_renderTargets.count() == 0)
        return false;

    // Just like scenegraph version, if there is even one source that says "hide original" we hide it.
    int count = m_renderTargets.count();
    for (int i = 0; i < count; i++) {
        if (m_renderTargets[i]->hideSource())
            return true;
    }
    return false;
}
