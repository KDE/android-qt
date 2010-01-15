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
!ifdef MODULE_OPENSOURCE
!macro OPENSOURCE_INITIALIZE
  !define MODULE_OPENSOURCE_PAGE "opensource.ini"
  page custom ModuleOpenSourceShowPage
!macroend
!macro OPENSOURCE_SECTIONS
  Section -ModuleOpenSourceSection
    !ifdef MODULE_OPENSOURCE_ROOT
      SetOutPath "$INSTDIR"
      File "${MODULE_OPENSOURCE_ROOT}\OPENSOURCE-NOTICE.TXT"
    !endif
  SectionEnd

  Function ModuleOpenSourceShowPage
    !insertmacro MUI_HEADER_TEXT "Open Source Edition" " "
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${MODULE_OPENSOURCE_PAGE}"
    strcpy "$LICENSEE" "Open Source"
    strcpy "$LICENSE_PRODUCT" "OpenSource"
  FunctionEnd
!macroend
!macro OPENSOURCE_DESCRIPTION
!macroend
!macro OPENSOURCE_STARTUP
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${MODULE_OPENSOURCE_PAGE}"
!macroend
!macro OPENSOURCE_FINISH
!macroend
!macro OPENSOURCE_UNSTARTUP
!macroend
!macro OPENSOURCE_UNINSTALL
  Section -un.ModuleOpenSourceSection
    Delete "$SMPROGRAMS\$STARTMENU_STRING\OpenSource Notice.lnk"
  SectionEnd
!macroend
!macro OPENSOURCE_UNFINISH
!macroend
!else ;MODULE_OPENSOURCE
!macro OPENSOURCE_INITIALIZE
!macroend
!macro OPENSOURCE_SECTIONS
!macroend
!macro OPENSOURCE_DESCRIPTION
!macroend
!macro OPENSOURCE_STARTUP
!macroend
!macro OPENSOURCE_FINISH
!macroend
!macro OPENSOURCE_UNSTARTUP
!macroend
!macro OPENSOURCE_UNINSTALL
!macroend
!macro OPENSOURCE_UNFINISH
!macroend
!endif ;MODULE_OPENSOURCE

