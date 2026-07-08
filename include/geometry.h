#ifndef GEOMETRY_H
#define GEOMETRY_H

/* Primitivas geometricas simples usadas pelos modulos de dominio.
 *
 * Point e Rect sao tipos opacos. Os campos numericos ficam restritos a
 * geometry.c para manter a interface publica baseada em void pointers.
 */
typedef void *Point;
typedef void *Rect;

/* Cria um ponto 2D. Retorna NULL em erro de memoria. */
Point point_create(double x, double y);

/* Libera um ponto. Aceita NULL. */
void point_destroy(Point point);

/* Acessores das coordenadas do ponto; retornam 0.0 para ponteiro NULL. */
double point_x(const Point point);
double point_y(const Point point);

/* Cria um retangulo alinhado aos eixos.
 * width e height devem ser nao negativos. Retorna NULL em erro.
 */
Rect rect_create(double x, double y, double width, double height);

/* Libera um retangulo. Aceita NULL. */
void rect_destroy(Rect rect);

/* Acessores dos campos do retangulo; retornam 0.0 para ponteiro NULL. */
double rect_x(const Rect rect);
double rect_y(const Rect rect);
double rect_width(const Rect rect);
double rect_height(const Rect rect);

/* Retorna 1 quando o ponto esta dentro ou na borda do retangulo. */
int rect_contains_point(const Rect rect, double x, double y);

#endif
