// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SOSM_BINARYIO_HPP
#define SOSM_BINARYIO_HPP

#include <string>
#include <vector>

class SOSM_SpectralObject;
#include "TblLookup.hpp"

//! A collection of functions to assist in performing binary I/O for tables.
namespace SOSM_BinaryIO
{
enum
{
   cSO_IDENT  = 0x51,
   cIV_IDENT  = 0x53,
   cDV_IDENT  = 0x57,
   cEOF_IDENT = 0x77
};

void ReadHeader(std::istream& aInput, std::string aUserIdent[3]);

void WriteHeader(std::ostream& aOutput, const std::string aUserIdent[3]);

void ReadSpectralObject(std::istream& aInput, SOSM_SpectralObject& aObject);

void WriteSpectralObject(std::ostream& aOutput, const SOSM_SpectralObject& aObject);

void ReadDV(std::istream& aInput, std::vector<float>& aDV);

void WriteDV(std::ostream& aOutput, const std::vector<float>& aDV);

void ReadIV(std::istream& aInput, TblIndVarU<float>& aIV);

void WriteIV(std::ostream& aOutput, const TblIndVarU<float>& aIV);

void ReadTrailer(std::istream& aInput);

void WriteTrailer(std::ostream& aOutput);

void ReadString(std::istream& aInput, std::string& aString);

void WriteString(std::ostream& aOutput, const std::string& aString);
} // namespace SOSM_BinaryIO

#endif
