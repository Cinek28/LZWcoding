@echo off

@rem Download opencv version 3.3.1 executable file.
git clone https://github.com/HBuczynski/opencv_exe.git

IF not exist opencv_exe\opencv-3.3.1-vc14.exe (
	
	echo Executable file was not downloaded.
	
) ELSE (
	cd opencv_exe
	copy *.exe ..\
	cd ..
	rmdir /s /q opencv_exe

	@rem Install opencv.
	.\opencv-3.3.1-vc14.exe

	@rem Create folders:
	mkdir x64
	mkdir x64\Debug
	mkdir x64\Release

	copy opencv\build\x64\vc14\bin\opencv_world331d.dll x64\Debug
	copy opencv\build\x64\vc14\bin\opencv_world331.dll x64\Release

	del opencv-3.3.1-vc14.exe
)