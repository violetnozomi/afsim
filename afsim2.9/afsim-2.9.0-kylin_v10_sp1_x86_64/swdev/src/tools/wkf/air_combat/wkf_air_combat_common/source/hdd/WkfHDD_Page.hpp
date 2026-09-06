// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFHDD_PAGE_HPP
#define WKFHDD_PAGE_HPP

#include "wkf_air_combat_common_export.h"

#include <QObject>
#include <QRect>

#include "VaViewer.hpp"
#include "hdd/WkfHDD_DataContainer.hpp"
#include "hdd/WkfHDD_Overlay.hpp"

namespace vespa
{
class VaViewer;
class VaOverlay;
} // namespace vespa

namespace HDD
{
// Colors for HDD Pages, to keep things consistent.
namespace Color
{
static const osg::Vec4 White(0.9, 0.9, 0.9, 1.0);
static const osg::Vec4 LightGray(0.8, 0.8, 0.8, 1.0);
static const osg::Vec4 Gray(0.5, 0.5, 0.5, 1.0);
static const osg::Vec4 Black(0.0, 0.0, 0.0, 1.0);
static const osg::Vec4 Red(0.9, 0.0, 0.0, 1.0);
static const osg::Vec4 Yellow(0.9, 0.9, 0.0, 1.0);
static const osg::Vec4 Green(0.0, 0.8, 0.2, 1.0);
static const osg::Vec4 SlateBlue(0.2, 0.51, 0.67, 1.0);
static const osg::Vec4 Cyan(0.0, 1.0, 1.0, 1.0);
static const osg::Vec4 Magenta(0.85, 0.0, 1.0, 1.0);
} // namespace Color

class Overlay;
class WKF_AIR_COMBAT_COMMON_EXPORT Page : public QObject
{
   Q_OBJECT
public:
   /**
    * |           0           | |           0           | |           0           |
    * +-----+-----+-----+-----+ +-----+-----+-----+-----+ +-----+-----+-----+-----+
    * |  1  |  2  |  3  |  4  | |     |     |     |     | |           |           |
    * |     |     |     |     | | 13  | 14  |  15 |  16 | |    17     |    18     |
    * +-----+-----+-----+-----+ |     |     |     |     | |           |           |
    * | 5|6 | 7|8 | 9|10|11|12| |     |     |     |     | |           |           |
    * +-----+-----+-----+-----+ +-----+-----+-----+-----+ +-----+-----+-----+-----+
    * 0 is reserved for the page header that goes on the top of the window.
    * 1-4  Medium-sized pages
    * 5-12 Small-sized pages
    * 13-16 Large-sized pages
    * 17-18 Full-sized pages
    */
   enum Size
   {
      Small,
      Medium,
      Large,
      Full
   };

   /** This defines a click region for a page. Depending on
    * where the user clicks and what size the page is, it will
    * trigger the paired function.  If there is no rect defined for
    * a size, the function will not be triggered.
    */
   struct ClickRegion
   {
      // Depending on which size the page is, the click region might move.
      std::map<Size, QRectF> rect;

      // This function will be called if the user clicks inside the matching rect.
      std::function<void()> function;
   };

   explicit Page(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~Page() override = default;

   /** Get the page's name. */
   const QString& GetPageName() const { return mPageName; }

   /** Get the page's position. */
   const size_t GetPosition() const { return mPosition; }

   /** Set the page's size enum.
    * @param aPosition position of the page.
    */
   void SetPageSize(const size_t aPosition);

   /** Get the rect of a page at a position. There are 19 unique positions (0-18):
    * |           0           | |           0           | |           0           |
    * +-----+-----+-----+-----+ +-----+-----+-----+-----+ +-----+-----+-----+-----+
    * |  1  |  2  |  3  |  4  | |     |     |     |     | |           |           |
    * |     |     |     |     | | 13  | 14  |  15 |  16 | |    17     |    18     |
    * +-----+-----+-----+-----+ |     |     |     |     | |           |           |
    * | 5|6 | 7|8 | 9|10|11|12| |     |     |     |     | |           |           |
    * +-----+-----+-----+-----+ +-----+-----+-----+-----+ +-----+-----+-----+-----+
    * 0 is reserved for the page header that goes on the top of the window.
    * 1-4  Medium-sized pages
    * 5-12 Small-sized pages
    * 13-16 Large-sized pages
    * 17-18 Full-sized pages
    * @param aPosition position of the page.
    * @param aRect ref to rect object to fill.
    */
   static void GetRect(const size_t aPosition, QRectF& aRect);

   /** Return the page's id. */
   const size_t Id() const { return mId; }

   /** Return if the page is currently visible. */
   virtual const bool IsVisible() const;

   /** Get the name of this page's overlay. */
   virtual const std::string& OverlayName() const;

   /** Bring the overlay to the front of the viewer. */
   virtual void BringOverlayToFront();

   /** Remove this page's overlay from the viewer. */
   virtual void RemoveOverlay();

   /** Set the page's name.
    * @param aName name to set.
    */
   void SetPageName(const QString& aName);

   /** Set the position of this page.
    * @param aPosition position to set.
    */
   virtual void SetPosition(const size_t aPosition);

   /** Set if this page should be shown.
    * @param aState true to show, false to hide.
    */
   virtual void SetVisible(const bool aState);
signals:
   /** Signal for a page being clicked on.
    * @param aPage page clicked on.
    */
   void PageClicked(Page* aPage);

protected:
   /** Add a click region to this overlay.  Clicking in it will trigger a function.
    * @param aClickRegion click region to add.  This has a rect and a function to run.
    */
   void AddClickRegion(ClickRegion aClickRegion);

   /** Loop through any click regions and if the mouse cursor was inside, trigger the regions function. */
   void HandleClickRegions(const int aMouseX, const int aMouseY);

   /** Draw the page. */
   virtual void Draw() = 0;

   /** Return the pixel width of this page from the overlay.  This is used to scale objects from
    * relative coordinates to pixel coordinates when the drawing.
    */
   const double pW() const { return (mOverlayPtr != nullptr) ? mOverlayPtr->GetWidth() : 0; }

   /** Return the pixel height of this page from the overlay, scaled from the w/h aspect ratio.
    * This is used to scale objects from relative coordinates to pixel coordinates when the drawing.
    */
   const double pH() const { return (mOverlayPtr != nullptr) ? mOverlayPtr->GetHeight() * mRatio : 0; }

   /** Get a unique int id for this node. */
   static size_t GetUniqueId() { return sId++; }

   size_t           mId;         ///< Id of the page.
   QString          mPageName;   ///< Name of the page.
   QRectF           mRect;       ///< Rect representing the x,y,w,h of the page.
   size_t           mPosition;   ///< Position of the page.
   vespa::VaViewer* mViewerPtr;  ///< Pointer to the parent viewer.
   Overlay*         mOverlayPtr; ///< Pointer to the overlay for this page.
   Size             mSize;       ///< Enum representing the size of the page.  Changes aspect ratio and what is drawn.
   double mRatio; ///< Current aspect ratio for this page.  Used for symmetrical drawing (e.g circles, squares).
   std::vector<ClickRegion> mClickRegions; ///< This holds a map of regions that can be clicked on in the page.
private:
   static size_t sId; ///< Unique id counter.
};
} // namespace HDD
#endif // WKFHDD_PAGE_HPP
