// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISGRIDDEDDATA_HPP
#define DISGRIDDEDDATA_HPP

#include "dis_export.h"

#include "DisClockTime.hpp"
#include "DisCoordinateSystemEnum.hpp"
#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisEnvironmentalRecord.hpp"
#include "DisGridAxisDescriptorBaseRecord.hpp"
#include "DisGridDataRecord.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSimulationId.hpp"

/**!
 * The Gridded Data PDU shall transmit information about large-scale or high-fidelity spatially and temporally
 * varying ambient fields and about environmental features and processes.Multiple PDUs may be required to
 * transfer all data necessary to describe an environmental state variable.The environmental state information
 * transmitted shall be sufficient for receiving applications to represent the issued environment within the
 * application’s own simulation.
 */
class DIS_EXPORT DisGriddedData : public DisPdu
{
public:
   DisGriddedData();
   DisGriddedData(const DisGriddedData& aSrc);
   DisGriddedData(const DisPdu& aPdu, GenI& aGenI);
   ~DisGriddedData() = default;

   DisGriddedData* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Getters
   const DisSimulationId         GetEnvironmentalSimulationId() const;
   DisUint16                     GetFieldNumber() const;
   DisUint16                     GetPduNumber() const;
   DisUint16                     GetPduTotal() const;
   const DisCoordinateSystemEnum GetCoordinateSystem() const;
   DisUint8                      GetNumberOfGridAxis() const;
   DisEnum8                      GetConstantGrid() const;
   const DisEntityType           GetEnvironmentType() const;
   void GetOrientation(DisFloat32& aOrientationX, DisFloat32& aOrientationY, DisFloat32& aOrientationZ) const;
   const DisClockTime                     GetSampleTime() const;
   DisUint32                              GetTotalValue() const;
   DisUint8                               GetVectorDimension() const;
   const DisGridAxisDescriptorBaseRecord& GetGridAccessDescriptorAtIndex(int aIndex) const;
   DisPtrContainer<DisGridDataRecord>&    GetGridData();

   // Setters
   void SetEnvironmentalSimulationId(const DisSimulationId& aEnvironmentalSimulationId);
   void SetFieldNumber(DisUint16 aFieldNumber);
   void SetPduNumber(DisUint16 aPduNumber);
   void SetPduTotal(DisUint16 aPduTotal);
   void SetCoordinateSystem(const DisCoordinateSystemEnum aCoordinateSystem);
   void SetNumberOfGridAxis(DisUint8 aNumberOfGridAxis);
   void SetConstantGrid(DisEnum8 aConstantGrid);
   void SetEnvironmentType(const DisEntityType& aEnvironmentType);
   void SetOrientation(DisFloat32 aOrientationX, DisFloat32 aOrientationY, DisFloat32 aOrientationZ);
   void SetSampleTime(DisClockTime aSampleTime);
   void SetTotalValue(DisUint32 aTotalValue);
   void SetVectorDimension(DisUint8 aVectorDimension);
   void SetGridAccessDescriptor(const DisPtrContainer<DisGridAxisDescriptorBaseRecord>& aGridAccessDescriptors);
   void SetGridData(const DisPtrContainer<DisGridDataRecord>& aGridData);

   // Data Validation
   bool IsValid() const override;

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const override;

private:
   void GetMemberData(GenI& aGenI) override;

   //! The environmental simulation application issuing the PDU
   DisSimulationId mEnvironmentalSimulationId;

   //! A unique identifier for each environmental variable transmitted
   DisUint16 mFieldNumber = 0;

   /**!
    * This field shall specify an index number of the current PDU within the
    * total number of PDUs used to transmit the environmental data.
    */
   DisUint16 mPduNumber = 0;

   //! This field shall specify the total number of PDUs used to transmit the environmental data.
   DisUint16 mPduTotal = 0;

   /**!
    * This field shall specify the coordinate system of the grid for
    * the environmental data contained in the PDU
    */
   DisEnum16 mCoordinateSystem = 0;

   /**!
    * This field shall specify the number of grid axes for the environmental data contained in
    * the PDU(e.g., three grid axes for an x, y, z coordinate system).
    */
   DisUint8 mNumberOfGridAxis = 0;

   /**!
    * This field shall specify whether the domain grid axes are identical to those of the previous
    * domain update grid for the environmental data sample contained in the PDU.
    * NOTE: Values can be either 0 (Constant) or 1 (Updated)
    */
   DisEnum8 mConstantGrid = 0;

   //! This field shall identify the type of environmental entity being described
   DisEntityType mEnvironmentType;

   //! The orientation of the data grid
   DisFloat32 mOrientation[3];

   /**!
    * This field shall specify the valid simulation time of the environmental
    * data sample contained in the PDU
    */
   DisClockTime mSampleTime;

   /**!
    * This field shall specify the total number of data values for all PDUs for an environmental
    * sample.This total includes vector - valued environmental data and equals the product of
    * the vector dimension and the total number of grid points
    */
   DisUint32 mTotalValue = 0;

   /**!
    * This field shall specify the total number of data values at each grid point and
    * accommodates scalar - or vector - valued environmental data.Vector Dimension shall be one for
    * scalar data and shall be greater than one when multiple enumerated environmental data values are
    * sent for each grid location(e.g., u, v, w wind components have V = 3).
    */
   DisUint8 mVectorDimension = 0;

   /*!
    * Padding
    */
   DisUint8 mPadding1 = 0;

   /*!
    * Padding
    */
   DisUint16 mPadding2 = 0;

   DisPtrContainer<DisGridAxisDescriptorBaseRecord> mGridAccessDescriptors;

   DisPtrContainer<DisGridDataRecord> mGridData;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 52;
};
#endif
