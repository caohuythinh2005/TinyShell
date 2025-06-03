set x=1
set y=2

pwd
cd ..
pwd


if exist apps
{
   echo file exists
   echo
   echo hello
   echo
}
if notexist apps
{
   echo file does not exist
   echo
}

cd test
echo end
echo
