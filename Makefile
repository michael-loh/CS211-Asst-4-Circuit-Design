truthtable: truthtable.c
	gcc -g -Wall -Werror -fsanitize=address -lm -o truthtable truthtable.c

clean:
	rm -f truthtable
