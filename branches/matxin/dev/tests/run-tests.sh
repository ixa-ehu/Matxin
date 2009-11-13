for i in *; do
	if [ -d $i ]; then
		cd $i;
		echo -n "Running "$i"..." ;
		diff=`sh run-test.sh | wc -l`;
		if [ $diff -eq 0 ]; then
			echo " pass.";
		else 
			echo " fail.";
		fi
		cd ..;
	fi
done;
