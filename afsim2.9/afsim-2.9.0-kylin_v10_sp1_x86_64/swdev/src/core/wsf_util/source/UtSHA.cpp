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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************


#include "UtSHA.hpp"

#include <cstring>

const unsigned int UtSHA::mK256[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
                                       0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
                                       0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
                                       0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                                       0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
                                       0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
                                       0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
                                       0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                                       0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
                                       0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
                                       0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

const unsigned int UtSHA::mH256[8] =
   {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

UtSHA::UtSHA()
{
   for (int i = 0; i < SHA256LENGTH; i++)
   {
      mBuf[i] = mH256[i];
   }
   for (int i = 0; i < BLOCKSIZE; i++)
   {
      mIn[i] = 0; // char 0
   }
   mBits[0] = 0;
   mBits[1] = 0;
}

// Update context to reflect the concatenation of another buffer of bytes.
void UtSHA::AddData(char const* aData, size_t aDataLength)
{
   if (aDataLength < 1)
   {
      return; // no data to add
   }
   size_t t = mBits[0]; // Update bitcount
   mBits[0] = t + (aDataLength << 3);
   if (mBits[0] < t)
   {
      mBits[1]++; // Carry from low to high
   }
   mBits[1] += aDataLength >> 29;
   t = (t >> 3) & 0x3f; // Bytes already
   // Handle any leading odd-sized chunks
   if (t != 0)
   {
      unsigned char* puc = (unsigned char*)mIn + t;
      t                  = BLOCKSIZE - t;
      if (aDataLength < t)
      {
         memcpy(puc, aData, aDataLength);
         return;
      }
      memcpy(puc, aData, t);
      Transform();
      aData += t;
      aDataLength -= t;
   }
   // Process data in 64-byte chunks
   while (aDataLength >= BLOCKSIZE)
   {
      memcpy(mIn, aData, BLOCKSIZE);
      Transform();
      aData += BLOCKSIZE;
      aDataLength -= BLOCKSIZE;
   }
   // Handle any remaining bytes of data
   memcpy(mIn, aData, aDataLength);
}

// Final wrapup - pad to 64-byte boundary with the bit pattern
// 1 0*(64-bit count of bits processed, MSB-first)
void UtSHA::FinalDigest(char* aDigest)
{
   // Is the User's responsibility to ensure that aDigest has at least 32 bytes allocated

   unsigned int   count;
   unsigned char* puc;
   // Compute number of bytes mod 64
   count = (mBits[0] >> 3) & 0x3F;
   // Set the first char of padding to 0x80. This is safe since there is
   // always at least one byte free
   puc    = mIn + count;
   *puc++ = 0x80;
   // Bytes of padding needed to make 64 bytes
   count = BLOCKSIZE - count - 1;
   // Pad out to 56 mod 64
   if (count < 8)
   {
      // Two lots of padding: Pad the first block to 64 bytes
      memset(puc, 0, count);
      Transform();
      // Now fill the next block with 56 bytes
      memset(mIn, 0, BLOCKSIZE - 8);
   }
   else
   {
      // Pad block to 56 bytes
      memset(puc, 0, count - 8);
   }
   // Append length in bits and transform
   Word2Bytes(mBits[1], &mIn[BLOCKSIZE - 8]);
   Word2Bytes(mBits[0], &mIn[BLOCKSIZE - 4]);
   Transform();
   for (int i = 0; i < SHA256LENGTH; i++, aDigest += 4)
   {
      Word2Bytes(mBuf[i], reinterpret_cast<unsigned char*>(aDigest));
   }
   // Reinitialize
   Reset();
}

void UtSHA::FinalDigest(UtSHA_Digest& aDigest)
{
   FinalDigest(&aDigest.mBytes[0]);
}

// Reset current operation in order to prepare a new one
void UtSHA::Reset()
{
   for (int i = 0; i < SHA256LENGTH; i++)
   {
      mBuf[i] = mH256[i];
   }
   mBits[0] = 0;
   mBits[1] = 0;
}

// The core of the SHA algorithm, this alters an existing SHA hash to
// reflect the addition of 16 longwords of new data.
void UtSHA::Transform()
{
   // Expansion of mIn
   unsigned char* in = mIn;
   unsigned int   w[64];
   int            i;
   for (i = 0; i < 16; i++, in += 4)
   {
      Bytes2Word(in, w[i]);
   }
   for (i = 16; i < 64; i++)
   {
      w[i] = sig1(w[i - 2]) + w[i - 7] + sig0(w[i - 15]) + w[i - 16];
   }
   // OR
   // for(i=0; i<48; i++)
   //    w[i+16] = sig1(w[i+14]) + w[i+9] + sig0(w[i+1]) + w[i];
   unsigned int a, b, c, d, e, f, g, h, t;
   a = mBuf[0];
   b = mBuf[1];
   c = mBuf[2];
   d = mBuf[3];
   e = mBuf[4];
   f = mBuf[5];
   g = mBuf[6];
   h = mBuf[7];
   t = h + SIG1(e) + CH(e, f, g) + mK256[0] + w[0];
   h = t + SIG0(a) + MAJ(a, b, c);
   d += t;
   t = g + SIG1(d) + CH(d, e, f) + mK256[1] + w[1];
   g = t + SIG0(h) + MAJ(h, a, b);
   c += t;
   t = f + SIG1(c) + CH(c, d, e) + mK256[2] + w[2];
   f = t + SIG0(g) + MAJ(g, h, a);
   b += t;
   t = e + SIG1(b) + CH(b, c, d) + mK256[3] + w[3];
   e = t + SIG0(f) + MAJ(f, g, h);
   a += t;
   t = d + SIG1(a) + CH(a, b, c) + mK256[4] + w[4];
   d = t + SIG0(e) + MAJ(e, f, g);
   h += t;
   t = c + SIG1(h) + CH(h, a, b) + mK256[5] + w[5];
   c = t + SIG0(d) + MAJ(d, e, f);
   g += t;
   t = b + SIG1(g) + CH(g, h, a) + mK256[6] + w[6];
   b = t + SIG0(c) + MAJ(c, d, e);
   f += t;
   t = a + SIG1(f) + CH(f, g, h) + mK256[7] + w[7];
   a = t + SIG0(b) + MAJ(b, c, d);
   e += t;
   //
   t = h + SIG1(e) + CH(e, f, g) + mK256[8] + w[8];
   h = t + SIG0(a) + MAJ(a, b, c);
   d += t;
   t = g + SIG1(d) + CH(d, e, f) + mK256[9] + w[9];
   g = t + SIG0(h) + MAJ(h, a, b);
   c += t;
   t = f + SIG1(c) + CH(c, d, e) + mK256[10] + w[10];
   f = t + SIG0(g) + MAJ(g, h, a);
   b += t;
   t = e + SIG1(b) + CH(b, c, d) + mK256[11] + w[11];
   e = t + SIG0(f) + MAJ(f, g, h);
   a += t;
   t = d + SIG1(a) + CH(a, b, c) + mK256[12] + w[12];
   d = t + SIG0(e) + MAJ(e, f, g);
   h += t;
   t = c + SIG1(h) + CH(h, a, b) + mK256[13] + w[13];
   c = t + SIG0(d) + MAJ(d, e, f);
   g += t;
   t = b + SIG1(g) + CH(g, h, a) + mK256[14] + w[14];
   b = t + SIG0(c) + MAJ(c, d, e);
   f += t;
   t = a + SIG1(f) + CH(f, g, h) + mK256[15] + w[15];
   a = t + SIG0(b) + MAJ(b, c, d);
   e += t;
   //
   t = h + SIG1(e) + CH(e, f, g) + mK256[16] + w[16];
   h = t + SIG0(a) + MAJ(a, b, c);
   d += t;
   t = g + SIG1(d) + CH(d, e, f) + mK256[17] + w[17];
   g = t + SIG0(h) + MAJ(h, a, b);
   c += t;
   t = f + SIG1(c) + CH(c, d, e) + mK256[18] + w[18];
   f = t + SIG0(g) + MAJ(g, h, a);
   b += t;
   t = e + SIG1(b) + CH(b, c, d) + mK256[19] + w[19];
   e = t + SIG0(f) + MAJ(f, g, h);
   a += t;
   t = d + SIG1(a) + CH(a, b, c) + mK256[20] + w[20];
   d = t + SIG0(e) + MAJ(e, f, g);
   h += t;
   t = c + SIG1(h) + CH(h, a, b) + mK256[21] + w[21];
   c = t + SIG0(d) + MAJ(d, e, f);
   g += t;
   t = b + SIG1(g) + CH(g, h, a) + mK256[22] + w[22];
   b = t + SIG0(c) + MAJ(c, d, e);
   f += t;
   t = a + SIG1(f) + CH(f, g, h) + mK256[23] + w[23];
   a = t + SIG0(b) + MAJ(b, c, d);
   e += t;
   //
   t = h + SIG1(e) + CH(e, f, g) + mK256[24] + w[24];
   h = t + SIG0(a) + MAJ(a, b, c);
   d += t;
   t = g + SIG1(d) + CH(d, e, f) + mK256[25] + w[25];
   g = t + SIG0(h) + MAJ(h, a, b);
   c += t;
   t = f + SIG1(c) + CH(c, d, e) + mK256[26] + w[26];
   f = t + SIG0(g) + MAJ(g, h, a);
   b += t;
   t = e + SIG1(b) + CH(b, c, d) + mK256[27] + w[27];
   e = t + SIG0(f) + MAJ(f, g, h);
   a += t;
   t = d + SIG1(a) + CH(a, b, c) + mK256[28] + w[28];
   d = t + SIG0(e) + MAJ(e, f, g);
   h += t;
   t = c + SIG1(h) + CH(h, a, b) + mK256[29] + w[29];
   c = t + SIG0(d) + MAJ(d, e, f);
   g += t;
   t = b + SIG1(g) + CH(g, h, a) + mK256[30] + w[30];
   b = t + SIG0(c) + MAJ(c, d, e);
   f += t;
   t = a + SIG1(f) + CH(f, g, h) + mK256[31] + w[31];
   a = t + SIG0(b) + MAJ(b, c, d);
   e += t;
   //
   t = h + SIG1(e) + CH(e, f, g) + mK256[32] + w[32];
   h = t + SIG0(a) + MAJ(a, b, c);
   d += t;
   t = g + SIG1(d) + CH(d, e, f) + mK256[33] + w[33];
   g = t + SIG0(h) + MAJ(h, a, b);
   c += t;
   t = f + SIG1(c) + CH(c, d, e) + mK256[34] + w[34];
   f = t + SIG0(g) + MAJ(g, h, a);
   b += t;
   t = e + SIG1(b) + CH(b, c, d) + mK256[35] + w[35];
   e = t + SIG0(f) + MAJ(f, g, h);
   a += t;
   t = d + SIG1(a) + CH(a, b, c) + mK256[36] + w[36];
   d = t + SIG0(e) + MAJ(e, f, g);
   h += t;
   t = c + SIG1(h) + CH(h, a, b) + mK256[37] + w[37];
   c = t + SIG0(d) + MAJ(d, e, f);
   g += t;
   t = b + SIG1(g) + CH(g, h, a) + mK256[38] + w[38];
   b = t + SIG0(c) + MAJ(c, d, e);
   f += t;
   t = a + SIG1(f) + CH(f, g, h) + mK256[39] + w[39];
   a = t + SIG0(b) + MAJ(b, c, d);
   e += t;
   //
   t = h + SIG1(e) + CH(e, f, g) + mK256[40] + w[40];
   h = t + SIG0(a) + MAJ(a, b, c);
   d += t;
   t = g + SIG1(d) + CH(d, e, f) + mK256[41] + w[41];
   g = t + SIG0(h) + MAJ(h, a, b);
   c += t;
   t = f + SIG1(c) + CH(c, d, e) + mK256[42] + w[42];
   f = t + SIG0(g) + MAJ(g, h, a);
   b += t;
   t = e + SIG1(b) + CH(b, c, d) + mK256[43] + w[43];
   e = t + SIG0(f) + MAJ(f, g, h);
   a += t;
   t = d + SIG1(a) + CH(a, b, c) + mK256[44] + w[44];
   d = t + SIG0(e) + MAJ(e, f, g);
   h += t;
   t = c + SIG1(h) + CH(h, a, b) + mK256[45] + w[45];
   c = t + SIG0(d) + MAJ(d, e, f);
   g += t;
   t = b + SIG1(g) + CH(g, h, a) + mK256[46] + w[46];
   b = t + SIG0(c) + MAJ(c, d, e);
   f += t;
   t = a + SIG1(f) + CH(f, g, h) + mK256[47] + w[47];
   a = t + SIG0(b) + MAJ(b, c, d);
   e += t;
   //
   t = h + SIG1(e) + CH(e, f, g) + mK256[48] + w[48];
   h = t + SIG0(a) + MAJ(a, b, c);
   d += t;
   t = g + SIG1(d) + CH(d, e, f) + mK256[49] + w[49];
   g = t + SIG0(h) + MAJ(h, a, b);
   c += t;
   t = f + SIG1(c) + CH(c, d, e) + mK256[50] + w[50];
   f = t + SIG0(g) + MAJ(g, h, a);
   b += t;
   t = e + SIG1(b) + CH(b, c, d) + mK256[51] + w[51];
   e = t + SIG0(f) + MAJ(f, g, h);
   a += t;
   t = d + SIG1(a) + CH(a, b, c) + mK256[52] + w[52];
   d = t + SIG0(e) + MAJ(e, f, g);
   h += t;
   t = c + SIG1(h) + CH(h, a, b) + mK256[53] + w[53];
   c = t + SIG0(d) + MAJ(d, e, f);
   g += t;
   t = b + SIG1(g) + CH(g, h, a) + mK256[54] + w[54];
   b = t + SIG0(c) + MAJ(c, d, e);
   f += t;
   t = a + SIG1(f) + CH(f, g, h) + mK256[55] + w[55];
   a = t + SIG0(b) + MAJ(b, c, d);
   e += t;
   //
   t = h + SIG1(e) + CH(e, f, g) + mK256[56] + w[56];
   h = t + SIG0(a) + MAJ(a, b, c);
   d += t;
   t = g + SIG1(d) + CH(d, e, f) + mK256[57] + w[57];
   g = t + SIG0(h) + MAJ(h, a, b);
   c += t;
   t = f + SIG1(c) + CH(c, d, e) + mK256[58] + w[58];
   f = t + SIG0(g) + MAJ(g, h, a);
   b += t;
   t = e + SIG1(b) + CH(b, c, d) + mK256[59] + w[59];
   e = t + SIG0(f) + MAJ(f, g, h);
   a += t;
   t = d + SIG1(a) + CH(a, b, c) + mK256[60] + w[60];
   d = t + SIG0(e) + MAJ(e, f, g);
   h += t;
   t = c + SIG1(h) + CH(h, a, b) + mK256[61] + w[61];
   c = t + SIG0(d) + MAJ(d, e, f);
   g += t;
   t = b + SIG1(g) + CH(g, h, a) + mK256[62] + w[62];
   b = t + SIG0(c) + MAJ(c, d, e);
   f += t;
   t = a + SIG1(f) + CH(f, g, h) + mK256[63] + w[63];
   a = t + SIG0(b) + MAJ(b, c, d);
   e += t;
   //
   // OR
   /*
     unsigned int a, b, c, d, e, f, g, h, t1, t2;
     a = mBuf[0];
     b = mBuf[1];
     c = mBuf[2];
     d = mBuf[3];
     e = mBuf[4];
     f = mBuf[5];
     g = mBuf[6];
     h = mBuf[7];
     //
     for(i=0; i<64; i++)
     {
     t1 = h + SIG1(e) + CH(e, f, g) + mK256[i] + w[i];
     t2 = SIG0(a) + MAJ(a, b, c);
     h = g;
     g = f;
     f = e;
     e = d+t1;
     d = c;
     c = b;
     b = a;
     a = t1 + t2;
     }
   */
   mBuf[0] += a;
   mBuf[1] += b;
   mBuf[2] += c;
   mBuf[3] += d;
   mBuf[4] += e;
   mBuf[5] += f;
   mBuf[6] += g;
   mBuf[7] += h;
}


std::string UtSHA_Digest::ToHex() const
{
   std::string hex;
   char        hexChars[] = "0123456789ABCDEF";
   for (int i = 0; i < cBUFFER_SIZE; ++i)
   {
      int low  = mBytes[i] & 0xf;
      int high = (((int)mBytes[i]) >> 4) & 0xf;
      hex += hexChars[low];
      hex += hexChars[high];
   }
   // UtSHA computes a 512 bit hash
   // which is 64 hex digits
   return hex;
}
