set x=0
while (x < 3)
{
    rem hello
    echo day la lan thu: %x%
    echo
    set /a x+=1
    for (j=0;j<4;j+=1)
    {
        echo rat la ... : %j%
        echo
    }
    pwd
    cd ..
    cd apps
    exec countdown.exe -b -c
    cd ..
    cd test
}


if (x < 3)
{
    echo dung
    echo
}
else
{
    echo sai
    echo
    set y=0
    if (y>-1)
    {
        echo gia tri cua y la : %y%
        echo
    }
}





echo minh
echo
echo thinh
echo
echo ittn
echo
echo okokokokok
echo