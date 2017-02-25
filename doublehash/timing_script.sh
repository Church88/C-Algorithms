#!/bin/tcsh
foreach x (`seq 0 2`) #`seq 0 2` = 0 1 2
	echo "Running it in mode $x";
	foreach y (hash100k/*%load) #Change this if you name your files something else echo "Running a.out $x < $y"
		a.out $x < $y > /dev/null
	end
end

