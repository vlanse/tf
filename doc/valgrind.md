## Valgrind on OSX High Sierra

Need some customization of build scripts since High Sierra is not supported yet

1. ```git clone git://sourceware.org/git/valgrind.git```
2. ```cd valgrind```
3. Update configure.ac:
```
1. at line 157, 
  before: applellvm-5.1|applellvm-6.*|applellvm-7.*|applellvm-8.*)
  after: applellvm-5.1|applellvm-6.*|applellvm-7.*|applellvm-9.*)
2. at line 415,
  before: 16.*)
  after: 17.*)
```

4. ```./autogen.sh```
5. ```./configure```
6. ```make``` 
7. ```sudo make install```
8. Check valgrind version: ```rehash && valgrind --version```