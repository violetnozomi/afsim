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


/*! \file   DisUtilities.h
 *  \brief  Dis Utility class
 */

#ifndef DIS_UTILITIES_H
#define DIS_UTILITIES_H

#include "DisEntityId.hpp"

/*!
  \brief Converts from DISEntityId+RadioId to a ReceivedPowerMapType key

  This function packs an input DisEntityId and a RadioId (part of a DisTransmitter PDU) into
  a 64-bit unsigned integer.  It is used for key generation in modules that use DisTransmitter
  PDUs.

  \param[in] aEntityId the DisEntityId to encode into the key
  \param[in] aRadioId the RadioId to encode into the key

  \return A 64-bit unsigned integer key value
 */
inline DisUint64 DisIdToTransmitterId(const DisEntityId& aEntityId, unsigned short aRadioId)
{
   DisUint64 rid = (static_cast<DisUint64>(aEntityId.GetSite()) << 48) +
                   (static_cast<DisUint64>(aEntityId.GetApplication()) << 32) +
                   (static_cast<DisUint64>(aEntityId.GetEntity()) << 16) + aRadioId;
   return rid;
}

/*!
  \brief Converts from a packed unsigned 64-bit integer to a DisEntityId and a RadioId

  This function converts a value previously packed by the DisToUint64 function back into
  a DisEntityId and a RadioId.  It is used to decode key values in modules that use
  DisTransmitter PDUs.

  \param[in] aRID the key value to decode
  \param[out] aEntityId the decoded DisEntityId value corresponding to the key
  \param[out] aRadioId the decoded RadioId value corresponding to the key
 */
inline void TransmitterIdToDisId(DisUint64 aRID, DisEntityId& aEntityId, unsigned short& aRadioId)
{
   DisUint32 highbits = static_cast<DisUint32>(aRID >> 32);
   aEntityId.SetSite(static_cast<DisUint16>((highbits & 0xffff0000) >> 16));
   aEntityId.SetApplication(static_cast<DisUint16>(highbits & 0x0000ffff));
   aEntityId.SetEntity(static_cast<DisUint16>((aRID & 0xffff0000) >> 16));
   aRadioId = (static_cast<DisUint16>(aRID & 0x0000ffff));
   return;
}

#endif
