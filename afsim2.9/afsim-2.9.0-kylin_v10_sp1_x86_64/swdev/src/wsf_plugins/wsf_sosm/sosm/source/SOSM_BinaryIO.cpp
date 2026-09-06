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

#include "SOSM_BinaryIO.hpp"

#include "SOSM_SpectralObject.hpp"
#include "SOSM_Utility.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"

// =================================================================================================
void SOSM_BinaryIO::ReadHeader(std::istream& aInput, std::string aUserIdent[3])
{
   char header[32];

   if (!aInput.read(header, sizeof(header)))
   {
      throw UtException("Read error-HDR");
   }
   ReadString(aInput, aUserIdent[0]);
   ReadString(aInput, aUserIdent[1]);
   ReadString(aInput, aUserIdent[2]);

   bool ok = true;
   if ((header[0] != 0x01) || (header[1] != 0x02) || (header[2] != 0x03) || (header[3] != 0x04) || (header[4] != 'S') ||
       (header[5] != 'O') || (header[6] != 'D') || (header[7] != 'M'))
   {
      ok = false;
   }

   if (header[16] != 1)
   {
      ok = false;
   }

   if (header[17] != 0)
   {
      ok = false;
   }

   if (header[18] != 0)
   {
      ok = false;
   }

   if (header[19] != 0)
   {
      ok = false;
   }

   if (header[20] != sizeof(int))
   {
      auto out = ut::log::warning() << "Mismatch in sizeof(int): ";
      out.AddNote() << "File: " << static_cast<int>(header[20]);
      out.AddNote() << "Host: " << sizeof(int);
      ok = false;
   }

   if (header[21] != sizeof(int))
   {
      auto out = ut::log::warning() << "Mismatch in sizeof(int): ";
      out.AddNote() << "File: " << static_cast<int>(header[20]);
      out.AddNote() << "Host: " << sizeof(int);
      ok = false;
   }
   if (!ok)
   {
      throw UtException("Header validation error");
   }
}

// =================================================================================================
void SOSM_BinaryIO::WriteHeader(std::ostream& aOutput, const std::string aUserIdent[3])
{
   char header[32] = {0x01, 0x02, 0x03, 0x04, 'S', 'O', 'D', 'M', 0, 0, 0, 0, 0, 0, 0, 0,
                      0,    0,    0,    0,    0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0};

   header[16] = 1; // Major version
   header[17] = 0; // Minor version
   header[18] = 0; // Numeric format (0=IEEE)
   header[19] = 0; // Byte ordering (0 = little endian, 1 = big endian)
   header[20] = sizeof(int);
   header[21] = sizeof(float);

   if (!aOutput.write(header, sizeof(header)))
   {
      throw UtException("Write error-HDR");
   }
   WriteString(aOutput, aUserIdent[0]);
   WriteString(aOutput, aUserIdent[1]);
   WriteString(aOutput, aUserIdent[2]);
}

// =================================================================================================
void SOSM_BinaryIO::ReadSpectralObject(std::istream& aInput, SOSM_SpectralObject& aObject)
{
   char  soIdent1;
   char  soIdent2;
   float origin;
   float increment;
   int   count;
   if (aInput.read(&soIdent1, sizeof(soIdent1)) && (soIdent1 == cSO_IDENT) &&
       // cppcheck-suppress invalidPointerCast
       aInput.read(reinterpret_cast<char*>(&origin), sizeof(float)) &&
       // cppcheck-suppress invalidPointerCast
       aInput.read(reinterpret_cast<char*>(&increment), sizeof(float)) &&
       // cppcheck-suppress invalidPointerCast
       aInput.read(reinterpret_cast<char*>(&count), sizeof(int)) && aInput.read(&soIdent2, sizeof(soIdent2)) &&
       (soIdent2 == cSO_IDENT))
   {
      if ((origin >= 0.0F) && (increment > 0.0F) && (count > 0))
      {
         aObject.DefineSpectralLimits(origin, increment, count);
         return;
      }
   }
   throw UtException("Read error-SL");
}

// =================================================================================================
void SOSM_BinaryIO::WriteSpectralObject(std::ostream& aOutput, const SOSM_SpectralObject& aObject)
{
   char  soIdent   = cSO_IDENT;
   float origin    = aObject.Origin();
   float increment = aObject.Increment();
   int   count     = aObject.Count();
   if (aOutput.write(&soIdent, sizeof(soIdent)) &&
       // cppcheck-suppress invalidPointerCast
       aOutput.write(reinterpret_cast<const char*>(&origin), sizeof(float)) &&
       // cppcheck-suppress invalidPointerCast
       aOutput.write(reinterpret_cast<const char*>(&increment), sizeof(float)) &&
       // cppcheck-suppress invalidPointerCast
       aOutput.write(reinterpret_cast<const char*>(&count), sizeof(int)) && aOutput.write(&soIdent, sizeof(soIdent)))
   {
      return;
   }
   throw UtException("Write Error");
}

// =================================================================================================
void SOSM_BinaryIO::ReadDV(std::istream& aInput, std::vector<float>& aDV)
{
   char dvIdent;
   int  dvSize;
   if (aInput.read(&dvIdent, sizeof(dvIdent)) && (dvIdent == cDV_IDENT) && aInput.read((char*)&dvSize, sizeof(int)))
   {
      if ((dvSize >= 2) && (dvSize < 200000000))
      {
         aDV.resize(dvSize);
         if (aInput.read((char*)&(aDV[0]), dvSize * sizeof(float)) && aInput.read(&dvIdent, sizeof(dvIdent)) &&
             (dvIdent == cDV_IDENT))
         {
            return;
         }
      }
   }
   throw UtException("Read error-DV");
}

// =================================================================================================
void SOSM_BinaryIO::WriteDV(std::ostream& aOutput, const std::vector<float>& aDV)
{
   char dvIdent = cDV_IDENT;
   int  dvSize  = static_cast<int>(aDV.size());
   if (aOutput.write(&dvIdent, sizeof(dvIdent)) && aOutput.write((const char*)&dvSize, sizeof(int)) &&
       aOutput.write((const char*)&(aDV[0]), dvSize * sizeof(float)) && aOutput.write(&dvIdent, sizeof(dvIdent)))
   {
      return;
   }
   throw UtException("Write error-DV");
}

// =================================================================================================
void SOSM_BinaryIO::ReadIV(std::istream& aInput, TblIndVarU<float>& aIV)
{
   char ivIdent;
   int  ivSize;
   if (aInput.read(&ivIdent, sizeof(ivIdent)) && (ivIdent == cIV_IDENT) && aInput.read((char*)&ivSize, sizeof(int)))
   {
      if ((ivSize >= 2) && (ivSize < 10000))
      {
         aIV.Resize(ivSize);
         std::vector<float>& iv = aIV.GetValues();
         if (aInput.read((char*)&(iv[0]), ivSize * sizeof(float)) && aInput.read(&ivIdent, sizeof(ivIdent)) &&
             (ivIdent == cIV_IDENT))
         {
            return;
         }
      }
   }
   throw UtException("Read error-IV");
}

// =================================================================================================
void SOSM_BinaryIO::WriteIV(std::ostream& aOutput, const TblIndVarU<float>& aIV)
{
   char                      ivIdent = cIV_IDENT;
   int                       ivSize  = static_cast<int>(aIV.GetSize());
   const std::vector<float>& iv      = aIV.GetValues();
   if (aOutput.write(&ivIdent, sizeof(ivIdent)) && aOutput.write((const char*)&ivSize, sizeof(int)) &&
       aOutput.write((const char*)&(iv[0]), ivSize * sizeof(float)) && aOutput.write(&ivIdent, sizeof(ivIdent)))
   {
      return;
   }
   throw UtException("Write error-IV");
}

// =================================================================================================
void SOSM_BinaryIO::ReadString(std::istream& aInput, std::string& aString)
{
   int strSize;
   if (aInput.read((char*)&strSize, sizeof(int)))
   {
      if (strSize == 0)
      {
         aString = "";
         return;
      }
      else if ((strSize > 0) && (strSize <= 1024))
      {
         char tempStr[1024 + 1];
         if (aInput.read(tempStr, strSize))
         {
            tempStr[strSize] = '\0';
            aString          = tempStr;
            return;
         }
      }
   }
   throw UtException("Read error-STR");
}

// =================================================================================================
void SOSM_BinaryIO::WriteString(std::ostream& aOutput, const std::string& aString)
{
   int strSize = static_cast<int>(aString.size());
   if (aOutput.write((const char*)&strSize, sizeof(int)))
   {
      if (strSize == 0)
      {
         return;
      }
      if (strSize > 1024)
      {
         strSize = 1024;
      }
      if (aOutput.write((const char*)aString.c_str(), strSize))
      {
         return;
      }
   }
   throw UtException("Write error-STR");
}

// =================================================================================================
void SOSM_BinaryIO::ReadTrailer(std::istream& aInput)
{
   char eofIdent;
   if ((aInput.read(&eofIdent, sizeof(eofIdent))) && (eofIdent == cEOF_IDENT))
   {
      return;
   }
   throw UtException("Read error-TRL");
}

// =================================================================================================
void SOSM_BinaryIO::WriteTrailer(std::ostream& aOutput)
{
   char eofIdent = cEOF_IDENT;
   if (aOutput.write(&eofIdent, sizeof(eofIdent)))
   {
      return;
   }
   throw UtException("Write error-TRL");
}
