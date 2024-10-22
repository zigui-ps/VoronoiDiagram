# Voronoi Diagram
Fortune's Algorithm
+ This code has some double precision issues. 
+ Only 150 lines.
+ This code uses splay tree. Worst time complexty is O(n log n).
+ Calculating the Voronoi Diagram with 1 million(1,000,000) random points in 2 seconds (single core, Intel(R) Core(TM) i9-9900K CPU @ 3.60GHz).
+ [Code](teamnote_VoronoiDiagram.cpp)

![Voronoi Diagram](inputs/Voronoi.png)

## Special thanks to
https://jacquesh.github.io/post/fortunes-algorithm/

## build
``` sh build.sh ```

## run
``` ./A input.txt ```

## input file format
n : # of points
```
(n)
(x1) (y1)
(x2) (y2)
...
(xn) (yn)
```

## constraint
+ point must be distinct.
+ n != 0

## function format
Reading ```main.cpp``` rendering part can be helpful to know about function format.
- ```std::vector<pdd> input```
  + Inputs. Order can (and will) be changed, so user should pay attention about this.
  + Additionally, ```input``` will sorted by (y, x) order.
- ```std::vector<pdd> vertex```
  + ```Vertex``` is locations of Voronoi Diagram's intersection points. It can contain same points, and the degree of each point is 3.
  + This code doesn't compress same points. If you want, then you should implement it.
- ```std::vector<pii> edge```
  + ```for(pii c : edge)```, Voronoi Diagram contains line from ```vertex[c.first]``` to ```vertex[c.second]```.
  + Voronoi can contain rays or straight line. In this case, rays are depicted as {-1, (point index)} or {(point index), -1}, and straight line are depicted as {-1, -1}. A direction of each line can calculate using ```area``` array.
- ```std::vector<pii> area```
  + Let (a, b) := i-th element of array ```area```, and (u, v) := i-th element of array ```edge```.
  + Then, Point ```input[a]``` is located CCW of an u->v line (or ray, or straight line), and point ```input[b]``` is located CW of an u->v line.
  + u->v line is a subset of perpendicular bisector of line segment from ```input[a]``` to ```input[b]```.
  + Straight line {a, b}, {-1, -1} through midpoint of ```input[a]``` and ```input[b]```.
  
## GUI
+ Mouse whill operates to change scale.
+ Mouse drag operates to move screen.

## And..
Feel free to ask questions. 

~And sorry for my poor English.~
