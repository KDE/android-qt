/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QTCONCURRENT_FILTER_H
#define QTCONCURRENT_FILTER_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qtconcurrentfilterkernel.h>
#include <QtCore/qtconcurrentfunctionwrappers.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifdef qdoc

namespace QtConcurrent {

    QFuture<void> filter(Sequence &sequence, FilterFunction filterFunction);

    template <typename T>
    QFuture<T> filtered(const Sequence &sequence, FilterFunction filterFunction);
    template <typename T>
    QFuture<T> filtered(ConstIterator begin, ConstIterator end, FilterFunction filterFunction);

    template <typename T>
    QFuture<T> filteredReduced(const Sequence &sequence,
                               FilterFunction filterFunction,
                               ReduceFunction reduceFunction,
                               QtConcurrent::ReduceOptions reduceOptions = UnorderedReduce | SequentialReduce);
    template <typename T>
    QFuture<T> filteredReduced(ConstIterator begin,
                               ConstIterator end,
                               FilterFunction filterFunction,
                               ReduceFunction reduceFunction,
                               QtConcurrent::ReduceOptions reduceOptions = UnorderedReduce | SequentialReduce);

    void blockingFilter(Sequence &sequence, FilterFunction filterFunction);

    template <typename Sequence>
    Sequence blockingFiltered(const Sequence &sequence, FilterFunction filterFunction);
    template <typename Sequence>
    Sequence blockingFiltered(ConstIterator begin, ConstIterator end, FilterFunction filterFunction);

    template <typename T>
    T blockingFilteredReduced(const Sequence &sequence,
                              FilterFunction filterFunction,
                              ReduceFunction reduceFunction,
                              QtConcurrent::ReduceOptions reduceOptions = UnorderedReduce | SequentialReduce);
    template <typename T>
    T blockingFilteredReduced(ConstIterator begin,
                              ConstIterator end,
                              FilterFunction filterFunction,
                              ReduceFunction reduceFunction,
                              QtConcurrent::ReduceOptions reduceOptions = UnorderedReduce | SequentialReduce);

} // namespace QtConcurrent

#else

namespace QtConcurrent {

template <typename Sequence, typename KeepFunctor, typename T, typename C, typename U>
ThreadEngineStarter<void> filterInternal(Sequence &sequence, KeepFunctor keep, T (C::*reduce)(U))
{
    typedef MemberFunctionWrapper1<T, C, U> ReduceFunctor;
    typedef typename Sequence::const_iterator Iterator;
    typedef FilterKernel<Sequence, KeepFunctor, ReduceFunctor> KernelType;
    return startThreadEngine(new KernelType(sequence, keep, reduce));
}

// filter() on sequences
template <typename Sequence, typename KeepFunctor>
QFuture<void> filter(Sequence &sequence, KeepFunctor keep)
{
    return filterInternal(sequence, QtPrivate::createFunctionWrapper(keep), &Sequence::push_back);
}

// filteredReduced() on sequences
template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startFilteredReduced<ResultType>(sequence, QtPrivate::createFunctionWrapper(keep), QtPrivate::createFunctionWrapper(reduce), options);
}

template <typename Sequence, typename KeepFunctor, typename ReduceFunctor>
QFuture<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType> filteredReduced(const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startFilteredReduced<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
            (sequence,
             QtPrivate::createFunctionWrapper(keep),
             QtPrivate::createFunctionWrapper(reduce),
             options);
}

// filteredReduced() on iterators
template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
   return startFilteredReduced<ResultType>(begin, end, QtPrivate::createFunctionWrapper(keep), QtPrivate::createFunctionWrapper(reduce), options);
}

template <typename Iterator, typename KeepFunctor, typename ReduceFunctor>
QFuture<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
   return startFilteredReduced<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
           (begin, end,
            QtPrivate::createFunctionWrapper(keep),
            QtPrivate::createFunctionWrapper(reduce),
            options);
}

// filtered() on sequences
template <typename Sequence, typename KeepFunctor>
QFuture<typename Sequence::value_type> filtered(const Sequence &sequence, KeepFunctor keep)
{
    return startFiltered(sequence, QtPrivate::createFunctionWrapper(keep));
}

// filtered() on iterators
template <typename Iterator, typename KeepFunctor>
QFuture<typename qValueType<Iterator>::value_type> filtered(Iterator begin, Iterator end, KeepFunctor keep)
{
    return startFiltered(begin, end, QtPrivate::createFunctionWrapper(keep));
}

// blocking filter() on sequences
template <typename Sequence, typename KeepFunctor>
void blockingFilter(Sequence &sequence, KeepFunctor keep)
{
    filterInternal(sequence, QtPrivate::createFunctionWrapper(keep), &Sequence::push_back).startBlocking();
}

// blocking filteredReduced() on sequences
template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor>
ResultType blockingFilteredReduced(const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startFilteredReduced<ResultType>(sequence, QtPrivate::createFunctionWrapper(keep), QtPrivate::createFunctionWrapper(reduce), options)
        .startBlocking();
}

template <typename Sequence, typename KeepFunctor, typename ReduceFunctor>
typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType blockingFilteredReduced(const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
        (sequence,
         QtPrivate::createFunctionWrapper(keep),
         QtPrivate::createFunctionWrapper(reduce),
         options);
}

// blocking filteredReduced() on iterators
template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor>
ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startFilteredReduced<ResultType>
        (begin, end,
         QtPrivate::createFunctionWrapper(keep),
         QtPrivate::createFunctionWrapper(reduce),
         options)
        .startBlocking();
}

template <typename Iterator, typename KeepFunctor, typename ReduceFunctor>
typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startFilteredReduced<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
        (begin, end,
         QtPrivate::createFunctionWrapper(keep),
         QtPrivate::createFunctionWrapper(reduce),
         options)
        .startBlocking();
}

// blocking filtered() on sequences
template <typename Sequence, typename KeepFunctor>
Sequence blockingFiltered(const Sequence &sequence, KeepFunctor keep)
{
    return blockingFilteredReduced(sequence, QtPrivate::createFunctionWrapper(keep), &Sequence::push_back, OrderedReduce);
}

// blocking filtered() on iterators
template <typename OutputSequence, typename Iterator, typename KeepFunctor>
OutputSequence blockingFiltered(Iterator begin, Iterator end, KeepFunctor keep)
{
    return blockingFilteredReduced(begin,
                                   end,
                                   QtPrivate::createFunctionWrapper(keep),
                                   &OutputSequence::push_back,
                                   OrderedReduce);
}

} // namespace QtConcurrent

#endif // qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
