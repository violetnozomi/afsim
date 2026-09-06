// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// this export come from NVidia.  I believe this will force laptops that have multiple GPUs to use the correct one.
#if defined(WIN32)
extern "C"
{
   _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#undef GetCurrentTime
#undef GetMessage
#else
extern "C"
{
   unsigned long NvOptimusEnablement = 0x00000001;
}
#endif
