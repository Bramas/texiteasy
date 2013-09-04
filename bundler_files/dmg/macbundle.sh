#cp /Library/Frameworks/QtGui.framework/Versions/4/QtGui .
#cp /Library/Frameworks/QtCore.framework/Versions/4/QtCore .
#cp /Library/Frameworks/QtXml.framework/Versions/4/QtXml .

#cp /usr/X11/lib/libfreetype.6.dylib .
#cp /usr/lib/libz.1.dylib .
#cp /Users/win/Documents/Quentin/poppler-0.22.5/qt4/src/libpoppler-qt4.4.dylib .
#cp /usr/lib/libstdc++.6.dylib .
#cp /usr/lib/libgcc_s.1.dylib .
#cp /Users/win/Documents/Quentin/poppler-0.22.5/libpoppler.37.dylib .
#cp /usr/local/lib/libfontconfig.1.dylib .
#cp /usr/local/lib/libjpeg.8.dylib .
#cp /usr/local/lib/liblcms2.2.dylib .
#cp /usr/local/lib/libtiff.5.dylib .


dylibbundler -od -b -x ../../../texiteasy_Release/texiteasy.app/Contents/MacOS/texiteasy -d ../../../texiteasy_Release/texiteasy.app/Contents/libs/
mkdir ../../../texiteasy_Release/texiteasy.app/Contents/Frameworks
cp -r bunddleNeeds/Frameworks/libpng.framework ../../../texiteasy_Release/texiteasy.app/Contents/Frameworks/libpng.framework
macdeployqt ../../../texiteasy_Release/texiteasy.app
