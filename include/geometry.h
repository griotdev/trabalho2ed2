#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef void *Point;
typedef void *Rect;

Point point_create(double x, double y);
void point_destroy(Point point);
double point_x(const Point point);
double point_y(const Point point);

Rect rect_create(double x, double y, double width, double height);
void rect_destroy(Rect rect);
double rect_x(const Rect rect);
double rect_y(const Rect rect);
double rect_width(const Rect rect);
double rect_height(const Rect rect);
int rect_contains_point(const Rect rect, double x, double y);

#endif
