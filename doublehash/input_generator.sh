#!/bin/tcsh
if ($# != 1) then
	echo "Usage: input_generator.sh <number of elements to generate and test>"
	exit 1
endif

foreach x (`seq 0 $1`) #`seq 0 2` = 0 1 2
	set num = `shuf -i 1-512343 -n 1`	
	echo "INSERT $num";
	echo "SEARCH $num";
end
echo "QUIT"
