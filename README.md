# Processing of Radar data
All packages used are included in 3rd_party except [libpng16](http://www.libpng.org/pub/png/libpng.html), that needs to be installed.

## To run:
```
$ mkdir build
$ cmake ..
$ make
$ ./ubimet ${absolute path where the binary is} ${where to save csv}
```
To obtain `longitude` and `latitude` I convert from `(ROW,COL)` using the projection formulas given.

The resulting image I get is:

<img src="https://github.com/hect1995/UBIMET_Challenge/blob/master/result/heatmap.png">

Using Tableau (get the data from C++ and import it as CSV) I get this map:
<img src="https://github.com/hect1995/UBIMET_Challenge/blob/master/result/result_Tableau.png">
