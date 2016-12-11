// SWIG interface file to define rpi_ddbooster library python wrapper.
// Author: Richard Guenther (richard.guenther@web.de)

// Define module name ddbooster
%module ddbooster

// Include standard SWIG types & array support for support of uint8_t and uint16_t
%include "stdint.i"
%include "carrays.i"

%{
#include "../DDBooster.h"
%}

// Process DDBooster.h header
%include "../DDBooster.h"