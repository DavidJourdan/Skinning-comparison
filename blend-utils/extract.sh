if [[ ! -f script.py ]]
then
    echo "Run from the blend-utils directory"
fi

input=$1
output=$2

echo $output | blender $input --background --python script.py
