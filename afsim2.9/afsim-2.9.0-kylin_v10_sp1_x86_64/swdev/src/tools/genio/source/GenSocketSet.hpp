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
#ifndef GENSOCKETSET_HPP
#define GENSOCKETSET_HPP

#include "genio_export.h"

#include <vector>

namespace GenSockets
{
class GenSocket;

//! A container for GenSocket pointers
class GENIO_EXPORT GenSocketSet
{
public:
   //! Adds a socket to the set
   void AddSocket(GenSocket* aSocket);

   //! Remove a socket from the set
   void RemoveSocket(GenSocket* aSocket);

   //! Returns the number of sockets in this set
   unsigned int GetSocketCount() const { return static_cast<unsigned int>(mSockets.size()); }

   //! Returns the socket at the specified index
   GenSocket* GetSocketEntry(unsigned int aIndex) const { return mSockets[aIndex]; }

   //! Removes any sockets in the socket set.
   void Clear() { mSockets.clear(); }

   //! Returns true if there are no sockets in the set
   bool IsEmpty() const { return mSockets.empty(); }

protected:
   std::vector<GenSocket*> mSockets;
};

} // namespace GenSockets
#endif
