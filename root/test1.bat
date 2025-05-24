set x=0
while (x < 3)
{
    echo day la lan thu: %x%
    set /a x+=1
    for (j=0;j<4;j+=1)
    {
        echo rat la ... : %j%
    }
}

if (x < 3)
{
    echo dung
}
else
{
    echo sai
}