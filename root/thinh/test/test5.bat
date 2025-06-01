set x=1
set y=2

pwd
cd ..
pwd


if exist apps && notexist apps
{
   echo file exists
   echo hello
}
if notexist appss
{
   echo file does not exist
}

cd test
echo end