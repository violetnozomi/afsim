// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEVENTPIPEEVENTIDS_HPP
#define WSFEVENTPIPEEVENTIDS_HPP

namespace wsf
{
namespace eventpipe
{
// store event ids for built-in events
struct EventIds
{
   int cBASE_DATA;
   int cENTITY_STATE;
   int cDETECTION_CHANGE;
   int cTRACK;
   int cTRACK_UPDATE;
   int cDETECTION_ATTEMPT;
   int cDRAW;
   int cMESSAGE_RECEIVED;
   int cMESSAGE_TRANSMITTED;
   int cMESSAGE_HOP;
   int cCOMMENT;
   int cBEHAVIOR_TOOL;
   int cAUX_DATA;
   int cROUTE_CHANGED;
   int cZONES;
   int cCOMM_INFO;
};
} // namespace eventpipe
} // namespace wsf


#endif
