## BuildInc

This tool can parse a build id file, increment the number and write it again
let you have many infos to include in your code

## Syntaxe

The syntax is : 

```
Usage : BuidInc [--project] [--file] prefix

Positionnal arguments :
 prefix       prefix of the build id

Optional arguments :
 --project    project of the build id
 --file       file of the build id
```
   
## Include File

by ex :

A file : Build.h 
A project : "Toto"
A prefix : "toto"

will give this result

```
#pragma once

#define Toto_Prefix "toto"
#define Toto_BuildNumber 3
#define Toto_MinorNumber 0
#define Toto_MajorNumber 0
#define Toto_BuildId "0.0.3"

```




