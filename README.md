TexItEasy
======

Simple and Powerful Latex Editor http://texiteasy.com

Features
========

- Syntax coloration
- Command completion (with commands defined in the source, \ref your \label, \cite your bibitem and bibfile)
- Integrated pdf viewer (auto sync during scrolling, reverse sync with Ctrl+Click on the pdf)
- Spell checker
- Hide auxilary files
- Search with regex
- Splitable editor
- Quickly open associted files (input, bibliography)
- Autodetect the master file (if it is open)
- Restore last session
- Configure multiple builders
- Configure keybinding and themes

Build on Linux
==============

[![Build Status](https://travis-ci.org/Bramas/texiteasy.svg?branch=master)](https://travis-ci.org/Bramas/texiteasy)

### Ubuntu

    svn checkout https://github.com/Bramas/texiteasy/trunk/source
    sudo apt-get install libqt4-dev qt4-qmake libpoppler-qt4-dev zlib1g-dev
    cd source
    qmake texiteasy.pro
    make

### ArchLinux

	sudo pacman -S make gcc qt4 poppler-qt4 zlib texlive-core
	wget https://github.com/Bramas/texiteasy/archive/master.tar.gz
	tar -zxf master.tar.gz
	cd texiteasy-master/source
	qmake-qt4 texiteasy.pro
	make

optional but usefull:

    pacman -S texlive-lang texlive-science texlive-pstricks

even more:

    pacman -S texlive-lang texlive-science texlive-pstricks texlive-pictures texlive-latexextra

everything:

    pacman -S texlive-most

Build on Mac Os
===============

Build poppler
-------------

be sure that pkg-config is aware of your version of qt:

	export PKG_CONFIG_PATH="$HOME/Qt/5.4/clang_64/lib/pkgconfig

be sure that qt chooser is aware of your version of qt (in /etc/xdg/qtchooser/5.conf):

	$HOME/Qt/5.4/clang_64/bin
	$HOME/Qt/5.4/clang_64/lib

I dont know why but qt pkgconfig gives the wrong include path, so you have to link as follow:

	 ln -s $HOME/Qt/5.4/clang_64/lib/QtCore.framework/Versions/5/Headers/ $HOME/Qt/5.4/clang_64/include/QtCore
	 ln -s $HOME/Qt/5.4/clang_64/lib/QtGui.framework/Versions/5/Headers/ $HOME/Qt/5.4/clang_64/include/QtGui
	 ln -s $HOME/Qt/5.4/clang_64/lib/QtWidgets.framework/Versions/5/Headers/ $HOME/Qt/5.4/clang_64/include/QtWidgets
	 ln -s $HOME/Qt/5.4/clang_64/lib/QtXml.framework/Versions/5/Headers/ $HOME/Qt/5.4/clang_64/include/QtXml



Build on Windows
================

Use the mingw-builds (all dependencies are included):

http://sourceforge.net/projects/mingwbuilds/files/external-binary-packages/Qt-Builds/
