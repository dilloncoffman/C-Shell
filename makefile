all:
	gcc main.c -o main
	## gcc -o outputfile file1.c file2.c file3.c for multiple source files

clean: 
	rm -rf main