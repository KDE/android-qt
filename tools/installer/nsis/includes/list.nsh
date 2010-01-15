;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
;; All rights reserved.
;; Contact: Nokia Corporation (qt-info@nokia.com)
;;
;; This file is part of the tools applications of the Qt Toolkit.
;;
;; $QT_BEGIN_LICENSE:LGPL$
;; No Commercial Usage
;; This file contains pre-release code and may not be distributed.
;; You may use this file in accordance with the terms and conditions
;; contained in the Technology Preview License Agreement accompanying
;; this package.
;;
;; GNU Lesser General Public License Usage
;; Alternatively, this file may be used under the terms of the GNU Lesser
;; General Public License version 2.1 as published by the Free Software
;; Foundation and appearing in the file LICENSE.LGPL included in the
;; packaging of this file.  Please review the following information to
;; ensure the GNU Lesser General Public License version 2.1 requirements
;; will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
;;
;; In addition, as a special exception, Nokia gives you certain additional
;; rights.  These rights are described in the Nokia Qt LGPL Exception
;; version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
;;
;; If you have questions regarding the use of this file, please contact
;; Nokia at qt-info@nokia.com.
;;
;;
;;
;;
;;
;;
;;
;;
;; $QT_END_LICENSE$
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!ifndef LIST_INCLUDE
!define LIST_INCLUDE

; usage:
; push item
; push list
; call ItemInList
; returns 1 or 0
!macro ItemInList UN
Function ${UN}ItemInList
  exch $0 ;list
  exch
  exch $1 ;item
  push $2 ;counter
  push $3 ;substr
  push $4 ;char
  
  strcpy $3 ""
  strcpy $2 "0"

  loop:
    strcpy $4 $0 1 $2
    strcmp "$4" "" atend
    intop $2 $2 + 1

    strcmp "$4" "|" 0 +4
      strcmp "$3" "$1" found
      strcpy $3 "" ;reset substr
      goto +2
    strcpy $3 "$3$4" ;append char to substr
    goto loop

  found:
    strcpy $0 "1"
    goto done
    
  atend:
    strcmp "$3" "$1" found
    strcpy $0 "0"

  done:
  pop $4
  pop $3
  pop $2
  pop $1
  exch $0
FunctionEnd
!macroend

!insertmacro ItemInList ""
!insertmacro ItemInList "un."

Function GetItemInList
  exch $0 ;list
  exch
  exch $1 ;index
  push $2 ;counter
  push $3 ;substr
  push $4 ;char
  push $5 ;current index

  strcpy $3 ""
  strcpy $2 "0"
  strcpy $5 "1"

  loop:
    strcpy $4 $0 1 $2
    strcmp "$4" "" atend
    intop $2 $2 + 1

    strcmp "$4" "|" 0 +5
      strcmp "$5" "$1" found
      strcpy $3 "" ;reset substr
      intop $5 $5 + 1
      goto +2
    strcpy $3 "$3$4" ;append char to substr
    goto loop

  found:
    strcpy $0 "$3"
    goto done

  atend:
    strcmp "$5" "$1" found
    strcpy $0 ""

  done:
  pop $5
  pop $4
  pop $3
  pop $2
  pop $1
  exch $0
FunctionEnd

!endif ;LIST_INCLUDE
