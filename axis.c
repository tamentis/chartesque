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
#include <math.h>

#include "chartesque.h"


/**
 * Constructor for a chq_axis with some sane defaults.
 */
chq_axis_t *
chq_axis_new()
{
	chq_axis_t *axis = malloc(sizeof(chq_axis_t));

	axis->label_fontfamily = strdup("Sans");
	axis->label_fontsize = 10.0;
	axis->label_padding = 4.0;

	axis->ticks_count = 0;
	axis->ticks_positions = NULL;
	axis->ticks_labels = NULL;

	axis->orientation = ORIENTATION_HORIZONTAL;
	axis->size = 0;

	return axis;
}


/**
 * Wrapping constructor for horizontal axis.
 */
chq_axis_t *
chq_axis_horizontal_new()
{
	chq_axis_t *axis = chq_axis_new();
	axis->orientation = ORIENTATION_HORIZONTAL;
	return axis;
}


/**
 * Wrapping constructor for vertical axis.
 */
chq_axis_t *
chq_axis_vertical_new()
{
	chq_axis_t *axis = chq_axis_new();
	axis->orientation = ORIENTATION_VERTICAL;
	return axis;
}


/**
 * Destructor for chq_axis.
 */
void
chq_axis_kill(chq_axis_t *axis)
{
	int i;

	free(axis->label_fontfamily);
	free(axis->ticks_positions);

	for (i = 0; i < axis->ticks_count; i++) {
		free(axis->ticks_labels[i]);
	}
	free(axis->ticks_labels);
	free(axis);
}


/**
 * Set the limit, boundaries of an axis.
 */
void
chq_axis_set_limit(chq_axis_t *axis, double min, double max)
{
	axis->limit_min = min;
	axis->limit_max = max;
}


/**
 * Get the spread, essentially limit max minus limit min.
 */
double
chq_axis_get_spread(chq_axis_t *axis)
{
	return axis->limit_max - axis->limit_min;
}


/**
 * Assign a size to this axis and prepare all the ticks properties.
 */
void
chq_axis_set_size(chq_axis_t *axis, double size)
{
	axis->size = size;

	switch (axis->orientation) {
	case ORIENTATION_VERTICAL:
		axis->ticks_count = (size / (axis->label_padding * 2.0 +
					axis->label_max_height)) / 2;
		break;
	case ORIENTATION_HORIZONTAL:
	default:
		axis->ticks_count = size / (axis->label_fontsize * 5.0);
		break;
	}

	axis->ticks_positions = malloc(sizeof(double) * axis->ticks_count);
	axis->ticks_labels = malloc(sizeof(char *) * axis->ticks_count);
	axis->ticks_value_spacing = chq_axis_get_spread(axis) / 
		(axis->ticks_count - 1);
}


/**
 * Convert a value to a chart coordinate on this axis (excludes the margins
 * or padding from the chart itself).
 */
double
chq_axis_convert_to_scale(chq_axis_t *axis, double value)
{
	double spread = chq_axis_get_spread(axis);
	double value_ratio = (value - axis->limit_min) / spread;

	switch (axis->orientation) {
	case ORIENTATION_VERTICAL:
		return axis->size * (1.0 - value_ratio);
		break;
	case ORIENTATION_HORIZONTAL:
	default:
		return axis->size * value_ratio;
		break;
	}
}


/**
 * Set the label's font family on the provided cairo context.
 */
void
chq_axis_select_label_fontfamily(chq_axis_t *axis, cairo_t *cr)
{
	cairo_select_font_face(cr, axis->label_fontfamily,
			   CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
}


/**
 * Return the width of a vertical axis on the canvas, the result for 
 * horizontal axes is undetermined. This is the maximum size of the labels
 * with padding.
 */
double
chq_axis_vertical_get_width(chq_axis_t *axis)
{
	return axis->label_max_width + axis->label_padding * 2.0;
}


/**
 * Return the height of an horizontal axis on the canvas, the result for
 * vertical axes is not undetermined. This is the maximum height of the labels
 * with padding.
 */
double
chq_axis_horizontal_get_height(chq_axis_t *axis)
{
	return axis->label_max_height + axis->label_padding * 2.0;
}

