
cp Info.plist ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app/Contents/Info.plist
mkdir ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app/Contents/PlugIns
mkdir ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app/Contents/PlugIns/platforms
cp /Users/quentinbramas/Qt/5.1.1/clang_64/plugins/platforms/libqcocoa.dylib ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app/Contents/PlugIns/platforms/libqcocoa.dylib




sudo macdeployqt ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app

mkdir /tmp/texiteasy
rm -r /tmp/texiteasy/*
cp /ApplicationsAlias /tmp/texiteasy/Applications
cp -r ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app /tmp/texiteasy/texiteasy.app

hdiutil create /tmp/texiteasy-$1.dmg -ov -volname "TexitEasy $1" -fs HFS+ -srcfolder "/tmp/texiteasy/"
hdiutil convert /tmp/texiteasy-$1.dmg -format UDZO -o ~/Documents/texiteasy-$1_mac.dmg

sudo rm -rf ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app
sudo rm -rf /tmp/texiteasy
sudo rm -f /tmp/texiteasy-$1.dmg

