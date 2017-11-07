#include "UniformGetSetters.hpp"

#include "Types.hpp"

PyObject * MGLUniform_invalid_getter(MGLUniform * self) {
	MGLError_Set("cannot detect uniform type");
	return 0;
}

PyObject * MGLUniform_bool_value_getter(MGLUniform * self) {
	int value = 0;
	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, &value);
	return PyBool_FromLong(value);
}

PyObject * MGLUniform_int_value_getter(MGLUniform * self) {
	int value = 0;
	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, &value);
	return PyLong_FromLong(value);
}

PyObject * MGLUniform_uint_value_getter(MGLUniform * self) {
	unsigned value = 0;
	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, &value);
	return PyLong_FromUnsignedLong(value);
}

PyObject * MGLUniform_float_value_getter(MGLUniform * self) {
	float value = 0;
	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, &value);
	return PyFloat_FromDouble(value);
}

PyObject * MGLUniform_double_value_getter(MGLUniform * self) {
	double value = 0;
	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, &value);
	return PyFloat_FromDouble(value);
}

PyObject * MGLUniform_sampler_value_getter(MGLUniform * self) {
	int value = 0;
	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, &value);
	return PyLong_FromLong(value);
}

PyObject * MGLUniform_bool_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {
		int value = 0;
		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, &value);
		PyList_SET_ITEM(lst, i, PyBool_FromLong(value));
	}

	return lst;
}

PyObject * MGLUniform_int_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {
		int value = 0;
		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, &value);
		PyList_SET_ITEM(lst, i, PyLong_FromLong(value));
	}

	return lst;
}

PyObject * MGLUniform_uint_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {
		unsigned value = 0;
		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, &value);
		PyList_SET_ITEM(lst, i, PyLong_FromUnsignedLong(value));
	}

	return lst;
}

PyObject * MGLUniform_float_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {
		float value = 0;
		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, &value);
		PyList_SET_ITEM(lst, i, PyFloat_FromDouble(value));
	}

	return lst;
}

PyObject * MGLUniform_double_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {
		double value = 0;
		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, &value);
		PyList_SET_ITEM(lst, i, PyFloat_FromDouble(value));
	}

	return lst;
}

PyObject * MGLUniform_sampler_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {
		int value = 0;
		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, &value);
		PyList_SET_ITEM(lst, i, PyLong_FromLong(value));
	}

	return lst;
}

template <int N>
PyObject * MGLUniform_bvec_value_getter(MGLUniform * self) {
	int values[N] = {};

	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, values);

	PyObject * res = PyTuple_New(N);

	for (int i = 0; i < N; ++i) {
		PyTuple_SET_ITEM(res, i, PyBool_FromLong(values[i]));
	}

	return res;
}

template <int N>
PyObject * MGLUniform_ivec_value_getter(MGLUniform * self) {
	int values[N] = {};

	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, values);

	PyObject * res = PyTuple_New(N);

	for (int i = 0; i < N; ++i) {
		PyTuple_SET_ITEM(res, i, PyLong_FromLong(values[i]));
	}

	return res;
}

template <int N>
PyObject * MGLUniform_uvec_value_getter(MGLUniform * self) {
	unsigned values[N] = {};

	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, values);

	PyObject * res = PyTuple_New(N);

	for (int i = 0; i < N; ++i) {
		PyTuple_SET_ITEM(res, i, PyLong_FromUnsignedLong(values[i]));
	}

	return res;
}

template <int N>
PyObject * MGLUniform_vec_value_getter(MGLUniform * self) {
	float values[N] = {};

	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, values);

	PyObject * res = PyTuple_New(N);

	for (int i = 0; i < N; ++i) {
		PyTuple_SET_ITEM(res, i, PyFloat_FromDouble(values[i]));
	}

	return res;
}

template <int N>
PyObject * MGLUniform_dvec_value_getter(MGLUniform * self) {
	double values[N] = {};

	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, values);

	PyObject * res = PyTuple_New(N);

	for (int i = 0; i < N; ++i) {
		PyTuple_SET_ITEM(res, i, PyFloat_FromDouble(values[i]));
	}

	return res;
}

template <int N>
PyObject * MGLUniform_bvec_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {
		int values[N] = {};
		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, values);

		PyObject * tuple = PyTuple_New(N);

		for (int j = 0; j < N; ++j) {
			PyTuple_SET_ITEM(tuple, j, PyBool_FromLong(values[j]));
		}

		PyList_SET_ITEM(lst, i, tuple);
	}

	return lst;
}

template <int N>
PyObject * MGLUniform_ivec_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {

		int values[N] = {};

		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, values);

		PyObject * tuple = PyTuple_New(N);

		for (int j = 0; j < N; ++j) {
			PyTuple_SET_ITEM(tuple, j, PyLong_FromLong(values[j]));
		}

		PyList_SET_ITEM(lst, i, tuple);
	}

	return lst;
}

template <int N>
PyObject * MGLUniform_uvec_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {

		unsigned values[N] = {};

		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, values);

		PyObject * tuple = PyTuple_New(N);

		for (int j = 0; j < N; ++j) {
			PyTuple_SET_ITEM(tuple, j, PyLong_FromUnsignedLong(values[j]));
		}

		PyList_SET_ITEM(lst, i, tuple);
	}

	return lst;
}

template <int N>
PyObject * MGLUniform_vec_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {

		float values[N] = {};

		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, values);

		PyObject * tuple = PyTuple_New(N);

		for (int j = 0; j < N; ++j) {
			PyTuple_SET_ITEM(tuple, j, PyFloat_FromDouble(values[j]));
		}

		PyList_SET_ITEM(lst, i, tuple);
	}

	return lst;
}

template <int N>
PyObject * MGLUniform_dvec_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {

		double values[N] = {};

		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, values);

		PyObject * tuple = PyTuple_New(N);

		for (int j = 0; j < N; ++j) {
			PyTuple_SET_ITEM(tuple, j, PyFloat_FromDouble(values[j]));
		}

		PyList_SET_ITEM(lst, i, tuple);
	}

	return lst;
}

template <typename T, int N, int M>
PyObject * MGLUniform_matrix_value_getter(MGLUniform * self) {
	T values[N * M] = {};

	((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location, values);

	PyObject * tuple = PyTuple_New(N * M);

	for (int i = 0; i < N * M; ++i) {
		PyTuple_SET_ITEM(tuple, i, PyFloat_FromDouble(values[i]));
	}

	return tuple;
}

template <typename T, int N, int M>
PyObject * MGLUniform_matrix_array_value_getter(MGLUniform * self) {
	int size = self->array_length;

	PyObject * lst = PyList_New(size);
	for (int i = 0; i < size; ++i) {
		T values[N * M] = {};

		((gl_uniform_reader_proc)self->gl_value_reader_proc)(self->program_obj, self->location + i, values);

		PyObject * tuple = PyTuple_New(N * M);

		for (int j = 0; j < N * M; ++j) {
			PyTuple_SET_ITEM(tuple, j, PyFloat_FromDouble(values[j]));
		}

		PyList_SET_ITEM(lst, i, tuple);
	}

	return lst;
}

template PyObject * MGLUniform_bvec_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_bvec_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_bvec_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_ivec_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_ivec_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_ivec_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_uvec_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_uvec_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_uvec_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_vec_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_vec_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_vec_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_dvec_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_dvec_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_dvec_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_bvec_array_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_bvec_array_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_bvec_array_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_ivec_array_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_ivec_array_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_ivec_array_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_uvec_array_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_uvec_array_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_uvec_array_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_vec_array_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_vec_array_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_vec_array_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_dvec_array_value_getter<2>(MGLUniform * self);
template PyObject * MGLUniform_dvec_array_value_getter<3>(MGLUniform * self);
template PyObject * MGLUniform_dvec_array_value_getter<4>(MGLUniform * self);

template PyObject * MGLUniform_matrix_value_getter<float, 2, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<float, 2, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<float, 2, 4>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<float, 3, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<float, 3, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<float, 3, 4>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<float, 4, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<float, 4, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<float, 4, 4>(MGLUniform * self);

template PyObject * MGLUniform_matrix_value_getter<double, 2, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<double, 2, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<double, 2, 4>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<double, 3, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<double, 3, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<double, 3, 4>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<double, 4, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<double, 4, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_value_getter<double, 4, 4>(MGLUniform * self);

template PyObject * MGLUniform_matrix_array_value_getter<float, 2, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<float, 2, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<float, 2, 4>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<float, 3, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<float, 3, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<float, 3, 4>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<float, 4, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<float, 4, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<float, 4, 4>(MGLUniform * self);

template PyObject * MGLUniform_matrix_array_value_getter<double, 2, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<double, 2, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<double, 2, 4>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<double, 3, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<double, 3, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<double, 3, 4>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<double, 4, 2>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<double, 4, 3>(MGLUniform * self);
template PyObject * MGLUniform_matrix_array_value_getter<double, 4, 4>(MGLUniform * self);
