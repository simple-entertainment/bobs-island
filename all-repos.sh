lightBlue='\033[1;34m'
noColor='\033[0m'

echo -e "${lightBlue}Executing '"$@"' in all repos!${noColor}"
echo -e "${lightBlue}simplicity${noColor}"
cd lib/simplicity
"$@"
echo -e "${lightBlue}simplicity-bullet${noColor}"
cd ../simplicity-bullet
"$@"
echo -e "${lightBlue}simplicity-glfw${noColor}"
cd ../simplicity-glfw
"$@"
echo -e "${lightBlue}simplicity-opengl${noColor}"
cd ../simplicity-opengl
"$@"
echo -e "${lightBlue}simplicity-terrain${noColor}"
cd ../simplicity-terrain
"$@"
echo -e "${lightBlue}bobs-island${noColor}"
cd ../..
"$@"

