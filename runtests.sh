
for i in `ls tests`; do
	echo running test: $i
	
	if [[ ! -f tests/$i/stdin.txt ]]; then
  	 	touch tests/$i/stdin.txt
	fi

	if [[ ! -f tests/$i/args.txt ]]; then
  	 	touch tests/$i/args.txt
	fi

	./ipli -t tests/$i/$i.ipl $(cat tests/$i/args.txt) < tests/$i/stdin.txt > tmpout.txt 
	

	diff tmpout.txt tests/$i/stdout.txt
	if [[ $? != 0 ]]; then
  		exit 1
	fi
	echo
	rm tmpout.txt


done

echo all tests are correct