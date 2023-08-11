#Margheanu Cristina-Andreea 313CA
CFLAGS=-Wall -Wextra -std=c99

build:
        gcc $(CFLAGS) -g -o vma vma.c main.c
run_vma:
        ./vma
clean:
        rm -r vma
~                    
