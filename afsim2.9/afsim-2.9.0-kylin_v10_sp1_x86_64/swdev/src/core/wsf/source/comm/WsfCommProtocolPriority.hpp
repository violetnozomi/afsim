// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMPROTOCOLPRIORITY_HPP
#define WSFCOMMPROTOCOLPRIORITY_HPP

namespace wsf
{
namespace comm
{

//! An enum that is centrally located for assignment of
//! protocol priorities. These are held in a single
//! location for ease of inspection of relative
//! priorities of differing comm protocols.
enum class ProtocolPriority : int
{
   cAD_HOC    = 0,
   cRIPv2     = 100,
   cOSPF      = 200,
   cIGMP      = 300,
   cMULTICAST = 400,
   cLEGACY    = 500
};


} // namespace comm
} // namespace wsf

#endif
