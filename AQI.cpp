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

// int main(int argc, char *argv[]) {
//     // Convert argv[0] from char* to wchar_t*
//     size_t size = mbstowcs(NULL, argv[0], 0);
//     wchar_t *wideArgv0 = (wchar_t *)malloc((size + 1) * sizeof(wchar_t));
// 
//     Py_SetProgramName(wideArgv0);
//     Py_Initialize();
//     std::cout << add(1) << std::endl;
//     std::cout << fetch_aqi_data_from_xml("http://www.stateair.net/web/rss/1/1.xml") << std::endl;
//     Py_Finalize();
//     // int temp;
//     // std::cin >> temp;
//     return 0;
// }