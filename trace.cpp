#include <QEvent>

#include "trace.h"

int Trace::indent;

Trace::EventName Trace::eventTab[] = {
    {QEvent::None, "QEvent::None"},
    {QEvent::Timer, "QEvent::Timer"},
    {QEvent::MouseButtonPress, "QEvent::MouseButtonPress"},
    {QEvent::MouseButtonRelease, "QEvent::MouseButtonRelease"},
    {QEvent::MouseButtonDblClick, "QEvent::MouseButtonDblClick"},
    {QEvent::MouseMove, "QEvent::MouseMove"},
    {QEvent::KeyPress, "QEvent::KeyPress"},
    {QEvent::KeyRelease, "QEvent::KeyRelease"},
    {QEvent::FocusIn, "QEvent::FocusIn"},
    {QEvent::FocusOut, "QEvent::FocusOut"},
    {QEvent::Enter, "QEvent::Enter"},
    {QEvent::Leave, "QEvent::Leave"},
    {QEvent::Paint, "QEvent::Paint"},
    {QEvent::Move, "QEvent::Move"},
    {QEvent::Resize, "QEvent::Resize"},
    {QEvent::Create, "QEvent::Create"},
    {QEvent::Destroy, "QEvent::Destroy"},
    {QEvent::Show, "QEvent::Show"},
    {QEvent::Hide, "QEvent::Hide"},
    {QEvent::Close, "QEvent::Close"},
    {QEvent::Quit, "QEvent::Quit"},
    {QEvent::ParentChange, "QEvent::ParentChange"},
    {QEvent::ParentAboutToChange, "QEvent::ParentAboutToChange"},
#ifdef QT3_SUPPORT
    {QEvent::Reparent, "QEvent::Reparent"},
#endif
    {QEvent::ThreadChange, "QEvent::ThreadChange"},
    {QEvent::WindowActivate, "QEvent::WindowActivate"},
    {QEvent::WindowDeactivate, "QEvent::WindowDeactivate"},
    {QEvent::ShowToParent, "QEvent::ShowToParent"},
    {QEvent::HideToParent, "QEvent::HideToParent"},
    {QEvent::Wheel, "QEvent::Wheel"},
    {QEvent::WindowTitleChange, "QEvent::WindowTitleChange"},
    {QEvent::WindowIconChange, "QEvent::WindowIconChange"},
    {QEvent::ApplicationWindowIconChange, "QEvent::ApplicationWindowIconChange"},
    {QEvent::ApplicationFontChange, "QEvent::ApplicationFontChange"},
    {QEvent::ApplicationLayoutDirectionChange, "QEvent::ApplicationLayoutDirectionChange"},
    {QEvent::ApplicationPaletteChange, "QEvent::ApplicationPaletteChange"},
    {QEvent::PaletteChange, "QEvent::PaletteChange"},
    {QEvent::Clipboard, "QEvent::Clipboard"},
    {QEvent::Speech, "QEvent::Speech"},
    {QEvent::MetaCall, "QEvent::MetaCall"},
    {QEvent::SockAct, "QEvent::SockAct"},
    {QEvent::WinEventAct, "QEvent::WinEventAct"},
    {QEvent::DeferredDelete, "QEvent::DeferredDelete"},
    {QEvent::DragEnter, "QEvent::DragEnter"},
    {QEvent::DragMove, "QEvent::DragMove"},
    {QEvent::DragLeave, "QEvent::DragLeave"},
    {QEvent::Drop, "QEvent::Drop"},
    {QEvent::DragResponse, "QEvent::DragResponse"},
    {QEvent::ChildAdded, "QEvent::ChildAdded"},
    {QEvent::ChildPolished, "QEvent::ChildPolished"},
#ifdef QT3_SUPPORT
    {QEvent::ChildInsertedRequest, "QEvent::ChildInsertedRequest"},
    {QEvent::ChildInserted, "QEvent::ChildInserted"},
    {QEvent::LayoutHint, "QEvent::LayoutHint"},
#endif
    {QEvent::ChildRemoved, "QEvent::ChildRemoved"},
    {QEvent::ShowWindowRequest, "QEvent::ShowWindowRequest"},
    {QEvent::PolishRequest, "QEvent::PolishRequest"},
    {QEvent::Polish, "QEvent::Polish"},
    {QEvent::LayoutRequest, "QEvent::LayoutRequest"},
    {QEvent::UpdateRequest, "QEvent::UpdateRequest"},
    {QEvent::UpdateLater, "QEvent::UpdateLater"},

    {QEvent::EmbeddingControl, "QEvent::EmbeddingControl"},
    {QEvent::ActivateControl, "QEvent::ActivateControl"},
    {QEvent::DeactivateControl, "QEvent::DeactivateControl"},
    {QEvent::ContextMenu, "QEvent::ContextMenu"},
    {QEvent::InputMethod, "QEvent::InputMethod"},
    {QEvent::AccessibilityPrepare, "QEvent::AccessibilityPrepare"},
    {QEvent::TabletMove, "QEvent::TabletMove"},
    {QEvent::LocaleChange, "QEvent::LocaleChange"},
    {QEvent::LanguageChange, "QEvent::LanguageChange"},
    {QEvent::LayoutDirectionChange, "QEvent::LayoutDirectionChange"},
    {QEvent::Style, "QEvent::Style"},
    {QEvent::TabletPress, "QEvent::TabletPress"},
    {QEvent::TabletRelease, "QEvent::TabletRelease"},
    {QEvent::OkRequest, "QEvent::OkRequest"},
    {QEvent::HelpRequest, "QEvent::HelpRequest"},

    {QEvent::IconDrag, "QEvent::IconDrag"},

    {QEvent::FontChange, "QEvent::FontChange"},
    {QEvent::EnabledChange, "QEvent::EnabledChange"},
    {QEvent::ActivationChange, "QEvent::ActivationChange"},
    {QEvent::StyleChange, "QEvent::StyleChange"},
    {QEvent::IconTextChange, "QEvent::IconTextChange"},
    {QEvent::ModifiedChange, "QEvent::ModifiedChange"},
    {QEvent::MouseTrackingChange, "QEvent::MouseTrackingChange"},

    {QEvent::WindowBlocked, "QEvent::WindowBlocked"},
    {QEvent::WindowUnblocked, "QEvent::WindowUnblocked"},
    {QEvent::WindowStateChange, "QEvent::WindowStateChange"},

    {QEvent::ToolTip, "QEvent::ToolTip"},
    {QEvent::WhatsThis, "QEvent::WhatsThis"},
    {QEvent::StatusTip, "QEvent::StatusTip"},

    {QEvent::ActionChanged, "QEvent::ActionChanged"},
    {QEvent::ActionAdded, "QEvent::ActionAdded"},
    {QEvent::ActionRemoved, "QEvent::ActionRemoved"},

    {QEvent::FileOpen, "QEvent::FileOpen"},

    {QEvent::Shortcut, "QEvent::Shortcut"},
    {QEvent::ShortcutOverride, "QEvent::ShortcutOverride"},

#ifdef QT3_SUPPORT
    {QEvent::Accel, "QEvent::Accel"},
    {QEvent::AccelAvailable, "QEvent::AccelAvailable"},
    {QEvent::AccelOverride, "QEvent::AccelOverride"},
#endif

    {QEvent::WhatsThisClicked, "QEvent::WhatsThisClicked"},

#ifdef QT3_SUPPORT
    {QEvent::CaptionChange, "QEvent::CaptionChange"},
    {QEvent::IconChange, "QEvent::IconChange"},
#endif
    {QEvent::ToolBarChange, "QEvent::ToolBarChange"},

    {QEvent::ApplicationActivate, "QEvent::ApplicationActivate"},
    {QEvent::ApplicationActivated, "QEvent::ApplicationActivated"},
    {QEvent::ApplicationDeactivate, "QEvent::ApplicationDeactivate"},
    {QEvent::ApplicationDeactivated, "QEvent::ApplicationDeactivated"},

    {QEvent::QueryWhatsThis, "QEvent::QueryWhatsThis"},
    {QEvent::EnterWhatsThisMode, "QEvent::EnterWhatsThisMode"},
    {QEvent::LeaveWhatsThisMode, "QEvent::LeaveWhatsThisMode"},

    {QEvent::ZOrderChange, "QEvent::ZOrderChange"},

    {QEvent::HoverEnter, "QEvent::HoverEnter"},
    {QEvent::HoverLeave, "QEvent::HoverLeave"},
    {QEvent::HoverMove, "QEvent::HoverMove"},

    {QEvent::AccessibilityHelp, "QEvent::AccessibilityHelp"},
    {QEvent::AccessibilityDescription, "QEvent::AccessibilityDescription"},

#ifdef QT_KEYPAD_NAVIGATION
    {QEvent::EnterEditFocus, "QEvent::EnterEditFocus"},
    {QEvent::LeaveEditFocus, "QEvent::LeaveEditFocus"},
#endif
    {QEvent::AcceptDropsChange, "QEvent::AcceptDropsChange"},

    {QEvent::MenubarUpdated, "QEvent::MenubarUpdated"},
    {QEvent::ZeroTimerEvent, "QEvent::ZeroTimerEvent"},

    {QEvent::GraphicsSceneMouseMove, "QEvent::GraphicsSceneMouseMove"},
    {QEvent::GraphicsSceneMousePress, "QEvent::GraphicsSceneMousePress"},
    {QEvent::GraphicsSceneMouseRelease, "QEvent::GraphicsSceneMouseRelease"},
    {QEvent::GraphicsSceneMouseDoubleClick, "QEvent::GraphicsSceneMouseDoubleClick"},
    {QEvent::GraphicsSceneContextMenu, "QEvent::GraphicsSceneContextMenu"},
    {QEvent::GraphicsSceneHoverEnter, "QEvent::GraphicsSceneHoverEnter"},
    {QEvent::GraphicsSceneHoverMove, "QEvent::GraphicsSceneHoverMove"},
    {QEvent::GraphicsSceneHoverLeave, "QEvent::GraphicsSceneHoverLeave"},
    {QEvent::GraphicsSceneHelp, "QEvent::GraphicsSceneHelp"},
    {QEvent::GraphicsSceneDragEnter, "QEvent::GraphicsSceneDragEnter"},
    {QEvent::GraphicsSceneDragMove, "QEvent::GraphicsSceneDragMove"},
    {QEvent::GraphicsSceneDragLeave, "QEvent::GraphicsSceneDragLeave"},
    {QEvent::GraphicsSceneDrop, "QEvent::GraphicsSceneDrop"},
    {QEvent::GraphicsSceneWheel, "QEvent::GraphicsSceneWheel"},

    {QEvent::KeyboardLayoutChange, "QEvent::KeyboardLayoutChange"},

    {QEvent::DynamicPropertyChange, "QEvent::DynamicPropertyChange"},

    {QEvent::TabletEnterProximity, "QEvent::TabletEnterProximity"},
    {QEvent::TabletLeaveProximity, "QEvent::TabletLeaveProximity"},

    {QEvent::NonClientAreaMouseMove, "QEvent::NonClientAreaMouseMove"},
    {QEvent::NonClientAreaMouseButtonPress, "QEvent::NonClientAreaMouseButtonPress"},
    {QEvent::NonClientAreaMouseButtonRelease, "QEvent::NonClientAreaMouseButtonRelease"},
    {QEvent::NonClientAreaMouseButtonDblClick, "QEvent::NonClientAreaMouseButtonDblClick"},

    {QEvent::MacSizeChange, "QEvent::MacSizeChange"},

    {QEvent::ContentsRectChange, "QEvent::ContentsRectChange"},

    {QEvent::MacGLWindowChange, "QEvent::MacGLWindowChange"},

    {QEvent::FutureCallOut, "QEvent::FutureCallOut"},

    {QEvent::GraphicsSceneResize, "QEvent::GraphicsSceneResize"},
    {QEvent::GraphicsSceneMove, "QEvent::GraphicsSceneMove"},

    {QEvent::CursorChange, "QEvent::CursorChange"},
    {QEvent::ToolTipChange, "QEvent::ToolTipChange"},

    {QEvent::NetworkReplyUpdated, "QEvent::NetworkReplyUpdated"},

    {QEvent::GrabMouse, "QEvent::GrabMouse"},
    {QEvent::UngrabMouse, "QEvent::UngrabMouse"},
    {QEvent::GrabKeyboard, "QEvent::GrabKeyboard"},
    {QEvent::UngrabKeyboard, "QEvent::UngrabKeyboard"},
    {QEvent::CocoaRequestModal, "QEvent::CocoaRequestModal"},
    {QEvent::MacGLClearDrawable, "QEvent::MacGLClearDrawable"},

    {QEvent::StateMachineSignal, "QEvent::StateMachineSignal"},
    {QEvent::StateMachineWrapped, "QEvent::StateMachineWrapped"},

    {QEvent::TouchBegin, "QEvent::TouchBegin"},
    {QEvent::TouchUpdate, "QEvent::TouchUpdate"},
    {QEvent::TouchEnd, "QEvent::TouchEnd"},

    {QEvent::NativeGesture, "QEvent::NativeGesture"},

    {QEvent::RequestSoftwareInputPanel, "QEvent::RequestSoftwareInputPanel"},
    {QEvent::CloseSoftwareInputPanel, "QEvent::CloseSoftwareInputPanel"},

    {QEvent::UpdateSoftKeys, "QEvent::UpdateSoftKeys"},

    {QEvent::WinIdChange, "QEvent::WinIdChange"},
    {QEvent::Gesture, "QEvent::Gesture"},
    {QEvent::GestureOverride, "QEvent::GestureOverride"},

    {0, 0}
};
