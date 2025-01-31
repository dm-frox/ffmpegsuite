# FFmpeg Suite
Autor: Dmitry Ponomarev

Contact: dmfrox219@mail.ru

Version 1.0

[eng](#eng)

Цель проекта — создание удобного инструментария для разработки мультимедийных приложений на основе FFmpeg API.
Ядром проекта является объектно-ориентированная обертка над FFmpeg API.
Обертка написана на C++14 и в ней представлены классы для основных компонент FFmpeg — мультиплексоров, демультиплексоров, потоков, кодеков, конвертеров кадров, фильтров и графа фильтров.
Есть классы, предназначенные для построения мультимедийных приложений разного уровня.
Эта обертка реализована в виде разделяемой библиотеки `FFmpegWrapper.dll`.
Эта библиотека может быть использована в .NET, для нее имеется необходимая управляемая обертка.
Также имеются .NET классы для организации удобного интерфейса, классы медиапроигрывателя и вспомогательных компонент, классы для реализации видеорендеренга.
Имеются три .NET (WPF) приложения — `Explorer`, `Player`, `Transcoder`.
Эти приложения позволяют получить информацию о компонентах FFmpeg, экспериментировать с разными медиаконтейнерами, кодеками, фильтрами, осуществлять транскодирование.

## Папка doc
Документация.
### Подпапка help
Справочные файлы для приложений.
## Папка ffmpeg
Результат сборки FFmpeg: библиотеки, приложения, документация.  
Файлы LICENSE, README.txt, version.txt.
### Подпапка bin/x64
Библиотеки и стандартные приложения — `ffmpeg`, `ffprobe`, `ffplay`.
### Подпапка dev/x64
Файлы, необходимые для компиляции и компоновки C/C++ программ, использующих FFmpeg API.
### Подпапка dev/examples
Примеры программирования на C с использованием FFmpeg API.
### Подпапка doc
Документация по компонентам FFmpeg.
## Папка params
Параметры для конфигурирования приложений.
## Папка sources
Исходный код проекта.
Файл `FFmpegSuite.sln` — решение для IDE Visual Studio 2022.
### Подпапка AudioRend
Аудиорендерер.
#### Проект AudioRend
Библиотека динамической компоновки `AudioRend.dll` (C++). 
Модуль, экспортирующий интерфейс аудиорендерера для C++ клиентов.
#### Проект AudioRendInterop
.NET сборка `AudioRendInterop.dll` (C++/CLI).  
Модуль-адаптер для использования аудиорендерера в .NET.
#### Проект AudioRendLib
Статическая библиотека `AudioRendLib.lib` (C++).  
C++ классы, реализация функционала аудиорендерера.
### Подпапка FFmpegCpp
C++ проекты.
#### Проект CommonToolLib
Статическая библиотека `CommonToolLib.lib` (C++).  
Вспомогательные C++ классы, не связанные с FFmpeg.
Включают текстовой логгер, классы для управления рабочими потоками, контейнеры.
#### Проект FFmpegAppLib
Статическая библиотека `FFmpegAppLib.lib` (C++).  
C++ классы для построения приложений, использующих FFmpeg.
#### Проект FFmpegCoreLib
Статическая библиотека `FFmpegCoreLib.lib` (C++).  
C++ классы — классы-обертки для основных компонент FFmpeg.
#### Проект FFmpegListLib
Статическая библиотека `FFmpegListLib.lib` (C++).  
C++ классы — классы для генерации списков основных компонент FFmpeg.
#### Проект FFmpegWrapper
Библиотека динамической компоновки `FFmpegWrapper.dll` (C++).  
Модуль, экспортирующий классы из `FFmpegCoreLib.lib`, `FFmpegAppLib.lib` и `FFmpegListLib.lib` для C++ клиентов.
### Подпапка FFmpegNet
.NET сборки, использующие `FFmpegWrapper.dll`.
#### Проект FFmpegInterop
.NET сборка `FFmpegInterop.dll` (C++/CLI).  
Модуль-адаптер для использования `FFmpegWrapper.dll` в .NET.
#### Проект FFmpegNetAux
.NET сборка `FFmpegNetAux.dll` (C#).  
Вспомогательные классы
#### Проект FFmpegNetMediaSource
.NET сборка `FFmpegNetMediaSource.dll` (C#).  
Классы для получения инфоромации об источнике медиаданных
#### Проект FFmpegNetPlayer
.NET сборка `FFmpegNetPlayer.dll` (C#).  
Классы для реализации медиапроигрывателя в .NET.
#### Проект FFmpegNetTranscoder
.NET сборка `FFmpegNetPTranscoder.dll` (C#).  
Классы для реализации транскодера в .NET.
#### Проект RendVideo
.NET сборка `RendVideo.dll` (C#).  
Видеорендерер для .NET (WPF).
#### Проект RendVideoInterop
.NET сборка `RendVideoInterop.dll` (C++/CLI).  
Модуль-адаптер для использования `RendVideo.dll` в `FFmpegWrapper.dll`.
### Подпапка FFmpegNetApps
.NET (WPF) приложения, базирующиеся на `FFmpegWrapper.dll`, вспомогательные сборки.
#### Проект AppTools
.NET сборка `AppTools.dll` (C#).  
Вспомогательные классы для приложений.
#### Проект CommonCtrls
.NET сборка `CommonCtrls.dll` (C#).  
Дополнительные управляющие элементы WPF.
#### Проект CsLogger
.NET сборка `CsLogger.dll` (C#).  
Текстовой логгер.
#### Проект Explorer
.NET приложение `Explorer.exe` (C#).  
Приложение для просмотра основных компонент FFmpeg.
#### Проект Player
.NET приложение `Player.exe` (C#).  
Медиапроигрыватель.
#### Проект Transcoder
.NET приложение `Transcoder.exe` (C#).  
Транскодер.

*****

## Eng

The goal of the project is to create a convenient toolkit for developing multimedia applications based on the FFmpeg API.
The core of the project is an object-oriented wrapper over the FFmpeg API.
The wrapper is written in C++14 and contains classes for the main components of FFmpeg — muxers, demuxers, streams, codecs, frame converters, filters, and a filter graph.
There are classes designed for building multimedia applications of various levels.
This wrapper is implemented as a shared library `FFmpegWrapper.dll`.
This library can be used in .NET, it has the required managed wrapper.
There are also .NET classes for organizing a convenient interface, classes for a media player and auxiliary components, and classes for implementing video rendering.
There are three .NET (WPF) applications - `Explorer`, `Player`, `Transcoder`.
These applications allow you to get information about FFmpeg components, experiment with different media containers, codecs, filters, and perform transcoding.
## Folder doc
Documentation.
### Subfolder help
Help files for the applications.
### Subfolder examples
A demo demuxer.
## Folder ffmpeg
FFmpeg build: libraries, applications, documentations.  
Files LICENSE, README.txt, version.txt.
### Subfolder bin/x64
The libraries and the standard applications -`ffmpeg`, `ffprobe`, `ffplay`.
### Subfolder dev/x64
Files needed to compile and build C/C++ programs using the FFmpeg API.
### Subfolder dev/examples
C programming examples using the FFmpeg API.
### Subfolder doc
FFmpeg component documentation.
## Folder params
Options for configuring applications.
## Folder sources
Project source code.  
A file `FFmpegSuite.sln` — solution for IDE Visual Studio 2022.
### Subfolder AudioRend
Audio renderer.
#### Project AudioRend
Dynamic-link library `AudioRend.dll` (C++).  
A module that exports the audio renderer interface for C++ clients.
#### Project AudioRendInterop
.NET assembly `AudioRendInterop.dll` (C++/CLI).  
An adapter module for using an audio renderer in .NET.
#### Project AudioRendLib
Static library `AudioRendLib.lib` (C++).  
C++ classes, implementation of the audio renderer functionality.
### Subfolder FFmpegCpp
C++ projects.
#### Project CommonToolLib
Static library `CommonToolLib.lib` (C++).  
Helper C++ classes not related to FFmpeg. Includes a text logger, classes for managing work threads, containers.
#### Project FFmpegAppLib
Static library `FFmpegAppLib.lib` (C++).  
C++ classes for building applications that use FFmpeg.
#### Project FFmpegCoreLib
Static library `FFmpegCoreLib.lib` (C++).  
Wrapper classes for the main FFmpeg components.
#### Project FFmpegListLib
Static library `FFmpegListLib.lib` (C++).  
C++ classes to generate FFmpeg component lists.
#### Project FFmpegWrapper
Dynamic-link library `FFmpegWrapper.dll` (C++).  
A module that exports classes from `FFmpegCoreLib.lib`, `FFmpegListLib.lib` and `FFmpegAppLib.lib` for C++ clients.
### Subfolder FFmpegNet
.NET assemblies using `FFmpegWrapper.dll`.
#### Project FFmpegInterop
.NET assembly `FFmpegInterop.dll` (C++/CLI).  
An adapter module for using `FFmpegWrapper.dll` in .NET.
#### Project FFmpegNetAux
.NET assembly `FFmpegNetAux.dll` (C#).  
Auxiliary classes
#### Project FFmpegNetMediaSource
.NET assembly `FFmpegNetMediaSource.dll` (C#).  
Classes for media source information reader
#### Project FFmpegNetPlayer
.NET assembly `FFmpegNetPlayer.dll` (C#).  
Classes for media player implementation in .NET.
#### Project FFmpegNetTranscoder
.NET assembly `FFmpegNetPTranscoder.dll` (C#).  
Classes for transcoder implementation in .NET.
#### Project RendVideo
.NET assembly `RendVideo.dll` (C#).  
Video renderer for use in .NET (WPF).
#### Project RendVideoInterop
.NET assembly `RendVideoInterop.dll` (C++/CLI).  
An adapter module for using `RendVideo.dll` in FFmpegWrapper.dll.
### Subfolder FFmpegNetApps
.NET (WPF) applications based on `FFmpegWrapper.dll`, auxiliary assemblies.
#### Project AppTools
.NET assembly `AppTools.dll` (C#).  
Helper classes for applications.
#### Project CommonCtrls
.NET assembly `CommonCtrls.dll` (C#).  
Additional WPF controls.
#### Project CsLogger
.NET assembly `CsLogger.dll` (C#).  
А text logger.
#### Project Explorer
.NET application `Explorer.exe` (C#).  
An application for viewing FFmpeg components.
#### Project Player
.NET application `Player.exe` (C#).  
A media player.
#### Project Transcoder
.NET application `Transcoder.exe` (C#).  
A transcoder.

