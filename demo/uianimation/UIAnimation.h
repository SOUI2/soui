

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* Compiler settings for UIAnimation.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __UIAnimation_h__
#define __UIAnimation_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUIAnimationManager_FWD_DEFINED__
#define __IUIAnimationManager_FWD_DEFINED__
typedef interface IUIAnimationManager IUIAnimationManager;
#endif 	/* __IUIAnimationManager_FWD_DEFINED__ */


#ifndef __IUIAnimationVariable_FWD_DEFINED__
#define __IUIAnimationVariable_FWD_DEFINED__
typedef interface IUIAnimationVariable IUIAnimationVariable;
#endif 	/* __IUIAnimationVariable_FWD_DEFINED__ */


#ifndef __IUIAnimationStoryboard_FWD_DEFINED__
#define __IUIAnimationStoryboard_FWD_DEFINED__
typedef interface IUIAnimationStoryboard IUIAnimationStoryboard;
#endif 	/* __IUIAnimationStoryboard_FWD_DEFINED__ */


#ifndef __IUIAnimationTransition_FWD_DEFINED__
#define __IUIAnimationTransition_FWD_DEFINED__
typedef interface IUIAnimationTransition IUIAnimationTransition;
#endif 	/* __IUIAnimationTransition_FWD_DEFINED__ */


#ifndef __IUIAnimationManagerEventHandler_FWD_DEFINED__
#define __IUIAnimationManagerEventHandler_FWD_DEFINED__
typedef interface IUIAnimationManagerEventHandler IUIAnimationManagerEventHandler;
#endif 	/* __IUIAnimationManagerEventHandler_FWD_DEFINED__ */


#ifndef __IUIAnimationVariableChangeHandler_FWD_DEFINED__
#define __IUIAnimationVariableChangeHandler_FWD_DEFINED__
typedef interface IUIAnimationVariableChangeHandler IUIAnimationVariableChangeHandler;
#endif 	/* __IUIAnimationVariableChangeHandler_FWD_DEFINED__ */


#ifndef __IUIAnimationVariableIntegerChangeHandler_FWD_DEFINED__
#define __IUIAnimationVariableIntegerChangeHandler_FWD_DEFINED__
typedef interface IUIAnimationVariableIntegerChangeHandler IUIAnimationVariableIntegerChangeHandler;
#endif 	/* __IUIAnimationVariableIntegerChangeHandler_FWD_DEFINED__ */


#ifndef __IUIAnimationStoryboardEventHandler_FWD_DEFINED__
#define __IUIAnimationStoryboardEventHandler_FWD_DEFINED__
typedef interface IUIAnimationStoryboardEventHandler IUIAnimationStoryboardEventHandler;
#endif 	/* __IUIAnimationStoryboardEventHandler_FWD_DEFINED__ */


#ifndef __IUIAnimationPriorityComparison_FWD_DEFINED__
#define __IUIAnimationPriorityComparison_FWD_DEFINED__
typedef interface IUIAnimationPriorityComparison IUIAnimationPriorityComparison;
#endif 	/* __IUIAnimationPriorityComparison_FWD_DEFINED__ */


#ifndef __IUIAnimationTransitionLibrary_FWD_DEFINED__
#define __IUIAnimationTransitionLibrary_FWD_DEFINED__
typedef interface IUIAnimationTransitionLibrary IUIAnimationTransitionLibrary;
#endif 	/* __IUIAnimationTransitionLibrary_FWD_DEFINED__ */


#ifndef __IUIAnimationInterpolator_FWD_DEFINED__
#define __IUIAnimationInterpolator_FWD_DEFINED__
typedef interface IUIAnimationInterpolator IUIAnimationInterpolator;
#endif 	/* __IUIAnimationInterpolator_FWD_DEFINED__ */


#ifndef __IUIAnimationTransitionFactory_FWD_DEFINED__
#define __IUIAnimationTransitionFactory_FWD_DEFINED__
typedef interface IUIAnimationTransitionFactory IUIAnimationTransitionFactory;
#endif 	/* __IUIAnimationTransitionFactory_FWD_DEFINED__ */


#ifndef __IUIAnimationTimer_FWD_DEFINED__
#define __IUIAnimationTimer_FWD_DEFINED__
typedef interface IUIAnimationTimer IUIAnimationTimer;
#endif 	/* __IUIAnimationTimer_FWD_DEFINED__ */


#ifndef __IUIAnimationTimerUpdateHandler_FWD_DEFINED__
#define __IUIAnimationTimerUpdateHandler_FWD_DEFINED__
typedef interface IUIAnimationTimerUpdateHandler IUIAnimationTimerUpdateHandler;
#endif 	/* __IUIAnimationTimerUpdateHandler_FWD_DEFINED__ */


#ifndef __IUIAnimationTimerClientEventHandler_FWD_DEFINED__
#define __IUIAnimationTimerClientEventHandler_FWD_DEFINED__
typedef interface IUIAnimationTimerClientEventHandler IUIAnimationTimerClientEventHandler;
#endif 	/* __IUIAnimationTimerClientEventHandler_FWD_DEFINED__ */


#ifndef __IUIAnimationTimerEventHandler_FWD_DEFINED__
#define __IUIAnimationTimerEventHandler_FWD_DEFINED__
typedef interface IUIAnimationTimerEventHandler IUIAnimationTimerEventHandler;
#endif 	/* __IUIAnimationTimerEventHandler_FWD_DEFINED__ */


#ifndef __UIAnimationManager_FWD_DEFINED__
#define __UIAnimationManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class UIAnimationManager UIAnimationManager;
#else
typedef struct UIAnimationManager UIAnimationManager;
#endif /* __cplusplus */

#endif 	/* __UIAnimationManager_FWD_DEFINED__ */


#ifndef __UIAnimationTransitionLibrary_FWD_DEFINED__
#define __UIAnimationTransitionLibrary_FWD_DEFINED__

#ifdef __cplusplus
typedef class UIAnimationTransitionLibrary UIAnimationTransitionLibrary;
#else
typedef struct UIAnimationTransitionLibrary UIAnimationTransitionLibrary;
#endif /* __cplusplus */

#endif 	/* __UIAnimationTransitionLibrary_FWD_DEFINED__ */


#ifndef __UIAnimationTransitionFactory_FWD_DEFINED__
#define __UIAnimationTransitionFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class UIAnimationTransitionFactory UIAnimationTransitionFactory;
#else
typedef struct UIAnimationTransitionFactory UIAnimationTransitionFactory;
#endif /* __cplusplus */

#endif 	/* __UIAnimationTransitionFactory_FWD_DEFINED__ */


#ifndef __UIAnimationTimer_FWD_DEFINED__
#define __UIAnimationTimer_FWD_DEFINED__

#ifdef __cplusplus
typedef class UIAnimationTimer UIAnimationTimer;
#else
typedef struct UIAnimationTimer UIAnimationTimer;
#endif /* __cplusplus */

#endif 	/* __UIAnimationTimer_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 



//--------------------------------------------------------------------------
//
//  UIAnimation.h
//
//  Windows Animation interface definitions and related types and enums
//  (Generated from UIAnimation.idl)
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//
//--------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4001) 
#pragma once
#pragma warning(pop)




typedef DOUBLE UI_ANIMATION_SECONDS;

#define	UI_ANIMATION_SECONDS_EVENTUALLY	( -1 )

typedef /* [public][public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0000_0001
    {	UI_ANIMATION_UPDATE_NO_CHANGE	= 0,
	UI_ANIMATION_UPDATE_VARIABLES_CHANGED	= 1
    } 	UI_ANIMATION_UPDATE_RESULT;

typedef /* [public][public][public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0000_0002
    {	UI_ANIMATION_MANAGER_IDLE	= 0,
	UI_ANIMATION_MANAGER_BUSY	= 1
    } 	UI_ANIMATION_MANAGER_STATUS;

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0000_0003
    {	UI_ANIMATION_MODE_DISABLED	= 0,
	UI_ANIMATION_MODE_SYSTEM_DEFAULT	= 1,
	UI_ANIMATION_MODE_ENABLED	= 2
    } 	UI_ANIMATION_MODE;



#ifndef __IUIAnimationManager_INTERFACE_DEFINED__
#define __IUIAnimationManager_INTERFACE_DEFINED__

    MIDL_INTERFACE("9169896C-AC8D-4e7d-94E5-67FA4DC2F2E8")
    IUIAnimationManager : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateAnimationVariable( 
            /* [annotation][in] */ 
            __in  DOUBLE initialValue,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationVariable **variable) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE ScheduleTransition( 
            /* [annotation][in] */ 
            __in  IUIAnimationVariable *variable,
            /* [annotation][in] */ 
            __in  IUIAnimationTransition *transition,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS timeNow) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateStoryboard( 
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationStoryboard **storyboard) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE FinishAllStoryboards( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS completionDeadline) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE AbandonAllStoryboards( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Update( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS timeNow,
            /* [annotation][defaultvalue][out] */ 
            __out_opt  UI_ANIMATION_UPDATE_RESULT *updateResult = 0) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetVariableFromTag( 
            /* [annotation][unique][in] */ 
            __in_opt  IUnknown *object,
            /* [annotation][in] */ 
            __in  UINT32 id,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationVariable **variable) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetStoryboardFromTag( 
            /* [annotation][unique][in] */ 
            __in_opt  IUnknown *object,
            /* [annotation][in] */ 
            __in  UINT32 id,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationStoryboard **storyboard) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [annotation][retval][out] */ 
            __out  UI_ANIMATION_MANAGER_STATUS *status) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetAnimationMode( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_MODE mode) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetManagerEventHandler( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationManagerEventHandler *handler) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetCancelPriorityComparison( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationPriorityComparison *comparison) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetTrimPriorityComparison( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationPriorityComparison *comparison) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetCompressPriorityComparison( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationPriorityComparison *comparison) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetConcludePriorityComparison( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationPriorityComparison *comparison) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetDefaultLongestAcceptableDelay( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS delay) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Shutdown( void) = 0;
        
    };


#endif 	/* __IUIAnimationManager_INTERFACE_DEFINED__ */


typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0001_0001
    {	UI_ANIMATION_ROUNDING_NEAREST	= 0,
	UI_ANIMATION_ROUNDING_FLOOR	= 1,
	UI_ANIMATION_ROUNDING_CEILING	= 2
    } 	UI_ANIMATION_ROUNDING_MODE;


#ifndef __IUIAnimationVariable_INTERFACE_DEFINED__
#define __IUIAnimationVariable_INTERFACE_DEFINED__

/* interface IUIAnimationVariable */
/* [unique][helpstring][uuid][object][local] */ 

    
    MIDL_INTERFACE("8CEEB155-2849-4ce5-9448-91FF70E1E4D9")
    IUIAnimationVariable : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetValue( 
            /* [annotation][retval][out] */ 
            __out  DOUBLE *value) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetFinalValue( 
            /* [annotation][retval][out] */ 
            __out  DOUBLE *finalValue) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetPreviousValue( 
            /* [annotation][retval][out] */ 
            __out  DOUBLE *previousValue) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetIntegerValue( 
            /* [annotation][retval][out] */ 
            __out  INT32 *value) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetFinalIntegerValue( 
            /* [annotation][retval][out] */ 
            __out  INT32 *finalValue) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetPreviousIntegerValue( 
            /* [annotation][retval][out] */ 
            __out  INT32 *previousValue) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetCurrentStoryboard( 
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationStoryboard **storyboard) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetLowerBound( 
            /* [annotation][in] */ 
            __in  DOUBLE bound) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetUpperBound( 
            /* [annotation][in] */ 
            __in  DOUBLE bound) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetRoundingMode( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_ROUNDING_MODE mode) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetTag( 
            /* [annotation][unique][in] */ 
            __in_opt  IUnknown *object,
            /* [annotation][in] */ 
            __in  UINT32 id) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetTag( 
            /* [annotation][out] */ 
            __deref_opt_out  IUnknown **object,
            /* [annotation][out] */ 
            __out_opt  UINT32 *id) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetVariableChangeHandler( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationVariableChangeHandler *handler) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetVariableIntegerChangeHandler( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationVariableIntegerChangeHandler *handler) = 0;
        
    };
    



#endif 	/* __IUIAnimationVariable_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_UIAnimation_0000_0002 */
/* [local] */ 

typedef /* [public][public][public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0002_0001
    {	UI_ANIMATION_STORYBOARD_BUILDING	= 0,
	UI_ANIMATION_STORYBOARD_SCHEDULED	= 1,
	UI_ANIMATION_STORYBOARD_CANCELLED	= 2,
	UI_ANIMATION_STORYBOARD_PLAYING	= 3,
	UI_ANIMATION_STORYBOARD_TRUNCATED	= 4,
	UI_ANIMATION_STORYBOARD_FINISHED	= 5,
	UI_ANIMATION_STORYBOARD_READY	= 6,
	UI_ANIMATION_STORYBOARD_INSUFFICIENT_PRIORITY	= 7
    } 	UI_ANIMATION_STORYBOARD_STATUS;

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0002_0002
    {	UI_ANIMATION_SCHEDULING_UNEXPECTED_FAILURE	= 0,
	UI_ANIMATION_SCHEDULING_INSUFFICIENT_PRIORITY	= 1,
	UI_ANIMATION_SCHEDULING_ALREADY_SCHEDULED	= 2,
	UI_ANIMATION_SCHEDULING_SUCCEEDED	= 3,
	UI_ANIMATION_SCHEDULING_DEFERRED	= 4
    } 	UI_ANIMATION_SCHEDULING_RESULT;

typedef struct __MIDL___MIDL_itf_UIAnimation_0000_0002_0003
    {
    int _;
    } 	*UI_ANIMATION_KEYFRAME;

#define	UI_ANIMATION_KEYFRAME_STORYBOARD_START	( ( UI_ANIMATION_KEYFRAME  )-1 )

#define	UI_ANIMATION_REPEAT_INDEFINITELY	( -1 )


#ifndef __IUIAnimationStoryboard_INTERFACE_DEFINED__
#define __IUIAnimationStoryboard_INTERFACE_DEFINED__


    MIDL_INTERFACE("A8FF128F-9BF9-4af1-9E67-E5E410DEFB84")
    IUIAnimationStoryboard : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE AddTransition( 
            /* [annotation][in] */ 
            __in  IUIAnimationVariable *variable,
            /* [annotation][in] */ 
            __in  IUIAnimationTransition *transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE AddKeyframeAtOffset( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_KEYFRAME existingKeyframe,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS offset,
            /* [annotation][retval][out] */ 
            __out  UI_ANIMATION_KEYFRAME *keyframe) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE AddKeyframeAfterTransition( 
            /* [annotation][in] */ 
            __in  IUIAnimationTransition *transition,
            /* [annotation][retval][out] */ 
            __out  UI_ANIMATION_KEYFRAME *keyframe) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE AddTransitionAtKeyframe( 
            /* [annotation][in] */ 
            __in  IUIAnimationVariable *variable,
            /* [annotation][in] */ 
            __in  IUIAnimationTransition *transition,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_KEYFRAME startKeyframe) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE AddTransitionBetweenKeyframes( 
            /* [annotation][in] */ 
            __in  IUIAnimationVariable *variable,
            /* [annotation][in] */ 
            __in  IUIAnimationTransition *transition,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_KEYFRAME startKeyframe,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_KEYFRAME endKeyframe) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE RepeatBetweenKeyframes( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_KEYFRAME startKeyframe,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_KEYFRAME endKeyframe,
            /* [annotation][in] */ 
            __in  INT32 repetitionCount) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE HoldVariable( 
            /* [annotation][in] */ 
            __in  IUIAnimationVariable *variable) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetLongestAcceptableDelay( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS delay) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Schedule( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS timeNow,
            /* [annotation][defaultvalue][out] */ 
            __out_opt  UI_ANIMATION_SCHEDULING_RESULT *schedulingResult = 0) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Conclude( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Finish( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS completionDeadline) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Abandon( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetTag( 
            /* [annotation][unique][in] */ 
            __in_opt  IUnknown *object,
            /* [annotation][in] */ 
            __in  UINT32 id) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetTag( 
            /* [annotation][out] */ 
            __deref_opt_out  IUnknown **object,
            /* [annotation][out] */ 
            __out_opt  UINT32 *id) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [annotation][retval][out] */ 
            __out  UI_ANIMATION_STORYBOARD_STATUS *status) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetElapsedTime( 
            /* [annotation][out] */ 
            __out  UI_ANIMATION_SECONDS *elapsedTime) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetStoryboardEventHandler( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationStoryboardEventHandler *handler) = 0;
        
    };



#endif 	/* __IUIAnimationStoryboard_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationTransition_INTERFACE_DEFINED__
#define __IUIAnimationTransition_INTERFACE_DEFINED__

    MIDL_INTERFACE("DC6CE252-F731-41cf-B610-614B6CA049AD")
    IUIAnimationTransition : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetInitialValue( 
            /* [annotation][in] */ 
            __in  DOUBLE value) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetInitialVelocity( 
            /* [annotation][in] */ 
            __in  DOUBLE velocity) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE IsDurationKnown( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetDuration( 
            /* [annotation][retval][out] */ 
            __out  UI_ANIMATION_SECONDS *duration) = 0;
        
    };
    


#endif 	/* __IUIAnimationTransition_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationManagerEventHandler_INTERFACE_DEFINED__
#define __IUIAnimationManagerEventHandler_INTERFACE_DEFINED__

    MIDL_INTERFACE("783321ED-78A3-4366-B574-6AF607A64788")
    IUIAnimationManagerEventHandler : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnManagerStatusChanged( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_MANAGER_STATUS newStatus,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_MANAGER_STATUS previousStatus) = 0;
        
    };


#endif 	/* __IUIAnimationManagerEventHandler_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationVariableChangeHandler_INTERFACE_DEFINED__
#define __IUIAnimationVariableChangeHandler_INTERFACE_DEFINED__

    MIDL_INTERFACE("6358B7BA-87D2-42d5-BF71-82E919DD5862")
    IUIAnimationVariableChangeHandler : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnValueChanged( 
            /* [annotation][in] */ 
            __in  IUIAnimationStoryboard *storyboard,
            /* [annotation][in] */ 
            __in  IUIAnimationVariable *variable,
            /* [annotation][in] */ 
            __in  DOUBLE newValue,
            /* [annotation][in] */ 
            __in  DOUBLE previousValue) = 0;
        
    };


#endif 	/* __IUIAnimationVariableChangeHandler_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationVariableIntegerChangeHandler_INTERFACE_DEFINED__
#define __IUIAnimationVariableIntegerChangeHandler_INTERFACE_DEFINED__

    
    MIDL_INTERFACE("BB3E1550-356E-44b0-99DA-85AC6017865E")
    IUIAnimationVariableIntegerChangeHandler : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnIntegerValueChanged( 
            /* [annotation][in] */ 
            __in  IUIAnimationStoryboard *storyboard,
            /* [annotation][in] */ 
            __in  IUIAnimationVariable *variable,
            /* [annotation][in] */ 
            __in  INT32 newValue,
            /* [annotation][in] */ 
            __in  INT32 previousValue) = 0;
        
    };



#endif 	/* __IUIAnimationVariableIntegerChangeHandler_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationStoryboardEventHandler_INTERFACE_DEFINED__
#define __IUIAnimationStoryboardEventHandler_INTERFACE_DEFINED__

    
    MIDL_INTERFACE("3D5C9008-EC7C-4364-9F8A-9AF3C58CBAE6")
    IUIAnimationStoryboardEventHandler : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnStoryboardStatusChanged( 
            /* [annotation][in] */ 
            __in  IUIAnimationStoryboard *storyboard,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_STORYBOARD_STATUS newStatus,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_STORYBOARD_STATUS previousStatus) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnStoryboardUpdated( 
            /* [annotation][in] */ 
            __in  IUIAnimationStoryboard *storyboard) = 0;
        
    };


#endif 	/* __IUIAnimationStoryboardEventHandler_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_UIAnimation_0000_0008 */
/* [local] */ 

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0008_0001
    {	UI_ANIMATION_PRIORITY_EFFECT_FAILURE	= 0,
	UI_ANIMATION_PRIORITY_EFFECT_DELAY	= 1
    } 	UI_ANIMATION_PRIORITY_EFFECT;



#ifndef __IUIAnimationPriorityComparison_INTERFACE_DEFINED__
#define __IUIAnimationPriorityComparison_INTERFACE_DEFINED__

/* interface IUIAnimationPriorityComparison */
/* [unique][helpstring][uuid][object][local] */ 


    MIDL_INTERFACE("83FA9B74-5F86-4618-BC6A-A2FAC19B3F44")
    IUIAnimationPriorityComparison : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE HasPriority( 
            /* [annotation][in] */ 
            __in  IUIAnimationStoryboard *scheduledStoryboard,
            /* [annotation][in] */ 
            __in  IUIAnimationStoryboard *newStoryboard,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_PRIORITY_EFFECT priorityEffect) = 0;
        
    };


#endif 	/* __IUIAnimationPriorityComparison_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_UIAnimation_0000_0009 */
/* [local] */ 

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0009_0001
    {	UI_ANIMATION_SLOPE_INCREASING	= 0,
	UI_ANIMATION_SLOPE_DECREASING	= 1
    } 	UI_ANIMATION_SLOPE;


#ifndef __IUIAnimationTransitionLibrary_INTERFACE_DEFINED__
#define __IUIAnimationTransitionLibrary_INTERFACE_DEFINED__

    
    MIDL_INTERFACE("CA5A14B1-D24F-48b8-8FE4-C78169BA954E")
    IUIAnimationTransitionLibrary : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateInstantaneousTransition( 
            /* [annotation][in] */ 
            __in  DOUBLE finalValue,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateConstantTransition( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS duration,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateDiscreteTransition( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS delay,
            /* [annotation][in] */ 
            __in  DOUBLE finalValue,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS hold,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateLinearTransition( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS duration,
            /* [annotation][in] */ 
            __in  DOUBLE finalValue,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateLinearTransitionFromSpeed( 
            /* [annotation][in] */ 
            __in  DOUBLE speed,
            /* [annotation][in] */ 
            __in  DOUBLE finalValue,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateSinusoidalTransitionFromVelocity( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS duration,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS period,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateSinusoidalTransitionFromRange( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS duration,
            /* [annotation][in] */ 
            __in  DOUBLE minimumValue,
            /* [annotation][in] */ 
            __in  DOUBLE maximumValue,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS period,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SLOPE slope,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateAccelerateDecelerateTransition( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS duration,
            /* [annotation][in] */ 
            __in  DOUBLE finalValue,
            /* [annotation][in] */ 
            __in  DOUBLE accelerationRatio,
            /* [annotation][in] */ 
            __in  DOUBLE decelerationRatio,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateReversalTransition( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS duration,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateCubicTransition( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS duration,
            /* [annotation][in] */ 
            __in  DOUBLE finalValue,
            /* [annotation][in] */ 
            __in  DOUBLE finalVelocity,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateSmoothStopTransition( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS maximumDuration,
            /* [annotation][in] */ 
            __in  DOUBLE finalValue,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateParabolicTransitionFromAcceleration( 
            /* [annotation][in] */ 
            __in  DOUBLE finalValue,
            /* [annotation][in] */ 
            __in  DOUBLE finalVelocity,
            /* [annotation][in] */ 
            __in  DOUBLE acceleration,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
    };
    
#endif// __IUIAnimationTransitionLibrary_INTERFACE_DEFINED__

#ifndef __IUIAnimationInterpolator_INTERFACE_DEFINED__
#define __IUIAnimationInterpolator_INTERFACE_DEFINED__



typedef /* [public][public][public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0010_0001
    {	UI_ANIMATION_DEPENDENCY_NONE	= 0,
        UI_ANIMATION_DEPENDENCY_INTERMEDIATE_VALUES	= 0x1,
        UI_ANIMATION_DEPENDENCY_FINAL_VALUE	= 0x2,
        UI_ANIMATION_DEPENDENCY_FINAL_VELOCITY	= 0x4,
        UI_ANIMATION_DEPENDENCY_DURATION	= 0x8
    } 	UI_ANIMATION_DEPENDENCIES;

    
    MIDL_INTERFACE("7815CBBA-DDF7-478c-A46C-7B6C738B7978")
    IUIAnimationInterpolator : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetInitialValueAndVelocity( 
            /* [annotation][in] */ 
            __in  DOUBLE initialValue,
            /* [annotation][in] */ 
            __in  DOUBLE initialVelocity) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetDuration( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS duration) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetDuration( 
            /* [annotation][retval][out] */ 
            __out  UI_ANIMATION_SECONDS *duration) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetFinalValue( 
            /* [annotation][retval][out] */ 
            __out  DOUBLE *value) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE InterpolateValue( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS offset,
            /* [annotation][retval][out] */ 
            __out  DOUBLE *value) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE InterpolateVelocity( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS offset,
            /* [annotation][retval][out] */ 
            __out  DOUBLE *velocity) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetDependencies( 
            /* [annotation][out] */ 
            __out  UI_ANIMATION_DEPENDENCIES *initialValueDependencies,
            /* [annotation][out] */ 
            __out  UI_ANIMATION_DEPENDENCIES *initialVelocityDependencies,
            /* [annotation][out] */ 
            __out  UI_ANIMATION_DEPENDENCIES *durationDependencies) = 0;
        
    };
    

#endif 	/* __IUIAnimationInterpolator_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationTransitionFactory_INTERFACE_DEFINED__
#define __IUIAnimationTransitionFactory_INTERFACE_DEFINED__

    
    MIDL_INTERFACE("FCD91E03-3E3B-45ad-BBB1-6DFC8153743D")
    IUIAnimationTransitionFactory : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE CreateTransition( 
            /* [annotation][in] */ 
            __in  IUIAnimationInterpolator *interpolator,
            /* [annotation][retval][out] */ 
            __deref_out  IUIAnimationTransition **transition) = 0;
        
    };


#endif 	/* __IUIAnimationTransitionFactory_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationTimer_INTERFACE_DEFINED__
#define __IUIAnimationTimer_INTERFACE_DEFINED__

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0012_0001
    {	UI_ANIMATION_IDLE_BEHAVIOR_CONTINUE	= 0,
        UI_ANIMATION_IDLE_BEHAVIOR_DISABLE	= 1
    } 	UI_ANIMATION_IDLE_BEHAVIOR;

    MIDL_INTERFACE("6B0EFAD1-A053-41d6-9085-33A689144665")
    IUIAnimationTimer : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetTimerUpdateHandler( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationTimerUpdateHandler *updateHandler,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_IDLE_BEHAVIOR idleBehavior) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetTimerEventHandler( 
            /* [annotation][unique][in] */ 
            __in_opt  IUIAnimationTimerEventHandler *handler) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Enable( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE Disable( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE IsEnabled( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE GetTime( 
            /* [annotation][out] */ 
            __out  UI_ANIMATION_SECONDS *seconds) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetFrameRateThreshold( 
            /* [annotation][in] */ 
            __in  UINT32 framesPerSecond) = 0;
        
    };
    



#endif 	/* __IUIAnimationTimer_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationTimerUpdateHandler_INTERFACE_DEFINED__
#define __IUIAnimationTimerUpdateHandler_INTERFACE_DEFINED__

   
    MIDL_INTERFACE("195509B7-5D5E-4e3e-B278-EE3759B367AD")
    IUIAnimationTimerUpdateHandler : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnUpdate( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_SECONDS timeNow,
            /* [annotation][retval][out] */ 
            __out  UI_ANIMATION_UPDATE_RESULT *result) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE SetTimerClientEventHandler( 
            /* [annotation][in] */ 
            __in  IUIAnimationTimerClientEventHandler *handler) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE ClearTimerClientEventHandler( void) = 0;
        
    };


#endif 	/* __IUIAnimationTimerUpdateHandler_INTERFACE_DEFINED__ */

#ifndef __IUIAnimationTimerClientEventHandler_INTERFACE_DEFINED__
#define __IUIAnimationTimerClientEventHandler_INTERFACE_DEFINED__

typedef /* [public][public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_UIAnimation_0000_0014_0001
    {	UI_ANIMATION_TIMER_CLIENT_IDLE	= 0,
        UI_ANIMATION_TIMER_CLIENT_BUSY	= 1
    } 	UI_ANIMATION_TIMER_CLIENT_STATUS;

    MIDL_INTERFACE("BEDB4DB6-94FA-4bfb-A47F-EF2D9E408C25")
    IUIAnimationTimerClientEventHandler : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnTimerClientStatusChanged( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_TIMER_CLIENT_STATUS newStatus,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_TIMER_CLIENT_STATUS previousStatus) = 0;
        
    };



#endif 	/* __IUIAnimationTimerClientEventHandler_INTERFACE_DEFINED__ */


#ifndef __IUIAnimationTimerEventHandler_INTERFACE_DEFINED__
#define __IUIAnimationTimerEventHandler_INTERFACE_DEFINED__

    MIDL_INTERFACE("274A7DEA-D771-4095-ABBD-8DF7ABD23CE3")
    IUIAnimationTimerEventHandler : public IUnknown
    {
    public:
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnPreUpdate( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnPostUpdate( void) = 0;
        
        virtual /* [annotation] */ 
        __checkReturn
        HRESULT STDMETHODCALLTYPE OnRenderingTooSlow( 
            /* [annotation][in] */ 
            __in  UINT32 framesPerSecond) = 0;
        
    };


#endif 	/* __IUIAnimationTimerEventHandler_INTERFACE_DEFINED__ */



#ifndef __UIAnimation_LIBRARY_DEFINED__
#define __UIAnimation_LIBRARY_DEFINED__

    class DECLSPEC_UUID("4C1FC63A-695C-47E8-A339-1A194BE3D0B8")
    UIAnimationManager;
    class DECLSPEC_UUID("1D6322AD-AA85-4EF5-A828-86D71067D145")
    UIAnimationTransitionLibrary;
    class DECLSPEC_UUID("8A9B1CDD-FCD7-419c-8B44-42FD17DB1887")
    UIAnimationTransitionFactory;
    class DECLSPEC_UUID("BFCD4A0C-06B6-4384-B768-0DAA792C380E")
    UIAnimationTimer;
    
#endif /* __UIAnimation_LIBRARY_DEFINED__ */

#ifdef __cplusplus
}
#endif

#endif



