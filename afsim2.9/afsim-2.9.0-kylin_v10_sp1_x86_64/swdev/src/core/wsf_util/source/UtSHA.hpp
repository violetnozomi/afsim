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


#ifndef UTSHA_HPP
#define UTSHA_HPP

#include "WsfUtExport.hpp"

// Typical DISCLAIMER:
// The code in this project is Copyright (C) 2003 by George Anescu. You have the right to
// use and distribute the code in any way you see fit as long as this paragraph is included
// with the distribution. No warranties or claims are made as to the validity of the
// information and code contained herein, so use it at your own risk.

#include <string>
class UtSHA_Digest;
// SHA Message Digest algorithm
class WSF_UT_EXPORT UtSHA
{
public:
   UtSHA();
   // Update context to reflect the concatenation of another buffer of bytes.
   void AddData(char const* aData, size_t aDataLength);

   // Convenience method to call AddData with a single value
   template<typename T>
   void Add(const T& value)
   {
      AddData((const char*)&value, (unsigned int)sizeof(value));
   }

   // Final wrapup - pad to 64-byte boundary with the bit pattern
   // 1 0*(64-bit count of bits processed, MSB-first)
   void FinalDigest(char* aDigest);
   void FinalDigest(UtSHA_Digest& aDigest);
   // Reset current operation in order to prepare a new one
   void Reset();

private:
   enum
   {
      BLOCKSIZE = 64
   };
   enum
   {
      SHA256LENGTH = 8
   };

   // Context Variables
   unsigned int  mBuf[SHA256LENGTH]; // Maximum for SHA256
   size_t        mBits[2];
   unsigned char mIn[BLOCKSIZE];

   // Internal auxiliary static functions
   unsigned int        CircularShift(unsigned int aBits, unsigned int aWord);
   static unsigned int CH(unsigned int aX, unsigned int aY, unsigned int aZ);
   static unsigned int MAJ(unsigned int aX, unsigned int aY, unsigned int aZ);

   static unsigned int SIG0(unsigned int aX);
   static unsigned int SIG1(unsigned int aX);
   static unsigned int sig0(unsigned int aX);
   static unsigned int sig1(unsigned int aX);

   static void Bytes2Word(unsigned char const* aBytes, unsigned int& aWord);
   static void Word2Bytes(size_t const& aWord, unsigned char* aBytes);

   // Transformation Function
   void Transform();

   // The Method
   static const unsigned int mK256[64];
   static const unsigned int mH256[SHA256LENGTH];
   // Control Flag
   // bool mAddData;
};

//! Container for a UtSHA Digest
class WSF_UT_EXPORT UtSHA_Digest
{
public:
   void Clear() { mWord64[0] = mWord64[1] = mWord64[2] = mWord64[3] = 0; }
   enum
   {
      cBUFFER_SIZE = 32
   };
   bool operator==(const UtSHA_Digest& aRhs) const
   {
      return mWord64[0] == aRhs.mWord64[0] && mWord64[1] == aRhs.mWord64[1] && mWord64[2] == aRhs.mWord64[2] &&
             mWord64[3] == aRhs.mWord64[3];
   }
   bool operator!=(const UtSHA_Digest& aRhs) const { return !(*this == aRhs); }
   bool operator<(const UtSHA_Digest& aRhs) const
   {
      if (mWord64[0] < aRhs.mWord64[0])
         return true;
      if (mWord64[0] > aRhs.mWord64[0])
         return false;
      if (mWord64[1] < aRhs.mWord64[1])
         return true;
      if (mWord64[1] > aRhs.mWord64[1])
         return false;
      if (mWord64[2] < aRhs.mWord64[2])
         return true;
      if (mWord64[2] > aRhs.mWord64[2])
         return false;
      return mWord64[3] < aRhs.mWord64[3];
   }
   std::string ToHex() const;
   union
   {
      char               mBytes[cBUFFER_SIZE];
      unsigned int       mWords[8];
      unsigned long long mWord64[4];
   };
};

inline unsigned int UtSHA::CircularShift(unsigned int aBits, unsigned int aWord)
{
   return (aWord << aBits) | (aWord >> (32 - aBits));
}

inline unsigned int UtSHA::CH(unsigned int aX, unsigned int aY, unsigned int aZ)
{
   return ((aX & (aY ^ aZ)) ^ aZ);
}

inline unsigned int UtSHA::MAJ(unsigned int aX, unsigned int aY, unsigned int aZ)
{
   return (((aX | aY) & aZ) | (aX & aY));
}

inline unsigned int UtSHA::SIG0(unsigned int aX)
{
   return ((aX >> 2) | (aX << 30)) ^ ((aX >> 13) | (aX << 19)) ^ ((aX >> 22) | (aX << 10));
}

inline unsigned int UtSHA::SIG1(unsigned int aX)
{
   return ((aX >> 6) | (aX << 26)) ^ ((aX >> 11) | (aX << 21)) ^ ((aX >> 25) | (aX << 7));
}

inline unsigned int UtSHA::sig0(unsigned int aX)
{
   return ((aX >> 7) | (aX << 25)) ^ ((aX >> 18) | (aX << 14)) ^ (aX >> 3);
}

inline unsigned int UtSHA::sig1(unsigned int aX)
{
   return ((aX >> 17) | (aX << 15)) ^ ((aX >> 19) | (aX << 13)) ^ (aX >> 10);
}

inline void UtSHA::Bytes2Word(unsigned char const* aBytes, unsigned int& aWord)
{
   aWord = (unsigned int)*(aBytes + 3) | (unsigned int)(*(aBytes + 2) << 8) | (unsigned int)(*(aBytes + 1) << 16) |
           (unsigned int)(*aBytes << 24);
}

inline void UtSHA::Word2Bytes(size_t const& aWord, unsigned char* aBytes)
{
   aBytes += 3;
   *aBytes   = aWord & 0xff;
   *--aBytes = (aWord >> 8) & 0xff;
   *--aBytes = (aWord >> 16) & 0xff;
   *--aBytes = (aWord >> 24) & 0xff;
}

#endif
