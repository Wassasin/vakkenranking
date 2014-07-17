vakkenranking
=============

Parsing application for Radboud University evaluations, for use by the OLC Informatica.

How to use
==========

```
Parsing application for Radboud University evaluations. [https://github.com/Wassasin/vakkenranking]
Usage: ./vakkenranking [options]

General options:
  -h [ --help ]           display this message
  -t [ --outputtype ] arg {html, csv} (defaults to html)
  -d [ --datadir ] arg    specify in which directory to search for the data 
                          files (default ./data)
```

In the datadir you need the following:
* A file `mapping.csv` mapping new courses to old courses, or with the entry `new`.
```
IPC021,IBI002
IPC019,IPC012
IPC020,new
IBC018,new
```
* A directory `new` containing the `xls` evaluations for the current semester or year, converted to `csv`.
* A directory `old` containing the same evaluations, but for the previous year.

The course codes from both years are matched, but can be overruled by the `mapping.csv` file.
`xls` evaluations can be converted using `libreoffice`:
```
$ libreoffice --headless --convert-to csv $filename --outdir $outdir
```

The `csv` file can be used for the `cloaca` project.
The OLC website contains example vakkenranking `html` files, as for example the [2012 vakkenranking](http://olciii.nl/assets/vakkenranking2012.html).

Dependencies
============

`vakkenranking` can be compiled using at least `Clang 3.2` or higher.

Compilation has been tested with the following setups:
* Ubuntu 13.10, Clang 3.2
* Ubuntu 14.04, Clang 3.4

This software project uses the following packages:
* Boost with {system, filesystem, regex, program_options}

How to build
============

`vakkenranking` can be built using cmake.

```
$ ./cmake-linux
$ cd build
$ make
```
