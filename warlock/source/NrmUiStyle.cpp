/**
 * @file NrmUiStyle.cpp
 * @brief Implements the fixed visual style used by the Warlock resource panel.
 */

#include "NrmUiStyle.hpp"

#include <QString>
#include <QWidget>

namespace
{
QString PointSize(double aPointSize)
{
   return QString::number(aPointSize, 'f', 1);
}

QString PixelSize(int aPixels)
{
   return QString::number(aPixels);
}

QString StyleSheet()
{
   QString style = QString::fromUtf8(R"QSS(
      QWidget#NrmRoot,
      QWidget#NrmRoot * {
         color: #dce7f5;
         font-family: "Noto Sans CJK SC", "Microsoft YaHei UI", sans-serif;
         font-size: @BASE_FONT@pt;
      }
      QWidget#NrmRoot {
         background: #0b1220;
      }
      QFrame#NrmHero {
         background: #111d30;
         border: 1px solid #24344e;
         border-radius: @HERO_RADIUS@px;
      }
      QLabel#NrmHeroTitle {
         color: #f5f9ff;
         font-size: @HERO_TITLE_FONT@pt;
         font-weight: 700;
      }
      QLabel#NrmHeroSubtitle {
         color: #8fa8c5;
         font-size: @HERO_SUBTITLE_FONT@pt;
      }
      QLabel#acceptanceTitle {
         color: #f5f9ff;
         font-size: @HERO_TITLE_FONT@pt;
         font-weight: 700;
      }
      QLabel#acceptanceSubtitle,
      QLabel#AcceptanceStepDescription {
         color: #8fa8c5;
      }
      QLabel#AcceptanceStepTitle {
         color: #dff3ff;
         font-weight: 650;
      }
      QFrame#AcceptanceSummaryCard,
      QFrame#AcceptanceStepCard {
         background: #101a2b;
         border: 1px solid #22324a;
         border-radius: @CARD_RADIUS@px;
      }
      QFrame#NrmMetricCard {
         background: #101a2b;
         border: 1px solid #22324a;
         border-radius: @CARD_RADIUS@px;
      }
      QLabel#NrmMetricTitle {
         color: #7f96b2;
         font-size: @METRIC_TITLE_FONT@pt;
      }
      QLabel#NrmMetricValue {
         color: #f2f7ff;
         font-size: @METRIC_VALUE_FONT@pt;
         font-weight: 650;
      }
      QTabWidget::pane {
         background: #0f1929;
         border: 1px solid #22324a;
         border-radius: @PANE_RADIUS@px;
         top: -1px;
      }
      QTabBar::tab {
         background: transparent;
         color: #8fa3bd;
         border: none;
         border-bottom: 2px solid transparent;
         padding: @TAB_PADDING_V@px @TAB_PADDING_H@px;
         margin-right: @TAB_MARGIN@px;
      }
      QTabBar::tab:hover { color: #dbeafe; background: #142238; }
      QTabBar::tab:selected {
         color: #66c7ff;
         background: #132238;
         border-bottom: 2px solid #38bdf8;
         font-weight: 600;
      }
      QTableWidget {
         background: #0d1726;
         alternate-background-color: #111d2e;
         color: #d8e4f2;
         border: none;
         border-radius: @INPUT_RADIUS@px;
         gridline-color: transparent;
         selection-background-color: #164e73;
         selection-color: #ffffff;
      }
      QHeaderView::section {
         background: #16243a;
         color: #9fb4ce;
         border: none;
         border-right: 1px solid #243650;
         border-bottom: 1px solid #2a3d59;
         padding: @HEADER_PADDING_V@px @HEADER_PADDING_H@px;
         font-weight: 600;
      }
      QTableCornerButton::section { background: #16243a; border: none; }
      QLineEdit, QComboBox, QDoubleSpinBox, QTextEdit {
         background: #0b1524;
         color: #edf5ff;
         border: 1px solid #2a3d59;
         border-radius: @INPUT_RADIUS@px;
         padding: @INPUT_PADDING_V@px @INPUT_PADDING_H@px;
         selection-background-color: #1479b8;
      }
      QLineEdit:focus, QComboBox:focus, QDoubleSpinBox:focus, QTextEdit:focus {
         border: 1px solid #38bdf8;
      }
      QComboBox::drop-down { border: none; width: @COMBO_WIDTH@px; }
      QPushButton {
         background: #1479b8;
         color: #ffffff;
         border: 1px solid #2998d2;
         border-radius: @INPUT_RADIUS@px;
         min-height: @BUTTON_HEIGHT@px;
         padding: @BUTTON_PADDING_V@px @BUTTON_PADDING_H@px;
         font-weight: 600;
      }
      QPushButton:hover { background: #188aca; border-color: #5bc7f4; }
      QPushButton:pressed { background: #0f6398; }
      QPushButton:disabled { background: #253247; color: #71839a; border-color: #34445b; }
      QLabel { color: #d7e3f2; }
      QScrollBar:vertical {
         background: #0b1422;
         width: @SCROLL_WIDTH@px;
         margin: 0;
      }
      QScrollBar::handle:vertical {
         background: #334a67;
         border-radius: @SCROLL_RADIUS@px;
         min-height: @SCROLL_HANDLE_HEIGHT@px;
      }
      QScrollBar::handle:vertical:hover { background: #456381; }
      QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
   )QSS");

   style.replace("@BASE_FONT@", PointSize(10.0));
   style.replace("@HERO_TITLE_FONT@", PointSize(17.0));
   style.replace("@HERO_SUBTITLE_FONT@", PointSize(9.0));
   style.replace("@METRIC_TITLE_FONT@", PointSize(8.0));
   style.replace("@METRIC_VALUE_FONT@", PointSize(11.0));
   style.replace("@HERO_RADIUS@", PixelSize(10));
   style.replace("@CARD_RADIUS@", PixelSize(8));
   style.replace("@PANE_RADIUS@", PixelSize(8));
   style.replace("@INPUT_RADIUS@", PixelSize(6));
   style.replace("@TAB_PADDING_V@", PixelSize(9));
   style.replace("@TAB_PADDING_H@", PixelSize(13));
   style.replace("@TAB_MARGIN@", PixelSize(2));
   style.replace("@HEADER_PADDING_V@", PixelSize(7));
   style.replace("@HEADER_PADDING_H@", PixelSize(8));
   style.replace("@INPUT_PADDING_V@", PixelSize(6));
   style.replace("@INPUT_PADDING_H@", PixelSize(8));
   style.replace("@COMBO_WIDTH@", PixelSize(24));
   style.replace("@BUTTON_HEIGHT@", PixelSize(24));
   style.replace("@BUTTON_PADDING_V@", PixelSize(6));
   style.replace("@BUTTON_PADDING_H@", PixelSize(14));
   style.replace("@SCROLL_WIDTH@", PixelSize(10));
   style.replace("@SCROLL_RADIUS@", PixelSize(5));
   style.replace("@SCROLL_HANDLE_HEIGHT@", PixelSize(28));
   return style;
}
} // namespace

void WkNrm::UiStyle::Apply(QWidget* aRootPtr)
{
   if (aRootPtr == nullptr)
   {
      return;
   }

   aRootPtr->setStyleSheet(StyleSheet());
   aRootPtr->updateGeometry();
}
