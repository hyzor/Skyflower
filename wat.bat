@echo off

echo Copying files...
md ..\Skyflower_hej
xcopy . ..\Skyflower_hej\ /s /i /y /q
cd ..\Skyflower_hej

echo Removing files...
rd /s /q .git build external include src
del /q .gitignore README.md wat.bat *.vcxproj* *.opensdf *.sdf *.sln *.suo
rd /s /q content\.SyncArchive
rd /s /q content\Tools
del /q /a:H content\.SyncID content\.SyncIgnore
del /q bin\x64\*.pdb bin\x64\*.lib bin\x64\*.exp bin\x64\*.ilk
del /q bin\x86\*.pdb bin\x86\*.lib bin\x86\*.exp bin\x86\*.ilk
