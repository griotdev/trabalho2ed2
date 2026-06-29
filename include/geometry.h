#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef void Point;
typedef void Rect;

Point *point_create(double x, double y);
void point_destroy(Point *point);

Rect *rect_create(double x, double y, double width, double height);
void rect_destroy(Rect *rect);

#endif
