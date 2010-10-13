/*
 * Copyright (c) 2010, Bertrand Janin <tamentis@neopulsar.org>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <string.h>
#include <cairo.h>
#ifndef CAIRO_HAS_PNG_FUNCTIONS
#error This program requires cairo with PNG support
#endif
#include <math.h>

#include "chartesque.h"

#define MAX_LABEL_SIZE	64

typedef struct _dataplot_chart_t {
	cairo_t		*cr;
	cairo_surface_t	*surface;
	unsigned int	 width;
	unsigned int	 height;
	char		*output_filename;
	/* x-axis */
	char		*x_label_fontfamily;
	double		 x_label_fontsize;
	double		 x_label_padding;
	double		 x_label_max_width;
	double		 x_label_max_height;
	double		 x_limit_min;
	double		 x_limit_max;
	unsigned int	 x_ticks_count;
	double		*x_ticks_positions;
	char	       **x_ticks_labels;
	/* y-axis */
	char		*y_label_fontfamily;
	double		 y_label_fontsize;
	double		 y_label_padding;
	double		 y_label_max_width;
	double		 y_label_max_height;
	double		 y_limit_min;
	double		 y_limit_max;
	unsigned int	 y_ticks_count;
	double		*y_ticks_positions;
	double	       **y_ticks_labels;
	/* margins */
	double		 margin_top;
	double		 margin_right;
	double		 margin_bottom;
	double		 margin_left;
} dataplot_chart_t;

/**
 * Given an exact font definition and an UTF-8 string, return the estimated
 * width of the piece of text.
 */
double
get_text_width(cairo_t *cr, const char *family, cairo_font_slant_t slant,
		cairo_font_weight_t weight, double size, const char *utf8)
{
	cairo_text_extents_t extents;

	cairo_save(cr);
	cairo_select_font_face(cr, family, slant, weight);
	cairo_set_font_size(cr, size);
	cairo_text_extents(cr, utf8, &extents);
	cairo_restore(cr);

	return extents.width;
}

/**
 * Given an exact font definition and an UTF-8 string, return the estimated
 * height of the piece of text.
 */
double
get_text_height(cairo_t *cr, const char *family, cairo_font_slant_t slant,
		cairo_font_weight_t weight, double size, const char *utf8)
{
	cairo_text_extents_t extents;

	cairo_save(cr);
	cairo_select_font_face(cr, family, slant, weight);
	cairo_set_font_size(cr, size);
	cairo_text_extents(cr, utf8, &extents);
	cairo_restore(cr);

	return extents.height;
}

/**
 * Set the limit, boundaries of the y axis.
 */
void
dataplot_chart_set_y_limit(dataplot_chart_t *chart, double min, double max)
{
	chart->y_limit_min = min;
	chart->y_limit_max = max;


}

/**
 * Used by cairo to save to file.
 */
static cairo_status_t
stdio_write (void *closure, const unsigned char *data, unsigned int length)
{
	FILE *file = closure;

	if (fwrite (data, 1, length, file) == length) {
		return CAIRO_STATUS_SUCCESS;
	} else {
		return CAIRO_STATUS_WRITE_ERROR;
	}
}


/**
 * Constructor for a dataplot_chart with some sane defaults.
 */
dataplot_chart_t *
dataplot_chart_new()
{
	dataplot_chart_t *chart = malloc(sizeof(dataplot_chart_t));

	chart->width = 800;
	chart->height = 600;
	chart->output_filename = strdup("output.png");

	chart->x_label_fontfamily = strdup("Sans");
	chart->x_label_fontsize = 10.0;

	chart->y_label_fontfamily = strdup("Sans");
	chart->y_label_fontsize = 10.0;
	chart->y_label_padding = 4.0;

	chart->margin_top = 10.0;
	chart->margin_right = 10.0;
	chart->margin_bottom = 10.0;
	chart->margin_left = 10.0;

	return chart;
}

/**
 * Destructor for a dataplot_chart.
 */
void
dataplot_chart_kill(dataplot_chart_t *chart)
{
	free(chart->output_filename);
	free(chart);
}

/**
 * Render a label for the y-axis, they are always right-aligned.
 */
void
dataplot_chart_render_y_label_text(dataplot_chart_t *chart, double y, char *text)
{
	cairo_text_extents_t extents;

	cairo_text_extents(chart->cr, text, &extents);

	printf("y=%f\n", chart->margin_top + y);

	cairo_set_font_size(chart->cr, chart->y_label_fontsize);
	cairo_move_to(chart->cr, chart->margin_left + chart->y_label_padding +
			chart->y_label_max_width - extents.width,
			chart->margin_top + chart->y_label_padding + y);
	cairo_text_path(chart->cr, text);
}

/**
 * Convert an actual data value to a y coordinate for drawing. This excludes
 * all margin and padding calculations, it outputs the coordinates from the
 * axis itself.
 */
double
dataplot_chart_convert_to_y_scale(dataplot_chart_t *chart, double value)
{
	double y_axis_height = chart->height - chart->margin_top -
		chart->margin_bottom;
	double y_limit_spread = chart->y_limit_max - chart->y_limit_min;

	double value_ratio = (value - chart->y_limit_min) / y_limit_spread;

	return y_axis_height - y_axis_height * value_ratio;
}

/**
 * Render a value to the y-axis.
 */
void
dataplot_chart_render_y_label_value(dataplot_chart_t *chart, double value)
{
	double y = dataplot_chart_convert_to_y_scale(chart, value);
	dataplot_chart_render_y_label_text(chart, y, "1.0");
}

void
dataplot_chart_render_y_axis_labels(dataplot_chart_t *chart)
{
	unsigned int i;

	cairo_select_font_face (chart->cr, chart->x_label_fontfamily,
			   CAIRO_FONT_SLANT_NORMAL,
			   CAIRO_FONT_WEIGHT_BOLD);

	for (i = 0; i < chart->y_ticks_count; i++) {
		dataplot_chart_render_y_label_text(chart,
				chart->y_ticks_positions[i],
				chart->y_ticks_labels[i]);
	}
}

void
dataplot_chart_render_y_axis_ticks(dataplot_chart_t *chart)
{
	unsigned int i;
	size_t len;
	double width, height, max_label_width = 0.0, max_label_height = 0.0;
	char lbuffer[MAX_LABEL_SIZE];

	/* This is the size of the y-axis line */
	double y_axis_height = chart->height - chart->margin_top -
		chart->margin_bottom;

	double y_limit_spread = chart->y_limit_max - chart->y_limit_min;

	/* This is the number of labels we would like to have shown */
	chart->y_ticks_count = (y_axis_height / (chart->y_label_padding * 2.0 +
				chart->y_label_max_height)) / 2;

	chart->y_ticks_positions =
		malloc(sizeof(double) * chart->y_ticks_count);
	chart->y_ticks_labels = malloc(sizeof(char *) * chart->y_ticks_count);

	double y_tick_value_spacing = y_limit_spread /
			(chart->y_ticks_count - 1);

	double y_value;
	for (i = 0; i < chart->y_ticks_count; i++) {
		y_value = chart->y_limit_min +
			(double)i * y_tick_value_spacing;
		snprintf(lbuffer, MAX_LABEL_SIZE, "%.1f", y_value);
		width = get_text_width(chart->cr, chart->y_label_fontfamily,
				CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD,
				chart->y_label_fontsize, lbuffer);
		height = get_text_height(chart->cr, chart->y_label_fontfamily,
				CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD,
				chart->y_label_fontsize, lbuffer);

		if (width > max_label_width)
			max_label_width = width;
		if (height > max_label_height)
			max_label_height = height;

		chart->y_ticks_positions[i] = 
			dataplot_chart_convert_to_y_scale(chart, y_value);
		chart->y_ticks_labels[i] = strdup(lbuffer);
	}

	/* Define label max label sizes */
	chart->y_label_max_width = max_label_width;
	chart->y_label_max_height = max_label_height;
}

/**
 * Routine drawing the axes.
 */
void
dataplot_chart_render_axes(dataplot_chart_t *chart)
{
	/* Generate the ticks (positions and labels) */
	dataplot_chart_render_y_axis_ticks(chart);

	/* Select axes color */
	cairo_set_source_rgb (chart->cr, 0.2, 0.2, 0.2);
	cairo_set_line_width(chart->cr, 2);

	/* Draw axes */
	cairo_new_path(chart->cr);
	cairo_move_to(chart->cr, chart->margin_left + chart->y_label_max_width +
			chart->y_label_padding * 2.0, chart->margin_top);
	cairo_line_to(chart->cr, chart->margin_left + chart->y_label_max_width +
			chart->y_label_padding * 2.0, chart->height -
			chart->margin_bottom);
	cairo_line_to(chart->cr, chart->width - chart->margin_right,
			chart->height - chart->margin_bottom);
	cairo_stroke(chart->cr);

	dataplot_chart_render_y_axis_labels(chart);

	cairo_set_source_rgb(chart->cr, 0, 0, 0);
	cairo_fill(chart->cr);
	cairo_stroke(chart->cr);
}


/**
 * Render the dataplot_chart.
 */
void
dataplot_chart_render(dataplot_chart_t *chart)
{
	FILE *fp;
	double data_x[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	double data_y[] = { 0.1, 0.2, 0.1, 0.1, 0.2, 0.3, 0.35, 0.4, 0.35, 0.3, 0.2 };

	chart->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
			chart->width, chart->height);
	chart->cr = cairo_create(chart->surface);

	fp = fopen(chart->output_filename, "w");

	dataplot_chart_render_axes(chart);

	cairo_surface_write_to_png_stream(chart->surface, stdio_write, fp);
	cairo_destroy(chart->cr);
	cairo_surface_destroy(chart->surface);
	fclose(fp);
}

void
dataplot_chart_set_width(dataplot_chart_t *chart, unsigned int width)
{
	chart->width = width;
}

void
dataplot_chart_set_height(dataplot_chart_t *chart, unsigned int height)
{
	chart->height = height;
}

void
dataplot_chart_set_output_file(dataplot_chart_t *chart, char *filename)
{
	free(chart->output_filename);
	chart->output_filename = strdup(filename);
}

void
draw_y_label(cairo_t *cr, double y, const char *utf8)
{
}

int
main (int argc, char *argv[])
{
	cairo_text_extents_t extents;
	cairo_pattern_t *pattern;

	dataplot_chart_t *chart = dataplot_chart_new();
	dataplot_chart_set_width(chart, 800);
	dataplot_chart_set_height(chart, 200);
	dataplot_chart_set_output_file(chart, "stuff.png");
	dataplot_chart_set_y_limit(chart, 1, 50);

	dataplot_chart_render(chart);

	dataplot_chart_kill(chart);

	return 0;
}
