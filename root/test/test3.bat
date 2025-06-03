set x=1
set y=2222


while (x < 128 && y < 5555)
{
    set /a x*=2
    set /a y*=2
    echo %x% : %y%
    echo
}
echo end
echo