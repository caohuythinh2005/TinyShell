

for (i=0; i<2; i+=1)
{
    for (j=0; j<2; j+=1)
    {
        echo Nhap gia tri arr[%i%][%j%]:
        in z
        set arr[%i%][%j%]=%z%
    }
}

for (i=0; i<2; i+=1)
{
    for (j=0; j<2; j+=1)
    {
        echo %arr[%i%][%j%]%
    }
}