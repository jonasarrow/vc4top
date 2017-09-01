#pragma once
#include <vector>
#include <array>
#include <gtk/gtk.h>

/*
Draws a stacked timeline of the matrix data. It assumes equal distant data points.
The first row is the lowest one drawn, the second row is stacked on top if it an so 
on. It assumes, that the sum of all rows does not exceed 1 and every value is positive.
The colors are the colors of the associated timelines. Transparency sets the filling 
transparency for these timelines.
*/
void draw_stacked(cairo_t*cr, guint width, guint height, const std::vector<std::vector<double>>&data, const std::vector<std::array<double, 3>>&colors, double transparency);