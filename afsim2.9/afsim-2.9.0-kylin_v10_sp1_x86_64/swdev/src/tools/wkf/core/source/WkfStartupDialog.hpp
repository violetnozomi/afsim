// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WkfStartupDialog_HPP
#define WkfStartupDialog_HPP

#include "wkf_export.h"

#include <random>

#include <QAbstractButton>
#include <QClipboard>
#include <QDesktopServices>
#include <QDialog>
#include <QDirIterator>
#include <QFileInfo>
#include <QLabel>
#include <QMenu>
#include <QMimeData>
#include <QResizeEvent>
#include <QToolTip>

#include "UtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPluginManager.hpp"
#include "WkfStartupWidgetWrapper.hpp"
#include "ui_WkfStartupDialog.h"

namespace wkf
{
namespace detail
{
constexpr auto urlSep = ';'; // A URL separator is needed for recent links that include multiple files

template<typename T>
QString ConvertToString(const T& aValue)
{
   return aValue;
}

template<>
inline QString ConvertToString<QStringList>(const QStringList& aValue)
{
   return aValue.join(urlSep);
}

template<typename T>
T SplitString(const QString& aString)
{
   return aString;
}

template<>
inline QStringList SplitString(const QString& aString)
{
   return aString.split(urlSep, QString::SkipEmptyParts);
}
} // namespace detail

// A label class that creates a styled hyper-link for navigating. Changes color of hyper-link when entering/leaving
class WKF_EXPORT StartupDialogLabel : public QLabel
{
public:
   StartupDialogLabel(const QString& link, const QString& text, QWidget* parent = nullptr);

protected:
   void UpdateLink(bool aHovered);
   void enterEvent(QEvent* event) override;
   void leaveEvent(QEvent* event) override;

   QString mLink;
   QString mCachedText;
};

// A label that will provide a hyper-link for a URL(s) representing a recent selection.
// Elides text, e.g. for long filenames, and provides a context menu with a number of actions.
template<class URL_TYPE>
class RecentSelectionLabel : public StartupDialogLabel
{
   using DisplayNameFunction = std::function<QString(const URL_TYPE&, bool)>;

public:
   RecentSelectionLabel(const URL_TYPE& url, const DisplayNameFunction& aFunction, QWidget* parent = nullptr)
      : StartupDialogLabel(detail::ConvertToString(url), "", parent)
      , mElideMode(Qt::ElideMiddle)
      , mUrl(url)
      , mDisplayNameFunction(aFunction)
   {
      // Set contents margin > 0, due to a "bug" that prevents linkHovered signal from being emitted when cursor leaves link
      setContentsMargins(1, 1, 1, 1);
      setContextMenuPolicy(Qt::CustomContextMenu);
      connect(this,
              &QLabel::linkHovered,
              [this](const QString& link)
              {
                 QString tip(link);
                 QToolTip::showText(QCursor::pos(), tip.replace(detail::urlSep, '\n'), this);
              });
   }

   void setElideMode(Qt::TextElideMode elideMode)
   {
      mElideMode = elideMode;
      updateGeometry();
   }

private:
   void resizeEvent(QResizeEvent* e) override
   {
      QLabel::resizeEvent(e);
      mCachedText = fontMetrics().elidedText(mDisplayNameFunction(mUrl, false), mElideMode, e->size().width());
      UpdateLink(false);
   }

   Qt::TextElideMode   mElideMode;
   URL_TYPE            mUrl;
   DisplayNameFunction mDisplayNameFunction;
};


namespace StartupDialogLayouts
{
const QString cRECENTS{"recentsPluginLayout"};
const QString cDOCUMENTATION{"docPluginLayout"};
const QString cTIPS{"tipPluginLayout"};
const QString cNEW{"newPluginLayout"};
}; // namespace StartupDialogLayouts

// Template argument is type representing the selection (e.g. file or files) chosen by startup dialog
template<class SELECTION_TYPE>
class StartupDialog : public QDialog
{
public:
   StartupDialog(SELECTION_TYPE& aSelection, QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

   int exec() override;

protected:
   Ui::WkfStartupDialog           mUi;
   SELECTION_TYPE&                mSelection;
   QList<QPair<QString, QString>> mDocLinks;

private:
   virtual SELECTION_TYPE               Browse()                                                                   = 0;
   virtual void                         ClearRecents()                                                             = 0;
   virtual const QList<SELECTION_TYPE>& GetRecents() const                                                         = 0;
   virtual void                         RemoveRecent(const SELECTION_TYPE& aRecent)                                = 0;
   virtual QString        GetRecentLabelDisplayName(const SELECTION_TYPE& aSelection, bool aDisplayFullPath) const = 0;
   virtual QString        RunActionText() const                                                                    = 0;
   virtual void           BuildRecentContextMenu(QMenu& aMenu, const SELECTION_TYPE& aLink) {}
   virtual bool           ValidateSelection()                             = 0;
   virtual SELECTION_TYPE ParseDroppedUrls(const QList<QUrl>& urls) const = 0;
   virtual void           PopulateLabels(){};

   void PopulateDocumentation();
   void PopulateRecents();
   void PopulateTips();
   void PopulatePluginStartupWidgets();
   void CreateDocLink(const QString& link, const QString& text);
   void DialogButtonClicked(QAbstractButton* button);
   void BrowseButtonClicked();
   void RecentLinkActivated(const QString& link);
   void RecentLabelContextMenuRequested(const QPoint& pos, const SELECTION_TYPE& link);

   void accept() override;
   void dragEnterEvent(QDragEnterEvent* event) override;
   void dropEvent(QDropEvent* event) override;

   int         mPreviousTipIndex;
   QStringList mTipsList;
};

template<class SELECTION_TYPE>
int StartupDialog<SELECTION_TYPE>::exec()
{
   PopulateLabels();
   PopulateRecents();
   PopulateDocumentation();
   PopulateTips();
   PopulatePluginStartupWidgets();

   return QDialog::exec();
}

template<class SELECTION_TYPE>
StartupDialog<SELECTION_TYPE>::StartupDialog(SELECTION_TYPE& aSelection,
                                             QWidget*        aParent,
                                             Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QDialog(aParent, aFlags)
   , mSelection(aSelection)
   , mPreviousTipIndex(0)
{
   mUi.setupUi(this);

   auto* okButton = mUi.buttonBox->button(QDialogButtonBox::Ok);
   okButton->setText("Run");
   okButton->setEnabled(false);
   mUi.buttonBox->button(QDialogButtonBox::Reset)->setText("Clear Recents");

   connect(mUi.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
   connect(mUi.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
   connect(mUi.buttonBox, &QDialogButtonBox::clicked, this, &StartupDialog::DialogButtonClicked);
   connect(mUi.browsePushButton, &QPushButton::clicked, this, &StartupDialog::BrowseButtonClicked);
   connect(mUi.filesLineEdit,
           &QLineEdit::textChanged,
           [okButton](const QString& text) { okButton->setEnabled(!text.isEmpty()); });
   connect(mUi.tipPushButton, &QPushButton::clicked, this, &StartupDialog::PopulateTips);

   mTipsList = wkfEnv.GetApplicationTips();
   if (mTipsList.empty())
   {
      mUi.tipDescription->setText("No tips are currently available.");
      mUi.tipPushButton->hide();
   }
   else
   {
      // Initialize to a random tip upon construction
      std::random_device                          randomdevice;
      std::mt19937                                mt(randomdevice());
      std::uniform_int_distribution<unsigned int> randomNumber(0, (unsigned int)mTipsList.size() - 1);
      mPreviousTipIndex = randomNumber(mt);
   }
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::CreateDocLink(const QString& link, const QString& text)
{
   if (QFileInfo::exists(link))
   {
      QLabel* labelPtr = new StartupDialogLabel("file:///" + link, text, mUi.docGroupBox);
      labelPtr->setOpenExternalLinks(true);
      mUi.docLayout->addWidget(labelPtr);
   }
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::DialogButtonClicked(QAbstractButton* button)
{
   switch (mUi.buttonBox->buttonRole(button))
   {
   case QDialogButtonBox::ResetRole:
      ClearRecents();
      PopulateRecents();
      break;

   case QDialogButtonBox::AcceptRole:
   {
      auto selection = detail::SplitString<SELECTION_TYPE>(mUi.filesLineEdit->text());
      mSelection.clear();
      for (QString file : QStringList(selection))
      {
         mSelection.push_back(file);
      }
      break;
   }

   default:
      break;
   }
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::BrowseButtonClicked()
{
   SELECTION_TYPE selection = Browse();
   if (!selection.isEmpty())
   {
      mUi.filesLineEdit->setText(detail::ConvertToString(selection));
   }
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::RecentLinkActivated(const QString& link)
{
   auto selection = detail::SplitString<SELECTION_TYPE>(link);
   mSelection.clear();

   for (QString filePath : QStringList(selection))
   {
      QFileInfo fi(filePath);
      if (!fi.exists())
      {
         QMessageBox::StandardButton button =
            QMessageBox::warning(this,
                                 "File not found",
                                 QString("File %1\ndoes not exist. Remove from recent list?").arg(filePath),
                                 QMessageBox::Yes | QMessageBox::No);
         if (button == QMessageBox::Yes)
         {
            RemoveRecent(selection);
            PopulateRecents();
         }
         return;
      }
      else
      {
         mSelection.push_back(filePath);
      }
   }

   accept();
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::RecentLabelContextMenuRequested(const QPoint& pos, const SELECTION_TYPE& link)
{
   QMenu menu(this);

   connect(menu.addAction(RunActionText()),
           &QAction::triggered,
           [=]() { RecentLinkActivated(detail::ConvertToString(link)); });
   connect(menu.addAction("Remove From List"),
           &QAction::triggered,
           [=]()
           {
              RemoveRecent(link);
              PopulateRecents();
           });
   menu.addSeparator();
   connect(menu.addAction("Open Containing Folder"),
           &QAction::triggered,
           [link]()
           {
              for (const auto& file : QStringList(link))
              {
                 QFileInfo fi(file);
                 QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
              }
           });
   connect(menu.addAction("Copy Path to Clipboard"),
           &QAction::triggered,
           [link]() { qApp->clipboard()->setText(detail::ConvertToString(link)); });
   BuildRecentContextMenu(menu, link);

   menu.exec(pos);
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::accept()
{
   if (mSelection.isEmpty())
   {
      QMessageBox::warning(this, "Empty Selection", "Please make a selection to continue.");
   }
   else
   {
      if (ValidateSelection())
      {
         QDialog::accept();
      }
   }
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::dragEnterEvent(QDragEnterEvent* event)
{
   event->setAccepted(event->mimeData()->hasUrls());
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::dropEvent(QDropEvent* event)
{
   if (event->mimeData()->hasUrls())
   {
      auto urls = event->mimeData()->urls();
      auto text = ParseDroppedUrls(urls);
      mUi.filesLineEdit->setText(detail::ConvertToString(text));
   }
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::PopulateRecents()
{
   // Clear recents list before populating
   auto* layoutPtr = qobject_cast<QBoxLayout*>(mUi.recentsLayout);
   while (layoutPtr->count() > 0)
   {
      QLayoutItem* itemPtr = layoutPtr->takeAt(0);
      delete itemPtr->widget();
      delete itemPtr;
   }

   auto recents = GetRecents();
   for (const auto& url : recents)
   {
      auto* labelPtr = new RecentSelectionLabel<SELECTION_TYPE>(
         url,
         [this](const SELECTION_TYPE& s, bool b) { return GetRecentLabelDisplayName(s, b); },
         mUi.recentsGroupBox);
      layoutPtr->addWidget(labelPtr);

      // Use queued connections because the receiver may end up deleting the label that emitted the signal, for example,
      // "Remove From List" in context menu. Queued connections are safe because they won't execute until control
      // returns to the event loop.
      connect(
         labelPtr,
         &QWidget::customContextMenuRequested,
         this,
         [=](const QPoint& pos) { RecentLabelContextMenuRequested(labelPtr->mapToGlobal(pos), url); },
         Qt::QueuedConnection);
      connect(labelPtr, &QLabel::linkActivated, this, &StartupDialog::RecentLinkActivated, Qt::QueuedConnection);
   }

   if (layoutPtr->count() == 0)
   {
      QLabel* labelPtr = new QLabel("<p style=\"font-size:10pt;\"><i>No recents found</i></p>", mUi.recentsGroupBox);
      layoutPtr->addWidget(labelPtr);
   }

   layoutPtr->addStretch();
}
template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::PopulateTips()
{
   if (!mTipsList.empty())
   {
      QString text("<p style = 'white-space:pre-wrap'>%1</p>");
      mPreviousTipIndex = (mPreviousTipIndex + 1) % mTipsList.size();
      mUi.tipDescription->setText(text.arg(mTipsList[mPreviousTipIndex]));
   }
   else
   {
      mUi.tipDescription->setText("No tips are currently available.");
   }
}


template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::PopulateDocumentation()
{
   QFileInfo fi(wkfEnv.GetDocumentationDir());

   QBoxLayout* layoutPtr = qobject_cast<QBoxLayout*>(mUi.docLayout);
   if (fi.isDir())
   {
      QString dir(fi.absoluteFilePath().append("/html/docs/"));
      foreach (auto doc, mDocLinks)
      {
         CreateDocLink(dir + doc.first, doc.second);
      }
   }

   if (layoutPtr->count() == 0)
   {
      QLabel* labelPtr = new QLabel("<p style=\"font-size:10pt;\"><i>No documentation found</i></p>", mUi.docGroupBox);
      layoutPtr->addWidget(labelPtr);
   }
   layoutPtr->addStretch();
}

template<class SELECTION_TYPE>
void StartupDialog<SELECTION_TYPE>::PopulatePluginStartupWidgets()
{
   QList<QList<wkf::StartupWidgetWrapper*>> startupWidgets;
   auto                                     plugins = wkfEnv.GetPluginManager()->GetLoadedPlugins();
   for (auto& p : plugins)
   {
      startupWidgets.append(p.second->GetStartupWidgets());
   }

   for (auto& curList : startupWidgets)
   {
      for (auto& curStartupWidget : curList)
      {
         QBoxLayout* layoutWidget = findChild<QBoxLayout*>(curStartupWidget->GetTargetGroupBox());
         if (layoutWidget)
         {
            if (curStartupWidget->GetLayout() != nullptr)
            {
               layoutWidget->addLayout(curStartupWidget->GetLayout());
            }
            else if (curStartupWidget->GetWidget() != nullptr)
            {
               connect(curStartupWidget, &wkf::StartupWidgetWrapper::CloseStartDialog, this, &QDialog::close);
               layoutWidget->addWidget(curStartupWidget->GetWidget());
            }
         }
      }
   }
}
} // namespace wkf
#endif
