cd /d "C:\Users\Andrey\source\repos\ScreenDuo\ScreenDuoDriver" &msbuild "ScreenDuoDriver.vcxproj" /t:sdvViewer /p:configuration="Debug" /p:platform="x64" /p:SolutionDir="C:\Users\Andrey\source\repos\ScreenDuo" 
exit %errorlevel% 