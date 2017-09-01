#include "drawing.h"
#include <assert.h>
#include <algorithm>
void draw_stacked(cairo_t*cr, guint width, guint height, const std::vector<std::vector<double>>&data, const std::vector<std::array<double, 3>>&colors, double transparency)
{
	auto numlines = data.size();
	if (!numlines)
		return;
	assert(numlines == colors.size());
	auto numpoints = data[0].size();
	if (numpoints<2)
		return;

	//Draw the fillings
	if (transparency > 0) {
		std::vector<double>last_row_height(numpoints);
		for (int i = 0; i < numlines; ++i) {
			cairo_set_source_rgba(cr, colors[i][0], colors[i][1], colors[i][2], transparency);
			cairo_move_to(cr, 0, i == 0 ? height : height - last_row_height[0]*height);
			//Draw the lines forwards at the upper level
			for (int j = 0; j < numpoints; ++j) {
				cairo_line_to(cr, width*j / (numpoints - 1), height - (last_row_height[j] + data[i][j])*height);
			}
			//Draw the line backwards at the lower level
			for (int j = numpoints-1; j>=0; --j) {
				cairo_line_to(cr, width*j / (numpoints - 1), height - last_row_height[j]*height);
				last_row_height[j] += data[i][j];
			}
			cairo_close_path(cr);
			cairo_fill(cr);
		}
	}
	//Draw the lines, the line will draw above the fillings
	std::vector<double>last_row_height(numpoints);
	for (int i = 0; i < numlines; ++i) {
		cairo_set_source_rgba(cr, colors[i][0], colors[i][1], colors[i][2], transparency);
		cairo_move_to(cr, 0, i == 0 ? height : height - last_row_height[0] * height);
		//Draw the lines forwards at the upper level
		for (int j = 0; j < numpoints; ++j) {
			cairo_line_to(cr, width*j / (numpoints - 1), height - (last_row_height[j] + data[i][j])*height);
			last_row_height[j] += data[i][j];
		}
		cairo_stroke(cr);
	}
}
