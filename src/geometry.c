#include "geometry.h"

#include <stdlib.h>

typedef struct {
    double x;
    double y;
} PointData;

typedef struct {
    double x;
    double y;
    double width;
    double height;
} RectData;

Point point_create(double x, double y) {
    PointData *point = malloc(sizeof(PointData));

    if (point == NULL) {
        return NULL;
    }

    point->x = x;
    point->y = y;
    return point;
}

void point_destroy(Point point) {
    free(point);
}

double point_x(const Point point) {
    const PointData *data = point;

    return data == NULL ? 0.0 : data->x;
}

double point_y(const Point point) {
    const PointData *data = point;

    return data == NULL ? 0.0 : data->y;
}

Rect rect_create(double x, double y, double width, double height) {
    RectData *rect;

    if (width < 0.0 || height < 0.0) {
        return NULL;
    }

    rect = malloc(sizeof(RectData));
    if (rect == NULL) {
        return NULL;
    }

    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    return rect;
}

void rect_destroy(Rect rect) {
    free(rect);
}

double rect_x(const Rect rect) {
    const RectData *data = rect;

    return data == NULL ? 0.0 : data->x;
}

double rect_y(const Rect rect) {
    const RectData *data = rect;

    return data == NULL ? 0.0 : data->y;
}

double rect_width(const Rect rect) {
    const RectData *data = rect;

    return data == NULL ? 0.0 : data->width;
}

double rect_height(const Rect rect) {
    const RectData *data = rect;

    return data == NULL ? 0.0 : data->height;
}

int rect_contains_point(const Rect rect, double x, double y) {
    const RectData *data = rect;

    if (data == NULL) {
        return 0;
    }

    return x >= data->x
        && x <= data->x + data->width
        && y >= data->y
        && y <= data->y + data->height;
}
