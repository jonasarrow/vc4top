/*
vc4top show utilization of Broadcom's VC4 GPU.
Copyright (C) 2017 Jonas Pfeil

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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