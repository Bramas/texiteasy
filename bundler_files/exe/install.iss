; -- Example2.iss --
; Same as Example1.iss, but creates its icon in the Programs folder of the
; Start Menu instead of in a subfolder, and also creates a desktop icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]   
DirExistsWarning=no
;PrivilegesRequired=lowest
;DisableDirPage=yes
UsePreviousAppDir=yes
AppName=TexitEasy          
AppVersion=0.15.0
DefaultDirName={pf}\TexitEasy
; Since no icons will be created in "{group}", we don't need the wizard
; to ask for a Start Menu folder name:
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\TexitEasy.exe
OutputDir=..\Release
LicenseFile=LICENCE.txt
SetupIconFile=texiteasy.ico
ChangesAssociations=yes
[Registry] 
Root: HKCR; Subkey: ".tex"; ValueType: string; ValueName: ""; ValueData: "LatexFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "LatexFile"; ValueType: string; ValueName: ""; ValueData: "LaTeX document"; Flags: uninsdeletekey
Root: HKCR; Subkey: "LatexFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\TexitEasy.exe,0"
Root: HKCR; Subkey: "LatexFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\TexitEasy.exe"" ""%1"""
[Run]
[Files]         
Source: "TexitEasy.exe"; DestDir: "{app}"
Source: "elevate.exe"; DestDir: "{app}"
Source: "texiteasy_upgrade.exe"; DestDir: "{app}"    
Source: "icudt51.dll"; DestDir: "{app}"  
Source: "icuin51.dll"; DestDir: "{app}"  
Source: "icuuc51.dll"; DestDir: "{app}" 
Source: "libbz2-1.dll"; DestDir: "{app}"   
Source: "libcurl-4.dll"; DestDir: "{app}"  
Source: "libfreetype-6.dll"; DestDir: "{app}"     
Source: "libgcc_s_dw2-1.dll"; DestDir: "{app}"       
Source: "libidn-11.dll"; DestDir: "{app}"  
Source: "libintl-8.dll"; DestDir: "{app}" 
Source: "libjbig-2.dll"; DestDir: "{app}" 
Source: "libjpeg-8.dll"; DestDir: "{app}" 
Source: "liblcms2-2.dll"; DestDir: "{app}" 
Source: "liblzma-5.dll"; DestDir: "{app}" 
Source: "libpcre-1.dll"; DestDir: "{app}" 
Source: "libpcre16-0.dll"; DestDir: "{app}" 
Source: "libpng16-16.dll"; DestDir: "{app}" 
Source: "libpoppler-43.dll"; DestDir: "{app}" 
Source: "libpoppler-qt5-1.dll"; DestDir: "{app}"   
Source: "libssh2-1.dll"; DestDir: "{app}" 
Source: "libstdc++-6.dll"; DestDir: "{app}"   
Source: "libtiff-5.dll"; DestDir: "{app}" 
Source: "libwinpthread-1.dll"; DestDir: "{app}"  
Source: "Qt5Core.dll"; DestDir: "{app}" 
Source: "Qt5Gui.dll"; DestDir: "{app}" 
Source: "Qt5Network.dll"; DestDir: "{app}"  
Source: "Qt5Sql.dll"; DestDir: "{app}"   
Source: "Qt5Widgets.dll"; DestDir: "{app}" 
Source: "Qt5Xml.dll"; DestDir: "{app}"  
Source: "Qt5Script.dll"; DestDir: "{app}"      
Source: "zlib1.dll"; DestDir: "{app}"     
Source: "libeay32.dll"; DestDir: "{app}" 
Source: "ssleay32.dll"; DestDir: "{app}"  
Source: "libiconv-2.dll"; DestDir: "{app}" 
Source: "PlugIns/platforms/qminimal.dll"; DestDir: "{app}\PlugIns\platforms"   
Source: "PlugIns/platforms/qwindows.dll"; DestDir: "{app}\PlugIns\platforms"                               
Source: "data/dictionaries/*"; DestDir: "{app}\data\dictionaries"               
                                              
[Icons]
Name: "{commonprograms}\TexitEasy"; Filename: "{app}\TexitEasy.exe"
Name: "{commondesktop}\TexitEasy"; Filename: "{app}\TexitEasy.exe"
