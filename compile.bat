@echo off
cls
echo "Compiling... "
set files="src\main.c"
cl /MD /D_CRT_SECURE_NO_WARNINGS %files% -Iexternal\windows\glfw\vc\x64\include -IC:\VulkanSDK\1.2.162.1\Include /link /LIBPATH:external\windows\glfw\vc\x64\lib /LIBPATH:C:\VulkanSDK\1.2.162.1\Lib gdi32.lib kernel32.lib user32.lib shell32.lib glfw3.lib vulkan-1.lib /out:bin/main.exe /DEBUG:none
del *.obj
