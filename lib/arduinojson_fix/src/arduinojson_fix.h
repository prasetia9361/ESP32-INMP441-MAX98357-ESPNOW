#pragma once

// Matikan sementara 'free' dari namespace eez kalau sudah ter-include
#ifdef free
#undef free
#endif

// Pastikan include stdlib.h global dulu
#include <stdlib.h>

// Lalu include ArduinoJson normal
#include <ArduinoJson.h>
