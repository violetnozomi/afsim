// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef GRAPHICAL_POINT_COLLECTION_HPP
#define GRAPHICAL_POINT_COLLECTION_HPP

#include "UtCast.hpp"
#include "UtQtGLWidgetBase.hpp"
#include "UtVec3dX.hpp"
#include "Vehicle.hpp"

#include <vector>

class QTreeWidgetItem;
class TextureFont;

namespace Designer
{
   class RingPoint
   {
      public:

         RingPoint(double x, double y, double z);
         virtual ~RingPoint();

         void SetPreviousRingPoint(RingPoint* aPreviousRingPoint);
         void SetNextRingPoint(RingPoint* aNextRingPoint);

         void SetPreviousLongitudinalPoint(RingPoint* aPreviousLongitudinalPoint);
         void SetNextLongitudinalPoint(RingPoint* aNextLongitudinalPoint);

         void CalculateNormal();
         void Draw(bool aWireframe);

         UtVec3dX GetPoint() { return mPointVec; }
         UtVec3dX GetNormal() { return mNormalVec; }

      protected:
         UtVec3dX   mPointVec;
         UtVec3dX   mNormalVec;
         RingPoint* mPreviousRingPoint         = nullptr;
         RingPoint* mNextRingPoint             = nullptr;
         RingPoint* mPreviousLongitudinalPoint = nullptr;
         RingPoint* mNextLongitudinalPoint     = nullptr;
   };

   class GraphicalPointRing
   {
      public:

         explicit GraphicalPointRing(bool aCircularList);
         virtual ~GraphicalPointRing();

         void AddPoint(double x, double y, double z);

         void SetPreviousRing(GraphicalPointRing* aPreviousRing);
         void SetNextRing(GraphicalPointRing* aNextRing);

         RingPoint* GetPoint(size_t aIndex);

         void CalculatePointConnections();
         void CalculateNormals();
         void Draw(bool aWireframe);

      protected:

         std::vector<RingPoint*> mPointList;
         GraphicalPointRing*     mPreviousRing   = nullptr;
         GraphicalPointRing*     mNextRing       = nullptr;
         bool                    mListIsCircular = false;
   };

   class GraphicalPointCollection
   {
      public:

         explicit GraphicalPointCollection(bool aCircularList);
         virtual ~GraphicalPointCollection();

         void AddRing();
         void AddPoint(double x, double y, double z);
         void CalculatePointCollection();
         void Draw(bool aWireframe);

      protected:

         std::vector<GraphicalPointRing*> mGraphicalPointRingList;
         size_t                           mCurrentRingIndex     = ut::npos;
         bool                             mCollectionIsCircular = false;
   };
}

#endif // !GRAPHICAL_POINT_COLLECTION_HPP
