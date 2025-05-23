echo Start script

goto middle

echo This line should be skipped

:middle
echo Reached the middle
pwd
goto end

echo This line should also be skipped

:end
echo End of script