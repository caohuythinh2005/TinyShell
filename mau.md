@echo off

rem set biến
set MYVAR=Hello

rem if condition
if "%MYVAR%"=="Hello" (
    echo Variable is Hello
) else (
    echo Variable is not Hello
)

rem goto label
goto :mylabel

echo This line will be skipped.

:mylabel
echo This is a label line.

rem for loop
for %%i in (1 2 3) do (
    echo Loop iteration %%i
)

rem while giả lập (dùng goto)
set /a count=0
:while_loop
if %count% geq 3 goto endwhile
echo Count is %count%
set /a count+=1
goto :while_loop
:endwhile

rem do while giả lập (kiểu 1)
set /a count=0
:do_while_start
echo Count in do-while %count%
set /a count+=1
if %count% lss 3 goto :do_while_start

rem exit
exit /b 0
