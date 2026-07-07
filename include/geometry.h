#ifndef GEOMETRY_H
#define GEOMETRY_H

/* Primitivas geometricas simples usadas pelos modulos de dominio.
 * Tipos opacos para manter campos internos fora dos headers.
 */
typedef void *Point;
typedef void *Rect;

/* Ponto 2D. */
Point point_create(double x, double y);
void point_destroy(Point point);
double point_x(const Point point);
double point_y(const Point point);

/* Retangulo axis-aligned. Retorna NULL se largura/altura forem negativas. */
Rect rect_create(double x, double y, double width, double height);
void rect_destroy(Rect rect);
double rect_x(const Rect rect);
double rect_y(const Rect rect);
double rect_width(const Rect rect);
double rect_height(const Rect rect);

/* Retorna 1 quando o ponto esta dentro ou na borda do retangulo. */
int rect_contains_point(const Rect rect, double x, double y);

#endif
