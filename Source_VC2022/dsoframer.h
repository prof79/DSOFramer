/***************************************************************************
 * DSOFRAMER.H
 *
 * Developer Support Office ActiveX Document Framer Control Sample
 *
 *  Copyright ?999-2004; Microsoft Corporation. All rights reserved.
 *  Written by Microsoft Developer Support Office Integration (PSS DSOI)
 *
 *  This code is provided via KB 311765 as a sample. It is not a formal
 *  product and has not been tested with all containers or servers. Use it
 *  for educational purposes only.
 *
 *  You have a royalty-free right to use, modify, reproduce and distribute
 *  this sample application, and/or any modified version, in any way you
 *  find useful, provided that you agree that Microsoft has no warranty,
 *  obligations or liability for the code or information provided herein.
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 *  EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  See the EULA.TXT file included in the KB download for full terms of use
 *  and restrictions. You should consult documentation on MSDN Library for
 *  possible updates or changes to behaviors or interfaces used in this sample.
 *
 ***************************************************************************/

#pragma once

////////////////////////////////////////////////////////////////////
// UNICODE
//
#define UNICODE
#define _UNICODE

////////////////////////////////////////////////////////////////////
// We compile at level 4 and disable some unnecessary warnings...
//
#pragma warning(push, 4) // Compile at level-4 warnings
#pragma warning(disable: 4100) // unreferenced formal parameter (in OLE this is common)
#pragma warning(disable: 4146) // unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4268) // const static/global data initialized with compiler generated default constructor
#pragma warning(disable: 4310) // cast truncates constant value
#pragma warning(disable: 4786) // identifier was truncated in the debug information

////////////////////////////////////////////////////////////////////
// Compile Options For Modified Behavior...
//
#define DSO_MSDAIPP_USE_DAVONLY          // Default to WebDAV protocol for open by HTTP/HTTPS
#define DSO_WORD12_PERSIST_BUG           // Perform workaround for IPersistFile bug in Word 2007

////////////////////////////////////////////////////////////////////
// Needed include files (both standard and custom)
//
#include <windows.h>
#include <ole2.h>
#include <olectl.h>
#include <oleidl.h>
#include <objsafe.h>
#include <strsafe.h>
#include <VersionHelpers.h>

#include "version.h"
#include "utilities.h"
#include "dsofdocobj.h"
#include ".\lib\dsoframerlib.h"
#include ".\res\resource.h"

////////////////////////////////////////////////////////////////////
// Global Variables
//
extern HINSTANCE        v_hModule;
extern CRITICAL_SECTION v_csecThreadSynch;
extern HICON            v_icoOffDocIcon;
extern ULONG            v_cLocks;
extern BOOL             v_fUnicodeAPI;
extern BOOL             v_fWindows2KPlus;

////////////////////////////////////////////////////////////////////
// Custom Errors - we support a very limited set of custom error messages
//
constexpr auto DSO_E_ERR_BASE               = 0x80041100;
constexpr auto DSO_E_UNKNOWN                = 0x80041101;   // "An unknown problem has occurred."
constexpr auto DSO_E_INVALIDPROGID          = 0x80041102;   // "The ProgID/Template could not be found or is not associated with a COM server."
constexpr auto DSO_E_INVALIDSERVER          = 0x80041103;   // "The associated COM server does not support ActiveX Document embedding."
constexpr auto DSO_E_COMMANDNOTSUPPORTED    = 0x80041104;   // "The command is not supported by the document server."
constexpr auto DSO_E_DOCUMENTREADONLY       = 0x80041105;   // "Unable to perform action because document was opened in read-only mode."
constexpr auto DSO_E_REQUIRESMSDAIPP        = 0x80041106;   // "The Microsoft Internet Publishing Provider is not installed, so the URL document cannot be open for write access."
constexpr auto DSO_E_DOCUMENTNOTOPEN        = 0x80041107;   // "No document is open to perform the operation requested."
constexpr auto DSO_E_INMODALSTATE           = 0x80041108;   // "Cannot access document when in modal condition."
constexpr auto DSO_E_NOTBEENSAVED           = 0x80041109;   // "Cannot Save file without a file path."
constexpr auto DSO_E_FRAMEHOOKFAILED        = 0x8004110A;   // "Unable to set frame hook for the parent window."
constexpr auto DSO_E_ERR_MAX                = 0x8004110B;

////////////////////////////////////////////////////////////////////
// Custom OLE Command IDs - we use for special tasks
//
constexpr auto OLECMDID_GETDATAFORMAT   = 0x7001;  // 28673
constexpr auto OLECMDID_SETDATAFORMAT   = 0x7002;  // 28674
constexpr auto OLECMDID_LOCKSERVER      = 0x7003;  // 28675
constexpr auto OLECMDID_RESETFRAMEHOOK  = 0x7009;  // 28681
constexpr auto OLECMDID_NOTIFYACTIVE    = 0x700A;  // 28682

////////////////////////////////////////////////////////////////////
// Custom Window Messages (only apply to CDsoFramerControl window proc)
//
constexpr auto DSO_WM_ASYNCH_OLECOMMAND         = (WM_USER + 300);
constexpr auto DSO_WM_ASYNCH_STATECHANGE        = (WM_USER + 301);

constexpr auto DSO_WM_HOOK_NOTIFY_COMPACTIVE    = (WM_USER + 400);
constexpr auto DSO_WM_HOOK_NOTIFY_APPACTIVATE   = (WM_USER + 401);
constexpr auto DSO_WM_HOOK_NOTIFY_FOCUSCHANGE   = (WM_USER + 402);
constexpr auto DSO_WM_HOOK_NOTIFY_SYNCPAINT     = (WM_USER + 403);
constexpr auto DSO_WM_HOOK_NOTIFY_PALETTECHANGE = (WM_USER + 404);

// State Flags for DSO_WM_ASYNCH_STATECHANGE:
constexpr auto DSO_STATE_MODAL = 1;
constexpr auto DSO_STATE_ACTIVATION = 2;
constexpr auto DSO_STATE_INTERACTIVE = 3;
constexpr auto DSO_STATE_RETURNFROMMODAL = 4;


////////////////////////////////////////////////////////////////////
// Menu Bar Items
//
constexpr auto DSO_MAX_MENUITEMS = 16;
constexpr auto DSO_MAX_MENUNAME_LENGTH = 32;

#ifndef DT_HIDEPREFIX
#define DT_HIDEPREFIX             0x00100000
#define DT_PREFIXONLY             0x00200000
#endif

constexpr auto SYNCPAINT_TIMER_ID = 4;

////////////////////////////////////////////////////////////////////
// DLL Entry Points
//
extern "C" BOOL APIENTRY DllMain(HINSTANCE hDllHandle, DWORD dwReason, LPVOID /*lpReserved*/);

#ifdef DSO_MIN_CRT_STARTUP
extern "C" BOOL APIENTRY _DllMainCRTStartup(HINSTANCE hDllHandle, DWORD dwReason, LPVOID lpReserved);
#endif

STDAPI DllCanUnloadNow(void);
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv);
STDAPI DllRegisterServer();
STDAPI DllUnregisterServer();
STDAPI DllInstall(BOOL bInstall, _In_opt_ LPCWSTR pszCmdLine);
// DllUnregisterServer() utility - TODO: should be in utilities.cpp
static HRESULT RegRecursiveDeleteKey(HKEY hkParent, LPCTSTR pszSubKey);

////////////////////////////////////////////////////////////////////
// Control Class Factory
//
class CDsoFramerClassFactory : public IClassFactory
{
public:
    CDsoFramerClassFactory(): m_cRef(0){}
    ~CDsoFramerClassFactory(void){}

    // IUnknown Implementation
    STDMETHODIMP         QueryInterface(REFIID riid, void ** ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // IClassFactory Implementation
    STDMETHODIMP  CreateInstance(LPUNKNOWN punk, REFIID riid, void** ppv);
    STDMETHODIMP  LockServer(BOOL fLock);

private:
    // Reference count
    ULONG m_cRef;
};

////////////////////////////////////////////////////////////////////
// CDsoFramerControl -- Main Control (OCX) Object
//
//  The CDsoFramerControl control is standard OLE control designed around
//  the OCX94 specification. Because we plan on doing custom integration to
//  act as both OLE object and OLE host, it does not use frameworks like ATL
//  or MFC which would only complicate the nature of the sample.
//
//  The control inherits from its automation interface, but uses nested
//  classes for all OLE interfaces. This is not a requirement but does help
//  to clearly seperate the tasks done by each interface and makes finding
//  ref count problems easier to spot since each interface carries its own
//  counter and will assert (in debug) if interface is over or under released.
//
//  The control is basically a stage for the ActiveDocument embedding, and
//  handles any external (user) commands. The task of actually acting as
//  a DocObject host is done in the site object CDsoDocObject, which this
//  class creates and uses for the embedding.
//
class CDsoFramerControl : public _FramerControl
{
public:

    CDsoFramerControl(LPUNKNOWN punk);

    ~CDsoFramerControl(void);

    // IUnknown Implementation -- Always delgates to outer unknown...
    STDMETHODIMP         QueryInterface(REFIID riid, void ** ppv){return m_pOuterUnknown->QueryInterface(riid, ppv);}
    STDMETHODIMP_(ULONG) AddRef(void){return m_pOuterUnknown->AddRef();}
    STDMETHODIMP_(ULONG) Release(void){return m_pOuterUnknown->Release();}

    // IDispatch Implementation
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo);
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId);
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);

    // _FramerControl Implementation
    STDMETHODIMP Activate();
    STDMETHODIMP get_ActiveDocument(IDispatch** ppdisp);
    STDMETHODIMP CreateNew(BSTR ProgIdOrTemplate);
    STDMETHODIMP Open(VARIANT Document, VARIANT ReadOnly, VARIANT ProgId, VARIANT WebUsername, VARIANT WebPassword);
    STDMETHODIMP Save(VARIANT SaveAsDocument, VARIANT OverwriteExisting, VARIANT WebUsername, VARIANT WebPassword);
    STDMETHODIMP _PrintOutOld(VARIANT PromptToSelectPrinter);
    STDMETHODIMP Close();
    STDMETHODIMP put_Caption(BSTR bstr);
    STDMETHODIMP get_Caption(BSTR* pbstr);
    STDMETHODIMP put_Titlebar(VARIANT_BOOL vbool);
    STDMETHODIMP get_Titlebar(VARIANT_BOOL* pbool);
    STDMETHODIMP put_Toolbars(VARIANT_BOOL vbool);
    STDMETHODIMP get_Toolbars(VARIANT_BOOL* pbool);
    STDMETHODIMP put_ModalState(VARIANT_BOOL vbool);
    STDMETHODIMP get_ModalState(VARIANT_BOOL* pbool);
    STDMETHODIMP ShowDialog(dsoShowDialogType DlgType);
    STDMETHODIMP put_EnableFileCommand(dsoFileCommandType Item, VARIANT_BOOL vbool);
    STDMETHODIMP get_EnableFileCommand(dsoFileCommandType Item, VARIANT_BOOL* pbool);
    STDMETHODIMP put_BorderStyle(dsoBorderStyle style);
    STDMETHODIMP get_BorderStyle(dsoBorderStyle* pstyle);
    STDMETHODIMP put_BorderColor(OLE_COLOR clr);
    STDMETHODIMP get_BorderColor(OLE_COLOR* pclr);
    STDMETHODIMP put_BackColor(OLE_COLOR clr);
    STDMETHODIMP get_BackColor(OLE_COLOR* pclr);
    STDMETHODIMP put_ForeColor(OLE_COLOR clr);
    STDMETHODIMP get_ForeColor(OLE_COLOR* pclr);
    STDMETHODIMP put_TitlebarColor(OLE_COLOR clr);
    STDMETHODIMP get_TitlebarColor(OLE_COLOR* pclr);
    STDMETHODIMP put_TitlebarTextColor(OLE_COLOR clr);
    STDMETHODIMP get_TitlebarTextColor(OLE_COLOR* pclr);
    STDMETHODIMP ExecOleCommand(LONG OLECMDID, VARIANT Options, VARIANT* vInParam, VARIANT* vInOutParam);
    STDMETHODIMP put_Menubar(VARIANT_BOOL vbool);
    STDMETHODIMP get_Menubar(VARIANT_BOOL* pbool);
    STDMETHODIMP put_HostName(BSTR bstr);
    STDMETHODIMP get_HostName(BSTR* pbstr);
    STDMETHODIMP get_DocumentFullName(BSTR* pbstr);
    STDMETHODIMP PrintOut(VARIANT PromptUser, VARIANT PrinterName, VARIANT Copies, VARIANT FromPage, VARIANT ToPage, VARIANT OutputFile);
    STDMETHODIMP PrintPreview();
    STDMETHODIMP PrintPreviewExit();
    STDMETHODIMP get_IsReadOnly(VARIANT_BOOL* pbool);
    STDMETHODIMP get_IsDirty(VARIANT_BOOL* pbool);
    STDMETHODIMP put_LockServer(VARIANT_BOOL vbool);
    STDMETHODIMP get_LockServer(VARIANT_BOOL* pvbool);
    STDMETHODIMP GetDataObjectContent(VARIANT ClipFormatNameOrNumber, VARIANT *pvResults);
    STDMETHODIMP SetDataObjectContent(VARIANT ClipFormatNameOrNumber, VARIANT DataByteArray);
    STDMETHODIMP put_ActivationPolicy(dsoActivationPolicy lPolicy);
    STDMETHODIMP get_ActivationPolicy(dsoActivationPolicy *plPolicy);
    STDMETHODIMP put_FrameHookPolicy(dsoFrameHookPolicy lPolicy);
    STDMETHODIMP get_FrameHookPolicy(dsoFrameHookPolicy *plPolicy);
    STDMETHODIMP put_MenuAccelerators(VARIANT_BOOL vbool);
    STDMETHODIMP get_MenuAccelerators(VARIANT_BOOL* pvbool);
    STDMETHODIMP put_EventsEnabled(VARIANT_BOOL vbool);
    STDMETHODIMP get_EventsEnabled(VARIANT_BOOL* pvbool);
    STDMETHODIMP get_DocumentName(BSTR* pbstr);

    // IInternalUnknown Implementation
    BEGIN_INTERFACE_PART(InternalUnknown, IUnknown)
    END_INTERFACE_PART(InternalUnknown)

    // IPersistStreamInit Implementation
    BEGIN_INTERFACE_PART(PersistStreamInit, IPersistStreamInit)
        STDMETHODIMP GetClassID(CLSID *pClassID);
        STDMETHODIMP IsDirty(void);
        STDMETHODIMP Load(LPSTREAM pStm);
        STDMETHODIMP Save(LPSTREAM pStm, BOOL fClearDirty);
        STDMETHODIMP GetSizeMax(ULARGE_INTEGER* pcbSize);
        STDMETHODIMP InitNew(void);
    END_INTERFACE_PART(PersistStreamInit)

    // IPersistPropertyBag Implementation
    BEGIN_INTERFACE_PART(PersistPropertyBag, IPersistPropertyBag)
        STDMETHODIMP GetClassID(CLSID *pClassID);
        STDMETHODIMP InitNew(void);
        STDMETHODIMP Load(IPropertyBag* pPropBag, IErrorLog* pErrorLog);
        STDMETHODIMP Save(IPropertyBag* pPropBag, BOOL fClearDirty, BOOL fSaveAllProperties);
    END_INTERFACE_PART(PersistPropertyBag)

    // IPersistStorage Implementation
    BEGIN_INTERFACE_PART(PersistStorage, IPersistStorage)
        STDMETHODIMP GetClassID(CLSID *pClassID);
        STDMETHODIMP IsDirty(void);
        STDMETHODIMP InitNew(LPSTORAGE pStg);
        STDMETHODIMP Load(LPSTORAGE pStg);
        STDMETHODIMP Save(LPSTORAGE pStg, BOOL fSameAsLoad);
        STDMETHODIMP SaveCompleted(LPSTORAGE pStg);
        STDMETHODIMP HandsOffStorage(void);
    END_INTERFACE_PART(PersistStorage)

    // IOleObject Implementation
    BEGIN_INTERFACE_PART(OleObject, IOleObject)
        STDMETHODIMP SetClientSite(IOleClientSite *pClientSite);
        STDMETHODIMP GetClientSite(IOleClientSite **ppClientSite);
        STDMETHODIMP SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj);
        STDMETHODIMP Close(DWORD dwSaveOption);
        STDMETHODIMP SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk);
        STDMETHODIMP GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
        STDMETHODIMP InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved);
        STDMETHODIMP GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject);
        STDMETHODIMP DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect);
        STDMETHODIMP EnumVerbs(IEnumOLEVERB **ppEnumOleVerb);
        STDMETHODIMP Update();
        STDMETHODIMP IsUpToDate();
        STDMETHODIMP GetUserClassID(CLSID *pClsid);
        STDMETHODIMP GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType);
        STDMETHODIMP SetExtent(DWORD dwDrawAspect, SIZEL *psizel);
        STDMETHODIMP GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
        STDMETHODIMP Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection);
        STDMETHODIMP Unadvise(DWORD dwConnection);
        STDMETHODIMP EnumAdvise(IEnumSTATDATA **ppenumAdvise);
        STDMETHODIMP GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus);
        STDMETHODIMP SetColorScheme(LOGPALETTE *pLogpal);
    END_INTERFACE_PART(OleObject)

    // IOleControl Implementation
    BEGIN_INTERFACE_PART(OleControl, IOleControl)
        STDMETHODIMP GetControlInfo(CONTROLINFO* pCI);
        STDMETHODIMP OnMnemonic(LPMSG pMsg);
        STDMETHODIMP OnAmbientPropertyChange(DISPID dispID);
        STDMETHODIMP FreezeEvents(BOOL bFreeze);
    END_INTERFACE_PART(OleControl)

    // IOleInplaceObject Implementation
    BEGIN_INTERFACE_PART(OleInplaceObject, IOleInPlaceObject)
        STDMETHODIMP GetWindow(HWND *phwnd);
        STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);
        STDMETHODIMP InPlaceDeactivate();
        STDMETHODIMP UIDeactivate();
        STDMETHODIMP SetObjectRects(LPCRECT lprcPosRect, LPCRECT lprcClipRect);
        STDMETHODIMP ReactivateAndUndo();
    END_INTERFACE_PART(OleInplaceObject)

    // IOleInplaceActiveObject Implementation
    BEGIN_INTERFACE_PART(OleInplaceActiveObject, IOleInPlaceActiveObject)
        STDMETHODIMP GetWindow(HWND *phwnd);
        STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);
        STDMETHODIMP TranslateAccelerator(LPMSG lpmsg);
        STDMETHODIMP OnFrameWindowActivate(BOOL fActivate);
        STDMETHODIMP OnDocWindowActivate(BOOL fActivate);
        STDMETHODIMP ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow);
        STDMETHODIMP EnableModeless(BOOL fEnable);
    END_INTERFACE_PART(OleInplaceActiveObject)

    // IViewObjectEx Implementation
    BEGIN_INTERFACE_PART(ViewObjectEx, IViewObjectEx)
        STDMETHODIMP Draw(DWORD dwDrawAspect, LONG lIndex, void *pvAspect, DVTARGETDEVICE *ptd, HDC hicTargetDevice, HDC hdcDraw, LPCRECTL prcBounds, LPCRECTL prcWBounds, BOOL (__stdcall *pfnContinue)(DWORD dwContinue), DWORD dwContinue);
        STDMETHODIMP GetColorSet(DWORD dwAspect, LONG lindex, void* pvAspect, DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE** ppColorSet);
        STDMETHODIMP Freeze(DWORD dwAspect, LONG lindex, void* pvAspect, DWORD* pdwFreeze);
        STDMETHODIMP Unfreeze(DWORD dwFreeze);
        STDMETHODIMP SetAdvise(DWORD dwAspect, DWORD advf, IAdviseSink* pAdviseSink);
        STDMETHODIMP GetAdvise(DWORD* pdwAspect, DWORD* padvf, IAdviseSink** ppAdviseSink);
        STDMETHODIMP GetExtent(DWORD  dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL psizel);
        STDMETHODIMP GetRect(DWORD dwAspect, LPRECTL pRect);
        STDMETHODIMP GetViewStatus(DWORD* pdwStatus);
        STDMETHODIMP QueryHitPoint(DWORD dwAspect, LPCRECT pRectBounds, POINT ptlLoc, LONG lCloseHint, DWORD *pHitResult);
        STDMETHODIMP QueryHitRect(DWORD dwAspect, LPCRECT pRectBounds, LPCRECT pRectLoc, LONG lCloseHint, DWORD *pHitResult);
        STDMETHODIMP GetNaturalExtent(DWORD dwAspect, LONG lindex, DVTARGETDEVICE *ptd, HDC hicTargetDev, DVEXTENTINFO *pExtentInfo, LPSIZEL pSizel);
    END_INTERFACE_PART(ViewObjectEx)

    // IDataObject Implementation
    BEGIN_INTERFACE_PART(DataObject, IDataObject)
        STDMETHODIMP GetData(FORMATETC *pfmtc,  STGMEDIUM *pstgm);
        STDMETHODIMP GetDataHere(FORMATETC *pfmtc, STGMEDIUM *pstgm);
        STDMETHODIMP QueryGetData(FORMATETC *pfmtc);
        STDMETHODIMP GetCanonicalFormatEtc(FORMATETC * pfmtcIn, FORMATETC * pfmtcOut);
        STDMETHODIMP SetData(FORMATETC *pfmtc, STGMEDIUM *pstgm, BOOL fRelease);
        STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenum);
        STDMETHODIMP DAdvise(FORMATETC *pfmtc, DWORD advf, IAdviseSink *psink, DWORD *pdwConnection);
        STDMETHODIMP DUnadvise(DWORD dwConnection);
        STDMETHODIMP EnumDAdvise(IEnumSTATDATA **ppenum);
    END_INTERFACE_PART(DataObject)

    // IProvideClassInfo Implementation
    BEGIN_INTERFACE_PART(ProvideClassInfo, IProvideClassInfo)
        STDMETHODIMP GetClassInfo(ITypeInfo** ppTI);
    END_INTERFACE_PART(ProvideClassInfo)

    // IConnectionPointContainer Implementation
    BEGIN_INTERFACE_PART(ConnectionPointContainer, IConnectionPointContainer)
        STDMETHODIMP EnumConnectionPoints(IEnumConnectionPoints **ppEnum);
        STDMETHODIMP FindConnectionPoint(REFIID riid, IConnectionPoint **ppCP);
    END_INTERFACE_PART(ConnectionPointContainer)

    // IEnumConnectionPoints Implementation
    BEGIN_INTERFACE_PART(EnumConnectionPoints, IEnumConnectionPoints)
        STDMETHODIMP Next(ULONG cConnections, IConnectionPoint **rgpcn, ULONG *pcFetched);
        STDMETHODIMP Skip(ULONG cConnections);
        STDMETHODIMP Reset(void);
        STDMETHODIMP Clone(IEnumConnectionPoints **ppEnum);
    END_INTERFACE_PART(EnumConnectionPoints)

    // IConnectionPoint Implementation
    BEGIN_INTERFACE_PART(ConnectionPoint, IConnectionPoint)
        STDMETHODIMP GetConnectionInterface(IID *pIID);
        STDMETHODIMP GetConnectionPointContainer(IConnectionPointContainer **ppCPC);
        STDMETHODIMP Advise(IUnknown *pUnk, DWORD *pdwCookie);
        STDMETHODIMP Unadvise(DWORD dwCookie);
        STDMETHODIMP EnumConnections(IEnumConnections **ppEnum);
    END_INTERFACE_PART(ConnectionPoint)

    // IOleCommandTarget  Implementation
    BEGIN_INTERFACE_PART(OleCommandTarget , IOleCommandTarget)
        STDMETHODIMP QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);
        STDMETHODIMP Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANTARG *pvaIn, VARIANTARG *pvaOut);
    END_INTERFACE_PART(OleCommandTarget)

    // ISupportErrorInfo Implementation
    BEGIN_INTERFACE_PART(SupportErrorInfo, ISupportErrorInfo)
        STDMETHODIMP InterfaceSupportsErrorInfo(REFIID riid);
    END_INTERFACE_PART(SupportErrorInfo)

    // IObjectSafety Implementation
    BEGIN_INTERFACE_PART(ObjectSafety, IObjectSafety)
        STDMETHODIMP GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions,DWORD *pdwEnabledOptions);
        STDMETHODIMP SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);
    END_INTERFACE_PART(ObjectSafety)

    // IDsoDocObjectSite Implementation (for DocObject Callbacks to control)
    BEGIN_INTERFACE_PART(DsoDocObjectSite, IDsoDocObjectSite)
        STDMETHODIMP QueryService(REFGUID guidService, REFIID riid, void **ppv);
        STDMETHODIMP GetWindow(HWND* phWnd);
        STDMETHODIMP GetBorder(LPRECT prcBorder);
        STDMETHODIMP GetHostName(LPWSTR *ppwszHostName);
        STDMETHODIMP SysMenuCommand(UINT uiCharCode);
        STDMETHODIMP SetStatusText(LPCOLESTR pszText);
    END_INTERFACE_PART(DsoDocObjectSite)

    STDMETHODIMP           InitializeNewInstance();

    STDMETHODIMP           InPlaceActivate(LONG lVerb);
    STDMETHODIMP           UIActivate(BOOL fForceUIActive);
    STDMETHODIMP_(void)    SetInPlaceVisible(BOOL fShow);
    STDMETHODIMP_(void)    UpdateModalState(BOOL fModeless, BOOL fNotifyIPObject);
    STDMETHODIMP_(void)    UpdateInteractiveState(BOOL fActive);
    STDMETHODIMP_(void)    EnableDropFile(BOOL fEnable);

    STDMETHODIMP_(void)    OnDraw(DWORD dvAspect, HDC hdcDraw, LPRECT prcBounds, LPRECT prcWBounds, HDC hicTargetDev, BOOL fOptimize);
    STDMETHODIMP_(void)    OnDestroyWindow();
    STDMETHODIMP_(void)    OnResize();
    STDMETHODIMP_(void)    OnMouseMove(UINT x, UINT y);
    STDMETHODIMP_(void)    OnButtonDown(UINT x, UINT y);
    STDMETHODIMP_(void)    OnMenuMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    STDMETHODIMP_(void)    OnToolbarAction(DWORD cmd);
    STDMETHODIMP_(void)    OnDropFile(HDROP hdrpFile);
    STDMETHODIMP_(void)    OnTimer(UINT id);

    STDMETHODIMP_(void)    OnForegroundCompChange(BOOL fCompActive);
    STDMETHODIMP_(void)    OnAppActivationChange(BOOL fActive, DWORD dwThreadID);
    STDMETHODIMP_(void)    OnComponentActivationChange(BOOL fActivate);
    STDMETHODIMP_(void)    OnCtrlFocusChange(BOOL fCtlGotFocus, HWND hFocusWnd);
    STDMETHODIMP_(void)    OnUIFocusChange(BOOL fUIActive);

    STDMETHODIMP_(void)    OnPaletteChanged(HWND hwndPalChg);
    STDMETHODIMP_(void)    OnSyncPaint();
    STDMETHODIMP_(void)    OnWindowEnable(BOOL fEnable){TRACE1(_T("CDsoFramerControl::OnWindowEnable(%d)\n"), fEnable);}
    STDMETHODIMP_(BOOL)    OnSysCommandMenu(TCHAR ch);

    STDMETHODIMP_(HMENU)   GetActivePopupMenu();
    STDMETHODIMP_(BOOL)    FAlertUser(HRESULT hr, LPWSTR pwsFileName);
    STDMETHODIMP_(BOOL)    FRunningInDesignMode();
    STDMETHODIMP           DoDialogAction(dsoShowDialogType item);

    STDMETHODIMP_(void)    RaiseActivationEvent(BOOL fActive);

    STDMETHODIMP           ProvideErrorInfo(HRESULT hres);
    STDMETHODIMP           RaiseAutomationEvent(DISPID did, ULONG cargs, VARIANT *pvtargs);

    STDMETHODIMP           SetTempServerLock(BOOL fLock);
    STDMETHODIMP           ResetFrameHook(HWND hwndFrameWindow);

    // Some inline methods are provided for common tasks such as site notification
    // or calculation of draw size based on user selection of tools and border style.

    void __fastcall ViewChanged()
    {
        // Send data change notification.
        if (m_pDataAdviseHolder)
        {
            m_pDataAdviseHolder->SendOnDataChange((IDataObject*)&m_xDataObject, 0, 0);
        }

        // Send the view change notification....
        if (m_pViewAdviseSink)
        {
            m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);

            // If they asked to be advised once, kill the connection
            if (m_fViewAdviseOnlyOnce)
            {
                m_xViewObjectEx.SetAdvise(DVASPECT_CONTENT, 0, nullptr);
            }
        }

        // Ensure a full repaint...
        InvalidateRect(m_hwnd, nullptr, TRUE);
    }

    void __fastcall GetSizeRectAfterBorder(LPRECT lprcx, LPRECT lprc)
    {
        if (lprcx)
        {
            CopyRect(lprc, lprcx);
        }
        else
        {
            SetRect(lprc, 0, 0, m_Size.cx, m_Size.cy);
        }
        
        if (m_fBorderStyle)
        {
            InflateRect(lprc, -(4 - m_fBorderStyle), -(4 - m_fBorderStyle));
        }
    }

    void __fastcall GetSizeRectAfterTitlebar(LPRECT lprcx, LPRECT lprc)
    {
        GetSizeRectAfterBorder(lprcx, lprc);

        if (m_fShowTitlebar)
        {
            lprc->top += 21;
        }
    }

    void __fastcall GetSizeRectForMenuBar(LPRECT lprcx, LPRECT lprc)
    {
        GetSizeRectAfterTitlebar(lprcx, lprc);

        lprc->bottom = lprc->top + 24;
    }

    void __fastcall GetSizeRectForDocument(LPRECT lprcx, LPRECT lprc)
    {
        GetSizeRectAfterTitlebar(lprcx, lprc);

        if (m_fShowMenuBar)
        {
            lprc->top += 24;
        }

        if (lprc->top > lprc->bottom)
        {
            lprc->top = lprc->bottom;
        }
    }

    void __fastcall RedrawCaption()
    {
        RECT rcT;

        if ((m_hwnd) && (m_fShowTitlebar))
        {
            GetClientRect(m_hwnd, &rcT); rcT.bottom = 21;
            InvalidateRect(m_hwnd, &rcT, FALSE);
        }

        if ((m_hwnd) && (m_fShowMenuBar))
        {
            GetSizeRectForMenuBar(nullptr, &rcT);
            InvalidateRect(m_hwnd, &rcT, FALSE);
        }
    }

    BOOL __fastcall FUseFrameHook() { return (m_lHookPolicy != dsoDisableHook); };
    BOOL __fastcall FDelayFrameHookSet() { return (m_lHookPolicy == dsoSetOnFirstOpen); };

    BOOL __fastcall FDrawBitmapOnAppDeactive( ){ return (!(m_lActivationPolicy & dsoKeepUIActiveOnAppDeactive)); }
    BOOL __fastcall FChangeObjActiveOnFocusChange() { return (m_lActivationPolicy & dsoCompDeactivateOnLostFocus); };
    BOOL __fastcall FIPDeactivateOnCompChange() { return (m_lActivationPolicy & dsoIPDeactivateOnCompDeactive); };

    // The control window proceedure is handled through static class method.
    static STDMETHODIMP_(LRESULT) ControlWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Force redaw of all child windows...
    STDMETHODIMP_(BOOL) InvalidateAllChildWindows(HWND hwnd);
    static STDMETHODIMP_(BOOL) InvalidateAllChildWindowsCallback(HWND hwnd, LPARAM lParam);

    // The variables for the control are kept private but accessible to the
    // nested classes for each interface.
private:

    ULONG                   m_cRef = 0UL;                           // Reference count
    IUnknown               *m_pOuterUnknown = nullptr;              // Outer IUnknown (points to m_xInternalUnknown if not agg)
    ITypeInfo              *m_ptiDispType = nullptr;                // ITypeInfo Pointer (IDispatch Impl)
    EXCEPINFO              *m_pDispExcep = nullptr;                 // EXCEPINFO Pointer (IDispatch Impl)

    HWND                    m_hwnd = nullptr;                       // our window
    HWND                    m_hwndParent = nullptr;                 // immediate parent window
    SIZEL                   m_Size{};                               // the size of this control
    RECT                    m_rcLocation{};                         // where we at

    IOleClientSite         *m_pClientSite = nullptr;                // active client site of host containter
    IOleControlSite        *m_pControlSite = nullptr;               // control site
    IOleInPlaceSite        *m_pInPlaceSite = nullptr;               // inplace site
    IOleInPlaceFrame       *m_pInPlaceFrame = nullptr;              // inplace frame
    IOleInPlaceUIWindow    *m_pInPlaceUIWindow = nullptr;           // inplace ui window

    IAdviseSink            *m_pViewAdviseSink = nullptr;            // advise sink for view (only 1 allowed)
    IOleAdviseHolder       *m_pOleAdviseHolder = nullptr;           // OLE advise holder (for oleobject sinks)
    IDataAdviseHolder      *m_pDataAdviseHolder = nullptr;          // OLE data advise holder (for dataobject sink)
    IDispatch              *m_dispEvents = nullptr;                 // event sink (we only support 1 at a time)
    IStorage               *m_pOleStorage = nullptr;                // IStorage for OLE hosts.

    CDsoDocObject          *m_pDocObjFrame = nullptr;               // The Embedding Class
    CDsoDocObject          *m_pServerLock = nullptr;                // Optional Server Lock for out-of-proc DocObject

    OLE_COLOR               m_clrBorderColor = 0;                   // Control Colors
    OLE_COLOR               m_clrBackColor = 0;                     // "
    OLE_COLOR               m_clrForeColor = 0;                     // "
    OLE_COLOR               m_clrTBarColor = 0;                     // "
    OLE_COLOR               m_clrTBarTextColor = 0;                 // "

    BSTR                    m_bstrCustomCaption = nullptr;          // A custom caption (if provided)
    HMENU                   m_hmenuFilePopup = nullptr;             // The File menu popup
    WORD                    m_wFileMenuFlags = (WORD) 0u;            // Bitflags of enabled file menu items.
    WORD                    m_wSelMenuItem = (WORD) - 1u;            // Which item (if any) is selected
    WORD                    m_cMenuItems = (WORD) 0u;                // Count of items on menu bar
    RECT                    m_rgrcMenuItems[DSO_MAX_MENUITEMS]{};   // Menu bar items
    TCHAR                   m_rgchMenuAccel[DSO_MAX_MENUITEMS]{};   // Menu bar accelerators
    LPWSTR                  m_pwszHostName = nullptr;               // Custom name for SetHostNames

    class CDsoFrameHookManager*  m_pHookManager = nullptr;          // Frame Window Hook Manager Class
    LONG                    m_lHookPolicy = 0L;                     // Policy on how to use frame hook for this host.
    LONG                    m_lActivationPolicy = 0L;               // Policy on activation behavior for comp focus
    HBITMAP                 m_hbmDeactive = nullptr;                // Bitmap used for IPDeactiveOnXXX policies
    ULONGLONG               m_uiSyncPaint = 0ULL;                   // Sync paint counter for draw issues with UIDeactivateOnXXX

	unsigned int            m_fDirty = 1;                           // does the control need to be resaved?
	unsigned int            m_fInPlaceActive = 1;                   // are we in place active or not?
	unsigned int            m_fInPlaceVisible = 1;                  // we are in place visible or not?
	unsigned int            m_fUIActive = 1;                        // are we UI active or not.
	unsigned int            m_fHasFocus = 1;                        // do we have current focus.
	unsigned int            m_fViewAdvisePrimeFirst = 1;            // for IViewobject2::setadvise
	unsigned int            m_fViewAdviseOnlyOnce = 1;              // for IViewobject2::setadvise
	unsigned int            m_fUsingWindowRgn = 1;                  // for SetObjectRects and clipping
	unsigned int            m_fFreezeEvents = 1;                    // should events be frozen?
	unsigned int            m_fDesignMode = 1;                      // are we in design mode?
	unsigned int            m_fModeFlagValid = 1;                   // has mode changed since last check?
	unsigned int            m_fBorderStyle = 2;                     // the border style
	unsigned int            m_fShowTitlebar = 1;                    // should we show titlebar?
	unsigned int            m_fShowToolbars = 1;                    // should we show toolbars?
	unsigned int            m_fModalState = 1;                      // are we modal?
	unsigned int            m_fObjectMenu = 1;                      // are we over obj menu item?
	unsigned int            m_fConCntDone = 1;                      // for enum connectpts
	unsigned int            m_fAppActive = 1;                       // is the app active?
	unsigned int            m_fComponentActive = 1;                 // is the component active?
	unsigned int            m_fShowMenuBar = 1;                     // should we show menubar?
	unsigned int            m_fInDocumentLoad = 1;                  // set when loading file
	unsigned int            m_fNoInteractive = 1;                   // set when we don't allow interaction with docobj
	unsigned int            m_fShowMenuPrev = 1;                    // were menus visible before loss of interactivity?
	unsigned int            m_fShowToolsPrev = 1;                   // were toolbars visible before loss of interactivity?
	unsigned int            m_fSyncPaintTimer = 1;                  // is syncpaint timer running?
	unsigned int            m_fInControlActivate = 1;               // is currently in activation call?
	unsigned int            m_fInFocusChange = 1;                   // are we in a focus change?
	unsigned int            m_fActivateOnStatus = 1;                // we need to activate on change of status
	unsigned int            m_fDisableMenuAccel = 1;                // using menu accelerators
	unsigned int            m_fBkgrdPaintTimer = 1;                 // using menu accelerators
};

////////////////////////////////////////////////////////////////////
// CDsoFrameWindowHook -- Frame Window Hook Class
//
//  Used by the control to allow for proper host notification of focus
//  and activation events occurring at top-level window frame. Because
//  this DocObject host is an OCX, we don't own these notifications and
//  have to "steal" them from our parent using a subclass.
//
//  IMPORTANT: Since the parent frame may exist on a separate thread, this
//  class does nothing but the hook. The code to notify the active component
//  is in a separate global class that is shared by all threads.
//
class CDsoFrameWindowHook
{
public:

    CDsoFrameWindowHook()
    {
        ODS(_T("CDsoFrameWindowHook created\n"));

        m_cHookCount = 0;
        m_hwndTopLevelHost = nullptr;
        m_pfnOrigWndProc = nullptr;
        m_fHostUnicodeWindow = FALSE;
    }

    ~CDsoFrameWindowHook()
    {
        ODS(_T("CDsoFrameWindowHook deleted\n"));
    }

    static STDMETHODIMP_(CDsoFrameWindowHook*) AttachToFrameWindow(HWND hwndParent);
    STDMETHODIMP Detach();

    static STDMETHODIMP_(CDsoFrameWindowHook*) GetHookFromWindow(HWND hwnd);
    inline STDMETHODIMP_(void) AddRef(){InterlockedIncrement((LONG*)&m_cHookCount);}

    static STDMETHODIMP_(LRESULT)
        HostWindowProcHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

    DWORD                   m_cHookCount = 0L;
    // Top-level host window (hooked)
    HWND                    m_hwndTopLevelHost = nullptr;
    WNDPROC                 m_pfnOrigWndProc = nullptr;
    BOOL                    m_fHostUnicodeWindow = TRUE;
};

// THE MAX NUMBER OF DSOFRAMER CONTROLS PER PROCESS
#define DSOF_MAX_CONTROLS   10

////////////////////////////////////////////////////////////////////
// CDsoFrameHookManager -- Hook Manager Class
//
//  Used to keep track of which control is active and forward notifications
//  to it using window messages (to cross thread boundaries).
//
class CDsoFrameHookManager
{
public:

    CDsoFrameHookManager()
    {
        ODS(_T("CDsoFrameHookManager created\n"));

        m_fAppActive = TRUE;
        m_idxActive = DSOF_MAX_CONTROLS;
        m_cComponents = 0;
    }

    ~CDsoFrameHookManager()
    {
        ODS(_T("CDsoFrameHookManager deleted\n"));
    }

    static STDMETHODIMP_(CDsoFrameHookManager*)
        RegisterFramerControl(HWND hwndParent, HWND hwndControl);

    STDMETHODIMP AddComponent(HWND hwndParent, HWND hwndControl);
    STDMETHODIMP DetachComponent(HWND hwndControl);
    STDMETHODIMP SetActiveComponent(HWND hwndControl);
    STDMETHODIMP OnComponentNotify(DWORD msg, WPARAM wParam, LPARAM lParam);

    inline STDMETHODIMP_(HWND)
        GetActiveComponentWindow(){return m_pComponents[m_idxActive].hwndControl;}

    inline STDMETHODIMP_(CDsoFrameWindowHook*)
        GetActiveComponentFrame(){return m_pComponents[m_idxActive].phookFrame;}

    STDMETHODIMP_(BOOL) SendNotifyMessage(HWND hwnd, DWORD msg, WPARAM wParam, LPARAM lParam);

protected:

    struct FHOOK_COMPONENTS
    {
        HWND hwndControl = nullptr;
        CDsoFrameWindowHook *phookFrame = nullptr;
    };

    BOOL                    m_fAppActive = FALSE;
    DWORD                   m_idxActive = 0L;
    DWORD                   m_cComponents = 0L;
    FHOOK_COMPONENTS        m_pComponents[DSOF_MAX_CONTROLS]{};
};
