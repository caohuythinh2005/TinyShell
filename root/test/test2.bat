set x=1
set y=2

pwd
cd ..
pwd


if exist apps
{
   echo file exists
   echo hello
}
if notexist apps
{
   echo file does not exist
}

cd test
echo end
