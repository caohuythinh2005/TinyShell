set x=1
set y=2
while ((1+x)*x < 128 && y < 4048) {
    echo ff : x = %x% va y = %y%
    set /a x=x*2
    set /a y=y*2
    time
}
echo end