# Dillon Coffman, Section 1 - Operating Systems Project 2
# CompLab Tutor: Chenglong Fu
# gcc -o outputfile file1.c file2.c file3.c for multiple source files
myshell:
	gcc -o myshell main.c parser.c

test:
	gcc -o testing main.c parser.c -D TESTING
	./testing

clean: 
	rm -rf myshell
	rm -rf testing