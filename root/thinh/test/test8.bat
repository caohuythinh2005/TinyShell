echo start
set x=0
while (x<4)
{
set i=0
for (i=0;i<3;i=i+1)
{
    echo i dang la : %i%
}
echo x dang la : %x%
set /a x+=1
}
echo end
