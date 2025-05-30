set x=0
while (x < 3)
{
    rem hello
    echo day la lan thu: %x%
    set /a x+=1
    for (j=0;j<4;j+=1)
    {
        echo rat la ... : %j%
    }
    pwd
    cd ..
    cd apps
    runExe countdown.exe -b -c
    cd ..
    cd test
}

if (x < 3)
{
    echo dung
}
else
{
    echo sai
    set y=0
    if (y>-1)
    {
        echo gia tri cua y la : %y%
    }
}