/******************************************************************************
*
*   Copyright 2013-2018 NXP
*
*   This software is owned or controlled by NXP and may only be used strictly in accordance with the
*   applicable license terms.  By expressly accepting such terms or by downloading, installing,
*   activating and/or otherwise using the software, you are agreeing that you have read, and that
*   you agree to comply with and are bound by, such license terms.  If you do not agree to be bound
*   by the applicable license terms, then you may not retain, install, activate or otherwise use the
*   software.
*
******************************************************************************/
/**
*
* @file     SWLIBS_Typedefs.h
* 
* @version  1.0.17.0
* 
* @date     Jan-31-2017
* 
* @brief    Basic types definition file.
*
*******************************************************************************
*
******************************************************************************/

/**
* @page misra_violations MISRA-C:2004 violations
* 
* @section SWLIBS_Typedefs_h_REF_1
* Violates MISRA 2004 Required Rule 20.2, Re-use of C90 identifier. 
* Symbol is defined only if not defined by the compiler.
* 
* @section SWLIBS_Typedefs_h_REF_2
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro. 
* To allow the user utilize the benefits of using all three supported implementation of each 
* function in user application, the macro dispatcher inevitably requires use of disallowed macro 
*
* @section SWLIBS_Typedefs_h_REF_3
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* The macro substitutes an intrinsic function provided by other compilers.
* definition. 
*
* @section SWLIBS_Typedefs_h_REF_4
* Violates MISRA 2004 Required Rule 20.2, Re-use of C90 identifier.
* The identifier is defined here only if the compiler does not provide the definition. 
*/

#ifndef SWLIBS_TYPEDEFS_H
#define SWLIBS_TYPEDEFS_H

#include "SWLIBS_Config.h"
#include "SWLIBS_MacroDisp.h"

/******************************************************************************
* Implementation configuration checking            (scope: module-global)
******************************************************************************/
#ifndef _MATLAB_BAM_CREATE
  #if((SWLIBS_DEFAULT_IMPLEMENTATION != SWLIBS_DEFAULT_IMPLEMENTATION_F32)&& \
      (SWLIBS_DEFAULT_IMPLEMENTATION != SWLIBS_DEFAULT_IMPLEMENTATION_F16)&& \
      (SWLIBS_DEFAULT_IMPLEMENTATION != SWLIBS_DEFAULT_IMPLEMENTATION_FLT))
    #error "Define default implementation in SWLIBS_Config.h file."
  #endif

  #if((SWLIBS_SUPPORT_F32 == SWLIBS_STD_OFF)&&(SWLIBS_SUPPORT_F16 == SWLIBS_STD_OFF)&&(SWLIBS_SUPPORT_FLT == SWLIBS_STD_OFF))
    #error "Define at least one supported implementation in SWLIBS_Config.h file."
  #endif

  #if((SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)&&(SWLIBS_SUPPORT_F32 == SWLIBS_STD_OFF))
    #error "Set the SWLIBS_SUPPORT_F32 to SWLIBS_STD_ON in SWLIBS_Config.h file to support FRAC32 default implementation."
  #endif

  #if((SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)&&(SWLIBS_SUPPORT_F16 == SWLIBS_STD_OFF))
    #error "Set the SWLIBS_SUPPORT_F16 to SWLIBS_STD_ON in SWLIBS_Config.h file to support FRAC16 default implementation."
  #endif

  #if((SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_FLT)&&(SWLIBS_SUPPORT_FLT == SWLIBS_STD_OFF))
    #error "Set the SWLIBS_SUPPORT_FLT to SWLIBS_STD_ON in SWLIBS_Config.h file to support FLOAT default implementation."
  #endif
#endif

/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
#ifndef NULL
  #ifdef __cplusplus
    #define NULL ((void*)0)
  #else
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90 
    * identifier. 
    */
    #define NULL 0
  #endif
#endif /* NULL */


/******************************************************************************
* Defines and macros            (scope: module-global)
******************************************************************************/


/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/
typedef unsigned char       tBool;          /**< basic boolean type */

#ifndef FALSE
#define FALSE ((tBool)0)                    /**< Boolean type FALSE constant */
#endif

#ifndef TRUE
#define TRUE ((tBool)1)                     /**< Boolean type TRUE constant */
#endif

#ifndef __SIZEOF_LONG__
  /*
  * @violates @ref SWLIBS_Typedefs_h_REF_4 MISRA 2004 Required Rule 20.2, Re-use of C90 identifier.
  */
  #define __SIZEOF_LONG__ 4
#endif
#if (__SIZEOF_LONG__ == 8)
  typedef unsigned char       tU8;            /**< unsigned 8-bit integer type */
  typedef signed char         tS8;            /**< signed 8-bit integer type */
  typedef unsigned short      tU16;           /**< unsigned 16-bit integer type */
  typedef signed short        tS16;           /**< signed 16-bit integer type */
  typedef unsigned int        tU32;           /**< unsigned 32-bit integer type */
  typedef signed int          tS32;           /**< signed 32-bit integer type */
  typedef signed long         tS64;           /**< signed 64-bit integer type */
  typedef unsigned long       tU64;           /**< unsigned 64-bit integer type */
  typedef tS16                tFrac16;        /**< 16-bit signed fractional Q1.15 type */
  typedef tS32                tFrac32;        /**< 32-bit Q1.31 type */
  typedef float               tFloat;         /**< single precision float type */
  typedef double              tDouble;        /**< double precision float type */
#else
  typedef unsigned char       tU8;            /**< unsigned 8-bit integer type */
  typedef signed char         tS8;            /**< signed 8-bit integer type */
  typedef unsigned short      tU16;           /**< unsigned 16-bit integer type */
  typedef signed short        tS16;           /**< signed 16-bit integer type */
  typedef unsigned long       tU32;           /**< unsigned 32-bit integer type */
  typedef signed long         tS32;           /**< signed 32-bit integer type */
  typedef signed long long    tS64;           /**< signed 64-bit integer type */
  typedef unsigned long long  tU64;           /**< unsigned 64-bit integer type */
  typedef tS16                tFrac16;        /**< 16-bit signed fractional Q1.15 type */
  typedef tS32                tFrac32;        /**< 32-bit Q1.31 type */
  typedef float               tFloat;         /**< single precision float type */
  typedef double              tDouble;        /**< double precision float type */
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef _MATLAB_BAM_CREATE
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition 
    * for macro. 
    */
    #define SWLIBS_2Syst    SWLIBS_2Syst_F32                        /**< Definition of SWLIBS_2Syst as alias for SWLIBS_2Syst_F32 array in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition 
    * for macro. 
    */
    #define SWLIBS_3Syst    SWLIBS_3Syst_F32                        /**< Definition of SWLIBS_3Syst as alias for SWLIBS_3Syst_F32 array in case the 32-bit fractional implementation is selected. */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition 
    * for macro. 
    */
    #define SWLIBS_2Syst    SWLIBS_2Syst_F16                        /**< Definition of SWLIBS_2Syst as alias for SWLIBS_2Syst_F16 array in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition 
    * for macro. 
    */
    #define SWLIBS_3Syst    SWLIBS_3Syst_F16                        /**< Definition of SWLIBS_3Syst as alias for SWLIBS_3Syst_F16 array in case the 16-bit fractional implementation is selected. */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_FLT)
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition 
    * for macro. 
    */
    #define SWLIBS_2Syst    SWLIBS_2Syst_FLT                        /**< Definition of SWLIBS_2Syst as alias for SWLIBS_2Syst_FLT array in case the single precision floating point implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition 
    * for macro. 
    */
    #define SWLIBS_3Syst    SWLIBS_3Syst_FLT                        /**< Definition of SWLIBS_3Syst as alias for SWLIBS_3Syst_FLT array in case the single precision floating point implementation is selected. */
  #endif
#endif /* _MATLAB_BAM_CREATE */


#if (SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON)
/************************************************************************//**
@struct SWLIBS_3Syst_F32 "\SWLIBS_Typedefs.h"

@brief  Array of three standard 32-bit fractional arguments.
*//*************************************************************************/
  typedef struct
  {
    tFrac32 f32Arg1;   /**< First argument */
    tFrac32 f32Arg2;   /**< Second argument */
    tFrac32 f32Arg3;   /**< Third argument */
  } SWLIBS_3Syst_F32;

/************************************************************************//**
@struct SWLIBS_2Syst_F32 "\SWLIBS_Typedefs.h"

@brief  Array of two standard 32-bit fractional arguments.
*//*************************************************************************/
  typedef struct
  {
    tFrac32 f32Arg1;   /**< First argument */
    tFrac32 f32Arg2;   /**< Second argument */
  } SWLIBS_2Syst_F32;
#endif


#if (SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON)
/************************************************************************//**
@struct SWLIBS_3Syst_F16 "\SWLIBS_Typedefs.h"

@brief  Array of three standard 16-bit fractional arguments.
*//*************************************************************************/
  typedef struct
  {
    tFrac16 f16Arg1;   /**< First argument */
    tFrac16 f16Arg2;   /**< Second argument */
    tFrac16 f16Arg3;   /**< Third argument */
  } SWLIBS_3Syst_F16;

/************************************************************************//**
@struct SWLIBS_2Syst_F16 "\SWLIBS_Typedefs.h"

@brief  Array of two standard 16-bit fractional arguments.
*//*************************************************************************/
  typedef struct
  {
    tFrac16 f16Arg1;   /**< First argument */
    tFrac16 f16Arg2;   /**< Second argument */
  } SWLIBS_2Syst_F16;
#endif


#if (SWLIBS_SUPPORT_FLT == SWLIBS_STD_ON)
/************************************************************************//**
@struct SWLIBS_3Syst_FLT "\SWLIBS_Typedefs.h"

@brief  Array of three standard single precision floating point arguments.
*//*************************************************************************/
  typedef struct
  {
    tFloat fltArg1;   /**< First argument */
    tFloat fltArg2;   /**< Second argument */
    tFloat fltArg3;   /**< Third argument */
  } SWLIBS_3Syst_FLT;

/************************************************************************//**
@struct SWLIBS_2Syst_FLT "\SWLIBS_Typedefs.h"

@brief  Array of two standard single precision floating point arguments.
*//*************************************************************************/
  typedef struct
  {
    tFloat fltArg1;   /**< First argument */
    tFloat fltArg2;   /**< Second argument */
  } SWLIBS_2Syst_FLT;
#endif

#if (defined(__DCC__) && defined(__PPC_APU_LSP))
  /*
  * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
  * identifier.
  */
  typedef unsigned short __lsp16_16__;
  /*
  * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
  * identifier.
  */
  typedef unsigned long __lsp32_16__;
  /*
  * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
  * identifier.
  */
  typedef unsigned long __lsp32_sf16__;
  /*
  * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
  * identifier.
  */
  typedef unsigned long __lsp32_32__;
  /*
  * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
  * identifier.
  */
  typedef unsigned long long __lsp64_33_31__;
  /*
  * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
  * identifier.
  */
  typedef const unsigned int __lsp4_uimm__;

  #ifndef __lsp_create_32_16
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
    * identifier.
    * @violates @ref SWLIBS_Typedefs_h_REF_3 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    */
    #define __lsp_create_32_16(a, b) (((__lsp16_16__)(a) << 16u) || (__lsp16_16__)(b))
  #endif
  #ifndef __lsp_extract_upper_hword
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
    * identifier.
    * @violates @ref SWLIBS_Typedefs_h_REF_3 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    */
    #define __lsp_extract_upper_hword(a) ((tFrac16)((__lsp32_sf16__)(a) >> 16u))
  #endif
  #ifndef __lsp_extract_lower_hword
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
    * identifier.
    * @violates @ref SWLIBS_Typedefs_h_REF_3 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    */
    #define __lsp_extract_lower_hword(a) ((tFrac16)(a))
  #endif
  #ifndef __lsp_extract_upper_word
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
    * identifier.
    * @violates @ref SWLIBS_Typedefs_h_REF_3 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    */
    #define __lsp_extract_upper_word(a) ((tFrac32)((__lsp64_33_31__)(a) >> 32u))
  #endif
  #ifndef __lsp_extract_lower_word
     /*
    * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
    * identifier.
    * @violates @ref SWLIBS_Typedefs_h_REF_3 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    */
    #define __lsp_extract_lower_word(a) ((tFrac32)(a))
  #endif
#endif

#endif /* SWLIBS_TYPEDEFS_H */
