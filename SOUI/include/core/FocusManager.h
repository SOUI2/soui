/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       FocusManager.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI中DUI窗口的焦点管理模块
*/

#pragma once
#include "core/Swnd.h"
#include "Accelerator.h"
#include "helper/copylist.hpp"

namespace SOUI
{


    // FocusSearch is an object that implements the algorithm to find the
    // next view to focus.
    class FocusSearch
    {
    public:

        // Constructor.
        // - |root| is the root of the view hierarchy to traverse. Focus will be
        //   trapped inside.
        // - |cycle| should be true if you want FindNextFocusableView to cycle back
        //           to the first view within this root when the traversal reaches
        //           the end. If this is true, then if you pass a valid starting
        //           view to FindNextFocusableView you will always get a valid view
        //           out, even if it's the same view.
        // - |accessibility_mode| should be true if full keyboard accessibility is
        //   needed and you  want to check IsAccessibilityFocusableInRootView(),
        //   rather than IsFocusableInRootView().
        FocusSearch(SWindow * root, bool cycle);
        virtual ~FocusSearch() {}

        // Finds the next view that should be focused and returns it. If a
        // FocusTraversable is found while searching for the focusable view,
        // returns NULL and sets |focus_traversable| to the FocusTraversable
        // and |focus_traversable_view| to the view associated with the
        // FocusTraversable.
        //
        // Return NULL if the end of the focus loop is reached, unless this object
        // was initialized with |cycle|=true, in which case it goes back to the
        // beginning when it reaches the end of the traversal.
        // - |starting_view| is the view that should be used as the starting point
        //   when looking for the previous/next view. It may be NULL (in which case
        //   the first/last view should be used depending if normal/reverse).
        // - |reverse| whether we should find the next (reverse is false) or the
        //   previous (reverse is true) view.
        // - |direction| specifies whether we are traversing down (meaning we should
        //   look into child views) or traversing up (don't look at child views).
        // - |check_starting_view| is true if starting_view may obtain the next focus.
        // - |focus_traversable| is set to the focus traversable that should be
        //   traversed if one is found (in which case the call returns NULL).
        // - |focus_traversable_view| is set to the view associated with the
        //   FocusTraversable set in the previous parameter (it is used as the
        //   starting view when looking for the next focusable view).
        SWindow* FindNextFocusableView(SWindow* starting_view,
            bool reverse,
            bool check_starting_view);

    private:
        // Convenience method that returns true if a view is focusable and does not
        // belong to the specified group.
        bool IsViewFocusableCandidate(SWindow* v,SWindow *pGroupOwner);

        // Convenience method; returns true if a view is not NULL and is focusable
        // (checking IsAccessibilityFocusableInRootView() if accessibility_mode_ is
        // true).
        bool IsFocusable(SWindow* view);

        // Returns the next focusable view or view containing a FocusTraversable
        // (NULL if none was found), starting at the starting_view.
        // |check_starting_view|, |can_go_up| and |can_go_down| controls the
        // traversal of the views hierarchy. |skip_group_id| specifies a group_id,
        // -1 means no group. All views from a group are traversed in one pass.
        SWindow* FindNextFocusableViewImpl(SWindow* starting_view,
            bool check_starting_view,
            bool can_go_up,
            bool can_go_down, 
            SWindow * pSkipGroupOwner);

        // Same as FindNextFocusableViewImpl but returns the previous focusable view.
        SWindow* FindPreviousFocusableViewImpl(SWindow* starting_view,
            bool check_starting_view,
            bool can_go_up,
            bool can_go_down,
            SWindow * pSkipGroupOwner);

        SWindow* root_;
        bool cycle_;
    };

    /**
    * @class      CFocusManager
    * @brief      焦点管理对象
    * 
    * Describe    
    */
    class SOUI_EXP CFocusManager : public IAcceleratorMgr
    {
    public:
        // The reason why the focus changed.
        enum FocusChangeReason
        {
			// The focus changed due to a click or a shortcut to jump directly to
			// a particular view.
			kReasonDirectFocusChange = 0,

            // The focus changed because the user traversed focusable views using
            // keys like Tab or Shift+Tab.
            kReasonFocusTraversal,

            // The focus changed due to restoring the focus.
            kReasonFocusRestore,

        };

        CFocusManager(SWindow *pOwner);
        ~CFocusManager(void);

        BOOL IsTabTraversalKey(UINT vKey);
        BOOL OnKeyDown(UINT vKey);

        // Advances the focus (backward if reverse is true).
        void AdvanceFocus(bool reverse);

        // Low-level methods to force the focus to change (and optionally provide
        // a reason). If the focus change should only happen if the view is
        // currenty focusable, enabled, and visible, call view->RequestFocus().
        void SetFocusedHwndWithReason(SWND swnd, FocusChangeReason reason);
        void SetFocusedHwnd(SWND swnd)
        {
            SetFocusedHwndWithReason(swnd, kReasonDirectFocusChange);
        }

        SWND GetFocusedHwnd(){return focused_view_;}

        // Stores and restores the focused view. Used when the window becomes
        // active/inactive.
        void StoreFocusedView();
        void RestoreFocusedView();

        bool IsFocusChanging(){return is_changing_focus_;}

        // IAcceleratorMgr

        // Register a keyboard accelerator for the specified target. If multiple
        // targets are registered for an accelerator, a target registered later has
        // higher priority.
        // Note that we are currently limited to accelerators that are either:
        // - a key combination including Ctrl or Alt
        // - the escape key
        // - the enter key
        // - any F key (F1, F2, F3 ...)
        // - any browser specific keys (as available on special keyboards)
        void RegisterAccelerator(const CAccelerator& accelerator,
            IAcceleratorTarget* target);

        // Unregister the specified keyboard accelerator for the specified target.
        void UnregisterAccelerator(const CAccelerator& accelerator,
            IAcceleratorTarget* target);

        // Unregister all keyboard accelerator for the specified target.
        void UnregisterAccelerators(IAcceleratorTarget* target);

        // Activate the target associated with the specified accelerator.
        // First, AcceleratorPressed handler of the most recently registered target
        // is called, and if that handler processes the event (i.e. returns true),
        // this method immediately returns. If not, we do the same thing on the next
        // target, and so on.
        // Returns true if an accelerator was activated.
        bool ProcessAccelerator(const CAccelerator& accelerator);
    private:
        // Returns the next focusable view.
        SWindow * GetNextFocusableView(SWindow* pWndStarting, bool bReverse, bool bLoop);

        // Clears the focused view. The window associated with the top root view gets
        // the native focus (so we still get keyboard events).
        void ClearFocus();

        // Validates the focused view, clearing it if the window it belongs too is not
        // attached to the window hierarchy anymore.
        void ValidateFocusedView();
    private:
        // The view that currently is focused.
        SWND  focused_view_;

        SWND  focused_backup_;

        bool is_changing_focus_;

        FocusChangeReason focus_change_reason_;

        SWindow *m_pOwner;

        typedef SList<IAcceleratorTarget*> AcceleratorTargetList;
        typedef SMap<CAccelerator, AcceleratorTargetList> AcceleratorMap;
        AcceleratorMap accelerators_;

    };


}//end of namespace SOUI
