# Dillon Coffman, Section 1 - Operating Systems Project 2
# CompLab Tutor: Chenglong Fu
# gcc -o outputfile file1.c file2.c file3.c for multiple source files
myshell:
	gcc main.c -o myshell

clean: 
	rm -rf myshell