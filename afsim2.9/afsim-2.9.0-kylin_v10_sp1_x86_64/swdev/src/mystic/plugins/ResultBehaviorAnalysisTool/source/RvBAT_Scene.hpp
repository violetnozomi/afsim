// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVBAT_SCENE_HPP
#define RVBAT_SCENE_HPP

#include <QGraphicsScene>
#include <QObject>

namespace RvBAT
{
/** Represents the type of scene (ABT or FSM) */
enum class SceneType
{
   cABT,
   cFSM
};

class Scene : public QGraphicsScene
{
   Q_OBJECT

public:
   explicit Scene(const size_t aSceneId, const SceneType aSceneType, QObject* aParent);
   ~Scene() override = default;

   /** Return the Id for this scene. */
   const size_t Id() const { return mId; }

   /** Return the SceneType for this scene. */
   const SceneType GetSceneType() const { return mSceneType; }

   /** Return the center point of this scene. */
   const QPointF& GetCenterPoint() const { return mCenterPoint; }

   /** Set the center point for this scene.
    * @param aCenterPoint point to set
    */
   void SetCenterPoint(const QPointF aCenterPoint) { mCenterPoint = aCenterPoint; }

   /** Get the current zoom for this scene. */
   const double GetZoom() const { return mZoom; }

   /** Set the current zoom for this scene.
    * @param aZoom zoom to set
    */
   void SetZoom(const double aZoom) { mZoom = aZoom; }

   /** Return if this scene has been initialized yet. */
   const bool Initialized() { return mInitialized; }

   /** Set whether or not this scene is initialized.
    * @param aState state to set
    */
   void SetInitialized(const bool aState) { mInitialized = aState; }

   /** Get the name of a tree from ID
    * @param aId id to look for
    */
   const QString GetTreeNameFromId(const size_t aId) const;

   /** Get the rect representing the scene. */
   virtual const QRectF GetSceneRect() const = 0;

   /** Redraw the visuals of the scene. */
   virtual void RedrawVisuals() = 0;

   /** Reset the layout for the scene. */
   virtual void ResetLayout() = 0;

   /** Return whether or not an object is at a point and
    * return the point if so.
    * @param aPointF point object to populate
    */
   virtual const bool IsObjectAtPoint(const QPointF& aPointF) const = 0;

private:
   SceneType mSceneType;
   size_t    mId;

   // Scene config vars - Used when swapping scenes to remember config.
   QPointF mCenterPoint{0.0, 0.0}; ///< Represents the center point of the scene.  Used for zoom/panning/layout
   double  mZoom{1.0};             ///< Holds the zoom of the scene
   bool    mInitialized{false};    ///< Whether or not the scene has been initialized (layout/zoom set)
};
} // namespace RvBAT
#endif // RVBAT_SCENE_HPP
