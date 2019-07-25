//*****************************************************************************
//
// Prototypes for the display driver for the Powertip PH480272T-006-106Q.
//
// Copyright (c) 2010-2015 Ningbo TECHMATION Software Co.,Ltd.
// All rights reserved. Software License Agreement
//
//*****************************************************************************

#ifndef __DRIVERS_PH480272T_SSD1963_H__
#define __DRIVERS_PH480272T_SSD1963_H__

// For replacement: (Please DELETE THIS)
// Kentec320x240x16_SSD2119 --> PH480272T_ssd1963
#include "grlib/grlib.h"
#include "device.h"
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Prototypes.
//
//*****************************************************************************
extern const tDisplay g_sPH480272T_ssd1963;
extern DEV_ENGINE_T     g_PH480272T_ssd1963_Engine;
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __DRIVERS_PH480272T_SSD1963_H__
