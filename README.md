#Stepwise Interactive Debugger Bison
##Siddhartha Prasad

SIDBison offers the user the ability to debug a bison spec in terms of the
underlying grammar. It is built on top of S Popuri's `iBison` system.

## Use and setup

1. Install ibison
2. `sh compile`
3. `./sidbison bisonspec.y lexerobject.so`

##Setting up iBison on Tufts University Servers

Given the permissions afforded to regular users on Tufts servers, it is
hard to script an `ibison` setup. Installing `ibison` can be pretty painful
on most machines.

1. Go to `www.cs.uic.edu/~spopuri/ibison.html` and download the source tar ball.

2. Run the following commands

    tar -xvjf iBison.tar.bz2
    cd iBison/
    ./configure
    make
    sudo make install  You likely do not have sudo priveliges on the Tufts
                       servers. So skip this step, and keep track of your
                       iBison directory folder. A binary is generated in
                       the src/ directory


----------------------------------------------------------

3. You now want to run the following in a bash subprocess (so you can temporarily set environment variables).

    bash
    export BISON_PKGDATADIR=/h/sprasa02/Desktop/iBison/data #This is my iBison directory


4. Now given a Flex file `lexer.l` and a Bison file `parser.y`

    ./ibison -d parser.y
    flex lexer.l
    gcc -c -fPIC lex.yy.c
    gcc -shared -o lex.so lex.yy.o

    Make sure LD_LIBRARY_PATH includes the directory in which lexer.so is
      available

    ./ibison -i parser.y #bison should refer to your ibison binary
                        mine is at /h/sprasa02/Desktop/iBison/src/bison

    lexer lex.so


5. Now I can run ibison however


