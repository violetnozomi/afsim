// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ARGO8MODELDEFS_HPP
#define ARGO8MODELDEFS_HPP

namespace Argo8ModelDefs
{
   //typedefs for lib function pointers
   typedef int    (*CreateModel_fn)          (void);
   typedef int    (*FreeModel_fn)            (int);
   typedef int    (*InitializeModel_fn)      (int);
   typedef int    (*SetInput_fn)             (int, int, double);
   typedef int    (*SetParameter_fn)         (int, int, double);
   typedef int    (*SetParameterString_fn)   (int, int, char*);
   typedef int    (*UpdateModel_fn)          (int);
   typedef int    (*GetOutput_fn)            (int, int, double*);
   typedef int    (*SetInputArray_fn)        (int, int, double*);
   typedef int    (*SetParameterArray_fn)    (int, int, double*);   
   typedef int    (*GetOutputArray_fn)       (int, int, double*);
   typedef int    (*EnableLogging_fn)        (int, char*);
   typedef int    (*DisableLogging_fn)       (int);
};

#endif

