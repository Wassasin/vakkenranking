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
* A file `mapping.csv` mapping new courses to old courses (in that order), or with the entry `new`. Note: if you have nothing to map, you must still supply an empty `mapping.csv` file.
```
IPC021,IBI002
IPC019,IPC012
IPC020,new
IBC018,new
```
* A directory `new` containing the `xls` evaluations for the current semester or year, converted to `csv`. Note that these need to be directly below `new`, and not in their individual course directories.
* A directory `old` containing the same evaluations, but for the previous year.

The course codes from both years are matched, but can be overruled by the `mapping.csv` file.
`xls` evaluations can be converted using `libreoffice`:
```
$ libreoffice --headless --convert-to csv $filename --outdir $outdir
```

Converting from the course directory structure to a flat directory with csv files can typically be done as follows:

```
mkdir xls_evals
find directory_with_courses -iname '*.xls' -exec cp {} xls_evals \;
mkdir csv_evals
for i in xls_evals/*; do libreoffice --headless --convert-to csv $i --outdir csv_evals; done
```

The `mapping.csv` file can be used for the `cloaca` project.
The OLC website contains example vakkenranking `html` files, as for example the [2012 vakkenranking](http://olcs.cs.ru.nl/assets/vakkenranking2012.html).

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
