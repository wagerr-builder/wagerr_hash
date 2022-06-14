#include <Python.h>

#include "wagerr.h"

static PyObject *wagerr_getpowhash(PyObject *self, PyObject *args)
{
    char *output;
    PyObject *value;
#if PY_MAJOR_VERSION >= 3
    PyBytesObject *input;
#else
    PyStringObject *input;
#endif
    if (!PyArg_ParseTuple(args, "S", &input))
        return NULL;
    Py_INCREF(input);
    output = PyMem_Malloc(32);

#if PY_MAJOR_VERSION >= 3
    wagerr_hash((char *)PyBytes_AsString((PyObject*) input), (int)PyBytes_Size((PyObject*) input), output);
#else
    wagerr_hash((char *)PyString_AsString((PyObject*) input), (int)PyString_Size((PyObject*) input), output);
#endif
    Py_DECREF(input);
#if PY_MAJOR_VERSION >= 3
    value = Py_BuildValue("y#", output, 32);
#else
    value = Py_BuildValue("s#", output, 32);
#endif
    PyMem_Free(output);
    return value;
}

static PyMethodDef DashMethods[] = {
    { "getPoWHash", wagerr_getpowhash, METH_VARARGS, "Returns the proof of work hash using wagerr hash" },
    { NULL, NULL, 0, NULL }
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef DashModule = {
    PyModuleDef_HEAD_INIT,
    "wagerr_hash",
    "...",
    -1,
    DashMethods
};

PyMODINIT_FUNC PyInit_wagerr_hash(void) {
    return PyModule_Create(&DashModule);
}

#else

PyMODINIT_FUNC initwagerr_hash(void) {
    (void) Py_InitModule("wagerr_hash", DashMethods);
}
#endif
