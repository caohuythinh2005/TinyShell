:test danang

echo Nhap n = 
in z
set n=%z%

for (i=0;i<n;i+=1)
{
    echo Nhap gia tri a[%i%]:
    in z
    set a[%i%]=%z%
}
set i=0
set j=1
for (i=0;i<n;i+=1)
{
    set /a j=i+1
    for (j=i+1;j<n;j+=1)
    {
        set f1=%a[%i%]%
        set f2=%a[%j%]%
        if (f1>f2)
        {
            set tmp=%f1%
            set f1=%f2%
            set f2=%tmp%
            set a[%i%]=%f1%
            set a[%j%]=%f2%
        }
    }
}
echo gia tri nhap theo tang dan:
for (i=0;i<n;i+=1)
{
    echo %a[%i%]%
}