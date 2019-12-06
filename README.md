# VoronoiDiagram
Fortune's Algorithm (for ICPC)

![Voronoi Diagram](https://github.com/zigui-ps/VoronoiDiagram/blob/master/Voronoi.png)

Code is in ```VoronoiDiagram.cpp```

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
point must be distinct.

## function format
- std::vector<pdd> input
  + Inputs. Order can (and will) be changed, so user should pay attention about this.
  + Additionally, input will sorted by (y, x) order
- std::vector<pdd> vertex
  + Vertex locations of Voronoi Diagram. It could contain same points, and degree of each point is 3.
  + This code doesn't compress same points. If you want, then you should implement it.
- std::vector<pii> edge
  + for(pii c : edge), voronoi contains line from vertex[c.first] to vertex[c.second].
  + Voronoi can contain rays or straight line. In this case, rays are depicted as {-1, (point index)}, {(point index), -1}, and straight line are depicted as {-1, -1}. Direction can be calculated using 'area' array.
- std::vector<pii> area
  + Let (a, b) := i-th element of array 'area', and (u, v) := i-th element of array 'edge'.
  + Point input[a] is located CCW of u->v line (or ray, or straight line), and point input[b] is located CW of u->v line.
  + u->v line is a subset of perpendicular bisector of line segment from input[a] to input[b].
  + Straight line {a, b}, {u, v}(= {-1, -1}) through midpoint of input[a] and input[b].
  
## GUI
Mouse whill operates to change scale.
Mouse drag operates to move screen.

## And..
Feel free to ask questions. 

~And sorry for my poor English~
