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

#include <cairo.h>
#ifndef CAIRO_HAS_PNG_FUNCTIONS
#error This program requires cairo with PNG support
#endif

#include <stdlib.h>

#define MAX_LABEL_SIZE	64

enum orientation {
	ORIENTATION_HORIZONTAL = 0,
	ORIENTATION_VERTICAL = 1
};

typedef struct _chq_axis_t {
	enum orientation	 orientation;
	double			 size;
	double			 limit_min;
	double			 limit_max;
	/* label style */
	char			*label_fontfamily;
	double			 label_fontsize;
	double			 label_padding;
	cairo_font_slant_t	 label_slant;
	cairo_font_weight_t	 label_weight;
	/* label misc */
	double			 label_max_width;
	double			 label_max_height;
	/* ticks */
	unsigned int		 ticks_count;
	double			*ticks_positions;
	char			**ticks_labels;
	double			 ticks_value_spacing;
} chq_axis_t;

typedef struct _chq_dataplot_t {
	cairo_t		*cr;
	cairo_surface_t	*surface;
	unsigned int	 width;
	unsigned int	 height;
	char		*output_filename;
	/* axes */
	chq_axis_t	*x_axis;
	chq_axis_t	*y_axis;
	/* margins */
	double		 margin_top;
	double		 margin_right;
	double		 margin_bottom;
	double		 margin_left;
} chq_dataplot_t;


/* strlcpy.c */
size_t		 strlcpy(char *, const char *, size_t);

/* axis.c */
chq_axis_t 	*chq_axis_new(void);
chq_axis_t 	*chq_axis_horizontal_new(void);
chq_axis_t 	*chq_axis_vertical_new(void);
void		 chq_axis_kill(chq_axis_t *);
void		 chq_axis_set_limit(chq_axis_t *, double, double);
double		 chq_axis_get_spread(chq_axis_t *);
void		 chq_axis_set_size(chq_axis_t *, double);
double		 chq_axis_convert_to_scale(chq_axis_t *, double);
void		 chq_axis_select_label_fontfamily(chq_axis_t *, cairo_t *);
double		 chq_axis_vertical_get_width(chq_axis_t *);
double		 chq_axis_horizontal_get_height(chq_axis_t *);
char 		*chq_axis_prerender_value(chq_axis_t *, cairo_t *, double, 
			double *, double *, int);
void		 chq_axis_calculate_label_size(chq_axis_t *, cairo_t *);
void		 chq_axis_prerender_ticks(chq_axis_t *, cairo_t *);

/* dataplot.c */
chq_dataplot_t 	*chq_dataplot_new(void);
void		 chq_dataplot_kill(chq_dataplot_t *);
void		 chq_dataplot_get_text_size(cairo_t *, const char *, 
			cairo_font_slant_t, cairo_font_weight_t, double, 
			const char *, double *, double *);
void		 chq_dataplot_render_y_label_text(chq_dataplot_t *, double, 
			char *);
double		 chq_dataplot_get_x_label_y(chq_dataplot_t *);
void		 chq_dataplot_render_x_label_text(chq_dataplot_t *, double, 
			char *);
void		 chq_dataplot_render_y_label_value(chq_dataplot_t *, double);
void		 chq_dataplot_render_y_axis_labels(chq_dataplot_t *);
void		 chq_dataplot_render_x_axis_labels(chq_dataplot_t *);
void		 chq_dataplot_render_axes(chq_dataplot_t *);
void		 chq_dataplot_render(chq_dataplot_t *);
void		 chq_dataplot_set_width(chq_dataplot_t *, unsigned int);
void		 chq_dataplot_set_height(chq_dataplot_t *, unsigned int);
void		 chq_dataplot_set_output_file(chq_dataplot_t *, char *);
