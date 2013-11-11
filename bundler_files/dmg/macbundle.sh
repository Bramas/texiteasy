
cp Info.plist ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app/Contents/Info.plist

sudo macdeployqt ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app

mkdir /tmp/texiteasy
rm -r /tmp/texiteasy/*
cp /ApplicationsAlias /tmp/texiteasy/Applications
cp -r ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app /tmp/texiteasy/texiteasy.app

hdiutil create /tmp/texiteasy-0.6.0.dmg -ov -volname "texiteasy-0.6.0" -fs HFS+ -srcfolder "/tmp/texiteasy/"
hdiutil convert /tmp/texiteasy-0.6.0.dmg -format UDZO -o ~/Documents/texiteasy-0.6.0_mac.dmg

sudo rm -rf ../../../build/Qt_5_1_1_clang_64bit-Release/texiteasy.app
