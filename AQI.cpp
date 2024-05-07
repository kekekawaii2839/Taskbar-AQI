#include "AQI.h"
#include <Python.h>

// https://www.zhihu.com/question/23003213/answer/23326530

AQI::AQI(const char* url) {
    this->url = (char*)url;
}

bool AQI::fetch_24_hours_data() {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pRetVal;

    Py_Initialize();
    pName = PyUnicode_DecodeFSDefault("getRSS");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (pModule == NULL) {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", "aqi");
        return 1;
    }

    pFunc = PyObject_GetAttrString(pModule, "fetch_aqi_data_from_xml");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        if (PyErr_Occurred())
            PyErr_Print();
        fprintf(stderr, "Cannot find function \"%s\"\n", "fetch_aqi_data");
    }

    pArgs = PyTuple_Pack(1, PyUnicode_DecodeFSDefault(url));
    pRetVal = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);
    if (pRetVal == NULL) {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        PyErr_Print();
        fprintf(stderr, "Call failed\n");
        return 1;
    }

    Py_ssize_t size = PyList_Size(pRetVal);
    // printf("Result of call:\n");
    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject* item = PyList_GetItem(pRetVal, i);
        if (!PyDict_Check(item)) {
            continue;
        }

        timestamp time;
        AQIData temp;

        PyObject *pKey, *pValue;
        Py_ssize_t pos = 0;
        while (PyDict_Next(item, &pos, &pKey, &pValue)) {
            std::string key_str = PyUnicode_AsUTF8(pKey);
            #ifdef DEBUG
            std::cout << "key: " << key_str << std::endl;
            #endif
            if (PyLong_Check(pValue) && (key_str=="pm25_aqi")) {
                #ifdef DEBUG
                std::cout << "is long " << PyLong_AsLong(pValue) << std::endl;
                #endif
                temp.pm25 = PyLong_AsLong(pValue);
            }
            else if (PyTuple_Check(pValue) && (key_str=="timestamp")) {
                #ifdef DEBUG
                std::cout << "is tuple" << std::endl;
                #endif
                time.year = PyLong_AsLong(PyTuple_GetItem(pValue, 0));
                time.month = PyLong_AsLong(PyTuple_GetItem(pValue, 1));
                time.day = PyLong_AsLong(PyTuple_GetItem(pValue, 2));
                time.hour = PyLong_AsLong(PyTuple_GetItem(pValue, 3));
                #ifdef DEBUG
                std::cout << "time:" << time.year << "-" \
                    << time.month << "-" << time.day << std::endl;
                #endif
            }
            else{
                #ifdef DEBUG
                std::cout << "nooooooooo" << std::endl;
                #endif
            }
            temp.time = time;
            aqiData.push_back(temp);
            #ifdef DEBUG
            std::cout << "=========================================" << std::endl;
            #endif
        }
    }

    Py_DECREF(pRetVal);    
    Py_XDECREF(pFunc);
    Py_DECREF(pModule);

    Py_Finalize();
    return 0;
}

int AQI::get_latest_pm25() {
    return aqiData.back().pm25;
}

D2D1::ColorF AQI::get_pm25_color(int pm25) {
    if (pm25 >=0 && pm25 <= 50) {
        return D2D1::ColorF(0, 255, 0, 1);
    }
    else if (pm25 >= 51 && pm25 <= 100) {
        return D2D1::ColorF(255, 255, 0, 1);
    }
    else if (pm25 >= 101 && pm25 <= 150) {
        return D2D1::ColorF(255, 126, 0, 1);
    }
    else if (pm25 >= 151 && pm25 <= 200) {
        return D2D1::ColorF(255, 0, 0, 1);
    }
    else if (pm25 >= 201 && pm25 <= 300) {
        return D2D1::ColorF(153, 0, 76, 1);
    }
    else if (pm25 >= 301 && pm25 <= 500) {
        return D2D1::ColorF(76, 0, 38, 1);
    }
    else if (pm25 >= 501) {
        return D2D1::ColorF(87, 87, 87, 1);
    }
    else {
        return D2D1::ColorF(255, 255, 255, 1);
    }
}