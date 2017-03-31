
#cp Info.plist ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app/Contents/Info.plist

BUILD_PATH=../../../build/release

sudo sed "/VERSION_NUMBER/s//$1/" Info.plist > $BUILD_PATH/texiteasy.app/Contents/Info.plist

#mkdir $BUILD_PATH/texiteasy.app/Contents/PlugIns
#mkdir $BUILD_PATH/texiteasy.app/Contents/PlugIns/platforms
#cp /Users/quentinbramas/Qt/5.8/clang_64/plugins/platforms/libqcocoa.dylib $BUILD_PATH/texiteasy.app/Contents/PlugIns/platforms/libqcocoa.dylib


mkdir $BUILD_PATH/texiteasy.app/Contents/Resources/dictionaries
cp ../../source/data/dictionaries/* $BUILD_PATH/texiteasy.app/Contents/Resources/dictionaries




macdeployqt $BUILD_PATH/texiteasy.app

mkdir /tmp/texiteasy
rm -r /tmp/texiteasy/*
cp /ApplicationsAlias /tmp/texiteasy/Applications
cp -r $BUILD_PATH/texiteasy.app /tmp/texiteasy/texiteasy.app

hdiutil create /tmp/texiteasy-$1.dmg -ov -volname "TexitEasy $1" -fs HFS+ -srcfolder "/tmp/texiteasy/"
hdiutil convert /tmp/texiteasy-$1.dmg -format UDZO -o ~/Documents/texiteasy-$1_mac.dmg

sudo rm -rf $BUILD_PATH/texiteasy.app
sudo rm -rf /tmp/texiteasy
sudo rm -f /tmp/texiteasy-$1.dmg
