sort_test:
	gcc -std=gnu99 -g -Wall main.c -o sort_test

clean:
	rm *.o sort_test
	echo clean done
