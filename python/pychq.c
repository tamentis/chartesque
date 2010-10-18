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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <Python.h>

#include "chartesque.h"

static PyObject *PyCHQError;

char *fgetln(FILE *, size_t *);
void *memrchr(const void *, int, size_t);

/**
 * Add a new record to the record set typle.
 */
void
add_record(PyObject *timestamps, PyObject *levels, unsigned int idx, time_t ts,
		float gallons)
{
	PyList_SetItem(timestamps, idx, PyInt_FromLong(ts));
	PyList_SetItem(levels, idx, PyFloat_FromDouble(gallons));
}

static PyObject *
pychq_plot(PyObject *self, PyObject *args)
{
	PyObject *data_x_list, *data_y_list;
	PyObject *item;
	size_t i, data_len = 0;
	double *data_x, *data_y;

	if (!PyArg_ParseTuple(args, "O!O!", &PyList_Type, &data_x_list,
				&PyList_Type, &data_y_list))
		return NULL;

	data_len = PyList_Size(data_x_list);
	data_x = malloc(sizeof(double) * data_len);
	data_y = malloc(sizeof(double) * data_len);

	/* TODO: Error if two lists have different size */

	/* TODO: Error if one of the item is not a float */
	for (i = 0; i < data_len; i++) {
		item = PyList_GetItem(data_x_list, i);
		data_x[i] = PyFloat_AsDouble(item);
		item = PyList_GetItem(data_y_list, i);
		data_y[i] = PyFloat_AsDouble(item);
	}

	chq_dataplot_t *chart = chq_dataplot_new();
	chq_dataplot_set_width(chart, 640);
	chq_dataplot_set_height(chart, 280);
	chq_dataplot_set_output_file(chart, "stuff.png");

	chq_dataplot_set_data(chart, data_x, data_y, data_len);

	chq_axis_set_limit(chart->x_axis, 200, 2000);
	chq_axis_set_limit(chart->y_axis, 1, 50);

	chq_dataplot_render(chart);

	chq_dataplot_kill(chart);

	Py_RETURN_TRUE;
}


static PyMethodDef PyCHQMethods[] = {
	{"plot",  pychq_plot, METH_VARARGS, 
		"Test func to run chq on a list."},
	{NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC
initpychq(void)
{
	PyObject *m;

	m = Py_InitModule("pychq", PyCHQMethods);
	if (m == NULL)
		return;

	PyCHQError = PyErr_NewException("pychq.error", NULL, NULL);
	Py_INCREF(PyCHQError);
	PyModule_AddObject(m, "error", PyCHQError);
}

