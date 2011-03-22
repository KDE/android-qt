/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite module of the Qt Toolkit.
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


#include <QtCore/QtDebug>
#include <QtCore/QList>

#include <algorithm>
#include <iterator>
#include <iostream>
#include "compress.h"

#define QLALR_NO_CHECK_SORTED_TABLE

struct _Fit: public std::binary_function<int, int, bool>
{
  inline bool operator () (int a, int b) const
  {
    return a == 0 || b == 0 || a == b;
  }
};

struct _PerfectMatch: public std::binary_function<int, int, bool>
{
  inline bool operator () (int a, int b) const
  { return a == b; }
};

struct _GenerateCheck
{
  QVector<int>::const_iterator iterator;
  int initial;

  _GenerateCheck (QVector<int>::const_iterator it, int i):
    iterator (it),
    initial (i) {}

  inline int operator () ()
  {
    int check = initial++;
    return *iterator++ ? check : -1;
  }
};

class UncompressedRow
{
public:
  typedef const int *const_iterator;
  typedef const int *iterator;

public:
  inline UncompressedRow ():
    _M_index (0),
    _M_begin (0),
    _M_end (0),
    _M_beginNonZeros (0),
    _M_endNonZeros (0) {}

  inline UncompressedRow (int index, const_iterator begin, const_iterator end)
  { assign (index, begin, end); }

  inline int index () const { return _M_index; }
  inline const_iterator begin () const { return _M_begin; }
  inline const_iterator end () const { return _M_end; }

  inline void assign (int index, const_iterator begin, const_iterator end)
  {
    _M_index = index;
    _M_begin = begin;
    _M_end = end;

    _M_beginNonZeros = _M_begin;
    _M_endNonZeros = _M_end;

    for (_M_beginNonZeros = _M_begin; _M_beginNonZeros != _M_end && ! _M_beginNonZeros [0]; ++_M_beginNonZeros)
      /*continue*/ ;

#if 0
    for (_M_endNonZeros = _M_end; _M_endNonZeros != _M_beginNonZeros && ! _M_endNonZeros [-1]; --_M_endNonZeros)
      /*continue*/ ;
#endif
  }

  inline int at (int index) const
  { return _M_begin [index]; }

  inline bool isEmpty () const
  { return _M_begin == _M_end; }

  inline int size () const
  { return _M_end - _M_begin; }

  inline int nonZeroElements () const
  { return _M_endNonZeros - _M_beginNonZeros; }

  inline int count (int value) const
  { return std::count (begin (), end (), value); }

  inline const_iterator beginNonZeros () const
  { return _M_beginNonZeros; }

  inline const_iterator endNonZeros () const
  { return _M_endNonZeros; }

private:
  int _M_index;
  const_iterator _M_begin;
  const_iterator _M_end;
  const_iterator _M_beginNonZeros;
  const_iterator _M_endNonZeros;
};

struct _SortUncompressedRow: public std::binary_function<UncompressedRow, UncompressedRow, bool>
{
  inline bool operator () (const UncompressedRow &a, const UncompressedRow &b) const
  { return a.count (0) > b.count (0); }
};

Compress::Compress ()
{
}

void Compress::operator () (int *table, int row_count, int column_count)
{
  index.clear ();
  info.clear ();
  check.clear ();

  QVector<UncompressedRow> sortedTable (row_count);

  for (int i = 0; i < row_count; ++i)
    {
      int *begin = &table [i * column_count];
      int *end = begin + column_count;

      sortedTable [i].assign (i, begin, end);
    }

  qSort (sortedTable.begin (), sortedTable.end (), _SortUncompressedRow ());

#ifndef QLALR_NO_CHECK_SORTED_TABLE
  int previous_zeros = INT_MAX;

  foreach (UncompressedRow row, sortedTable)
    {
      int zeros = row.count (0);

      Q_ASSERT (zeros <= previous_zeros);
      zeros = previous_zeros;
      qDebug () << "OK!";
    }
#endif

  index.fill (-999999, row_count);

  foreach (UncompressedRow row, sortedTable)
    {
      int first_token = std::distance (row.begin (), row.beginNonZeros ());
      QVector<int>::iterator pos = info.begin ();

      while (pos != info.end ())
        {
          if (pos == info.begin ())
            {
              // try to find a perfect match
              QVector<int>::iterator pm = std::search (pos, info.end (), row.beginNonZeros (), row.endNonZeros (), _PerfectMatch ());

              if (pm != info.end ())
                {
                  pos = pm;
                  break;
                }
            }

          pos = std::search (pos, info.end (), row.beginNonZeros (), row.endNonZeros (), _Fit ());

          if (pos == info.end ())
            break;

          int idx = std::distance (info.begin (), pos) - first_token;
          bool conflict = false;

          for (int j = 0; ! conflict && j < row.size (); ++j)
            {
              if (row.at (j) == 0)
                conflict |= idx + j >= 0 && check [idx + j] == j;

              else
                conflict |= check [idx + j] == j;
            }

          if (! conflict)
            break;

          ++pos;
        }

      if (pos == info.end ())
        {
          int size = info.size ();

          info.resize (info.size () + row.nonZeroElements ());
          check.resize (info.size ());

          std::fill (check.begin () + size, check.end (), -1);
          pos = info.begin () + size;
        }

      int offset = std::distance (info.begin (), pos);
      index [row.index ()] = offset - first_token;

      for (const int *it = row.beginNonZeros (); it != row.endNonZeros (); ++it, ++pos)
        {
          if (*it)
            *pos = *it;
        }

      int i = row.index ();

      for (int j = 0; j < row.size (); ++j)
        {
          if (row.at (j) == 0)
            continue;

          check [index [i] + j] = j;
        }
    }

#if 0
  foreach (UncompressedRow row, sortedTable)
    {
      int i = row.index ();
      Q_ASSERT (i < sortedTable.size ());

      for (int j = 0; j < row.size (); ++j)
        {
          if (row.at (j) == 0)
            {
              Q_ASSERT (index [i] + j < 0 || check [index [i] + j] != j);
              continue;
            }

          Q_ASSERT ( info [index [i] + j] == row.at (j));
          Q_ASSERT (check [index [i] + j] == j);
        }
    }
#endif
}
