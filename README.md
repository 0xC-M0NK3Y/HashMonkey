# HashMonkey
Example of using hashtable with custom hashfunction in C.

## Overview

Hashfunction sources is in hash_func_src :
```c
    uint8_t *hashmonkey(const uint8_t *data, size_t len);
    char *bin2hex(const uint8_t *hash);
```
The two functions allocates memory that you'll need to free.  
  
Hash is in computed in 128 bits (16 bytes) in an uint8_t*.  
You can call bin2hex to see hash value in hex  
    e.g 5407F7C0EB58870B5719B42105391C00 (hash of "0xC_M0NK3Y")
  
You have 3 executables.
- A tester for the quality of the hash function
- A shell command to compute hash using this hash function
- A program that creates a hashtable where you can PUT and GET elements in the database  
(see below for details)
  
### hashmonkey shell command
This hash function is used in the hashmonkey shell command (equivalent to md5sum)
```bash
    $ hashmonkey "0xC_M0NK3Y"
    5407F7C0EB58870B5719B42105391C00
```

### hashtable
This program make an hashtable with collisions handled with simple linked lists where you can add and get and remove elements.  
Types are :  
```
    INT  
    CHAR  
```
Commands are :
```
    PUT "data" TYPE;
    GET "data" TYPE;
    REMOVE "data" TYPE;
    SHOW;
    EXIT;
```
e.g of using:
```
> PUT "0xC_M0NK3Y" CHAR;
DATA INSERTED
> PUT "hello" CHAR;
DATA INSERTED
> PUT "26262626" INT;
DATA INSERTED
> PUT "a26262626" INT;
SYNTAX ERROR NOT AN INTEGER
> GET "bonjour" CHAR;
Not in database
> GET "hello" CHAR;
hello CHAR
> SHOW;
0xC_M0NK3Y CHAR
hello CHAR
26262626 INT
3 total entries
> REMOVE "hello" CHAR;
Removed 1 value
> SHOW;
0xC_M0NK3Y CHAR
26262626 INT
2 total entries
> EXIT;
Exiting...
```

### hashfunc_test
This program tests the quality of the hash function.
```bash
$ ./hashfunc_test 
Usage: ./hashfunc_test <options>

Option can be '"random" <number of hash to test>' or an input file with one string to hash per line (with no blank lines)
(last line must end with '\n', otherwise last line will not be counted)
Depending on the entry testing program ban be long
This does only test the quality of hash
```
You can input a file with one string to hash per line, or you can pass "random" with the number of random value to hash to generate as argument.  
(you can also put -v at the end as argument with file of random mode to see logs of the tester and have the collisions and same values printed in stderr)  
It generates and compute hash for all the values, search for collisions and is looking at the distribution of 0 and 1 of each bit in the hash.  
See the result format in TESTSET_RESULT and RANDOM_RESULT.  
