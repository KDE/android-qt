/*
    Copyright (c) 2011, BogDan Vatra <bog_dan_ro@yahoo.com>

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


package org.kde.necessitas.ministro;

import org.kde.necessitas.ministro.IMinistroCallback;

interface IMinistro
{
    /**
    * Check/download required libs to run the application
    *
    * param callback              - interface used by the service to notify the client when it has the libs
    * param modules               - Qt modules you want to check
    * param appName               - Application name, used to show more informations to user
    * param ministroApiLevel      - Ministro api level, used to check ministro service compatibility.
    *                               Current API Level is 1 !!!
    * param qtApiLevel            - Necessitas api level, used to download the right platform plugin.
    *                               Current API Level is 4 !!!
    */

    void checkModules(in IMinistroCallback callback, in String[] modules, in String appName, in int ministroApiLevel, in int qtApiLevel);
}
