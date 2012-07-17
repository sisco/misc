Wordgram program.
Brian Sisco
bps16@pitt.edu
March 6th, 2009


INTRODUCTION

This program unscrambles wordgrams (such as "eplhanet" which
unscrambles to "elephant").  It searches through permutations, growing
them from left-to-right from the remaining characters, and prunes the
search when it generates an impossible prefix.  Prefixes and words are
maintained in a hashed existence table for fast lookup. The comments 
in the .java files should be sufficient to explain the specifics.


INSTALLATION

The following files should be present:

     StringTable.java  --- A hashed existence table of Strings.
     StringTable.class --- The class file for the above file.
     Wordgram.java  --- Top-level routines including search.
     Wordgram.class --- The class file for the above file.
     Words.txt  --- Example list of words.

Compile everything with:

     javac Wordgram.java
     javac StringTable.java

using Java 2 1.5 or later.


USAGE

Run with:

     java Wordgram

The program should be self-explanatory.


PROBLEMS

No known problems.





