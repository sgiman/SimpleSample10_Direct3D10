//***************************************************************************************
// File: SimpleSample10.cpp
//
// Starting point for new Direct3D 10 samples.  For a more basic starting point, 
// use the EmptyProject10 sample instead.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
// Visual Studio 2010
// Microsoft DirectX SDK (June 2010)
//--------------------------------------------------------------------------------------
// Modified by sgiman, 2022, jul
//***************************************************************************************

#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"

//#define DEBUG_VS   // Раскомментируйте эту строку для отладки вершинных шейдеров D3D9.
//#define DEBUG_PS   // Раскомментируйте эту строку для отладки пиксельных шейдеров D3D9. 


//--------------------------------------------------------------------------------------
// Глобальные переменные
//--------------------------------------------------------------------------------------
CModelViewerCamera          g_Camera;               	// камера просмотра модели
CDXUTDialogResourceManager  g_DialogResourceManager; 	// менеджер общих ресурсов диалогов
CD3DSettingsDlg             g_SettingsDlg;          	// диалоговое окно настроек устройства
CDXUTTextHelper*            g_pTxtHelper = NULL;
CDXUTDialog                 g_HUD;                  	// диалог для стандартных элементов управления
CDXUTDialog                 g_SampleUI;             	// диалоговое окно для конкретных элементов управления образца

// Direct3D 9 resources
extern ID3DXFont*           g_pFont9;
extern ID3DXSprite*         g_pSprite9;


// Direct3D 10 resources
ID3DX10Font*                g_pFont10 = NULL;
ID3DX10Sprite*              g_pSprite10 = NULL;
ID3D10Effect*               g_pEffect10 = NULL;
ID3D10InputLayout*          g_pVertexLayout = NULL;
ID3D10EffectMatrixVariable* g_pmWorldViewProj = NULL;
ID3D10EffectMatrixVariable* g_pmWorld = NULL;
ID3D10EffectScalarVariable* g_pfTime = NULL;


//--------------------------------------------------------------------------------------
// UI control IDs
// Идентификаторы элементов управления пользовательского интерфейса
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_TOGGLEWARP          4


//--------------------------------------------------------------------------------------
// Форвардные объявления
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

extern bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                             bool bWindowed, void* pUserContext );
extern HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice,
                                            const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
extern HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                           void* pUserContext );
extern void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime,
                                        void* pUserContext );
extern void CALLBACK OnD3D9LostDevice( void* pUserContext );
extern void CALLBACK OnD3D9DestroyDevice( void* pUserContext );

bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext );
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D10DestroyDevice( void* pUserContext );

void InitApp();
void RenderText();


//--------------------------------------------------------------------------------------
// Точка входа в программу. Инициализирует все и переходит к обработке сообщения
// петля. Время простоя используется для рендеринга сцены.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Включите проверку памяти во время выполнения для отладочных сборок.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT создаст и использует лучшее устройство (либо D3D9, либо D3D10)
    // который доступен в системе в зависимости от того, какие обратные вызовы D3D установлены ниже
    
    // Установить обратные вызовы DXUT
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );

    DXUTSetCallbackD3D10DeviceAcceptable( IsD3D10DeviceAcceptable );
    DXUTSetCallbackD3D10DeviceCreated( OnD3D10CreateDevice );
    DXUTSetCallbackD3D10SwapChainResized( OnD3D10ResizedSwapChain );
    DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
    DXUTSetCallbackD3D10DeviceDestroyed( OnD3D10DestroyDevice );
    DXUTSetCallbackD3D10FrameRender( OnD3D10FrameRender );

    InitApp();
    DXUTInit( true, true, NULL ); // Разобрать командную строку, показать msgbox при ошибке, без дополнительных параметров командной строки
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"SimpleSample" );
    DXUTCreateDevice( true, 640, 480 );
    DXUTMainLoop(); // Войти в цикл рендеринга DXUT

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Инициализировать приложение
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22, VK_F3 );
    g_HUD.AddButton( IDC_TOGGLEWARP, L"Toggle WARP (F4)", 35, iY += 24, 125, 22, VK_F4 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
}


//-------------------------------------------------------------------------------------------
// Визуализация справки и текста статистики. Эта функция использует интерфейс ID3DXFont для
// эффективный рендеринг текста.
//-------------------------------------------------------------------------------------------
void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 5, 5 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );
    g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Отклоняем любые неприемлемые устройства D3D10, возвращая false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Создаем любые ресурсы D3D10, которые не зависят от заднего буфера
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
    HRESULT hr;

    V_RETURN( D3DX10CreateSprite( pd3dDevice, 500, &g_pSprite10 ) );
    V_RETURN( g_DialogResourceManager.OnD3D10CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D10CreateDevice( pd3dDevice ) );
    V_RETURN( D3DX10CreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                L"Arial", &g_pFont10 ) );
    g_pTxtHelper = new CDXUTTextHelper( NULL, NULL, g_pFont10, g_pSprite10, 15 );

    // Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"SimpleSample10.fx" ) );
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Установите флаг D3D10_SHADER_DEBUG, чтобы встроить отладочную информацию в шейдеры.
    // Установка этого флага улучшает процесс отладки шейдеров, но по-прежнему позволяет
    // шейдеры должны быть оптимизированы и работать точно так, как они будут работать в
    // конфигурация выпуска этой программы.
    dwShaderFlags |= D3D10_SHADER_DEBUG;
    #endif
    V_RETURN( D3DX10CreateEffectFromFile( str, NULL, NULL, "fx_4_0", dwShaderFlags, 0, pd3dDevice, NULL,
                                              NULL, &g_pEffect10, NULL, NULL ) );

    // Получить переменные эффектов
    g_pmWorldViewProj = g_pEffect10->GetVariableByName( "g_mWorldViewProjection" )->AsMatrix();
    g_pmWorld = g_pEffect10->GetVariableByName( "g_mWorld" )->AsMatrix();
    g_pfTime = g_pEffect10->GetVariableByName( "g_fTime" )->AsScalar();

    // Настройте параметры обзора камеры
    D3DXVECTOR3 vecEye( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vecAt ( 0.0f, 0.0f, -0.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Создайте любые ресурсы D3D10, зависящие от заднего буфера.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_SettingsDlg.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

    // Настройте параметры проекции камеры
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 1000.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_Camera.SetButtonMasks( MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
    g_SampleUI.SetSize( 170, 300 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Рендеринг сцены с помощью устройства D3D10
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    D3DXMATRIX mWorldViewProjection;
    D3DXMATRIX mWorld;
    D3DXMATRIX mView;
    D3DXMATRIX mProj;

    float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };
    ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
    pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );

    // Очистить трафарет глубины
    ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
    pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

    // Если отображается диалоговое окно настроек, визуализируйте его вместо визуализации сцены приложения.
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    // Получить матрицу проекции и вида из класса камеры
    mWorld = *g_Camera.GetWorldMatrix();
    mProj = *g_Camera.GetProjMatrix();
    mView = *g_Camera.GetViewMatrix();
    mWorldViewProjection = mWorld * mView * mProj;

	// Обновить переменные эффекта. Вместо использования строк было бы
    // более эффективно кэшировать дескриптор параметра, вызывая
    // ID3DXEffect::GetParameterByName
    g_pmWorldViewProj->SetMatrix( ( float* )&mWorldViewProjection );
    g_pmWorld->SetMatrix( ( float* )&mWorld );
    g_pfTime->SetFloat( ( float )fTime );

    // Установить макет вершины
    pd3dDevice->IASetInputLayout( g_pVertexLayout );

    DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
    RenderText();
    g_HUD.OnRender( fElapsedTime );
    g_SampleUI.OnRender( fElapsedTime );
    DXUT_EndPerfEvent();
}


//--------------------------------------------------------------------------------------
// Освобождение ресурсов D3D10, созданных в OnD3D10ResizedSwapChain. 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Освобождение ресурсов D3D10, созданных в OnD3D10CreateDevice.
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10DestroyDevice();
    g_SettingsDlg.OnD3D10DestroyDevice();
    SAFE_RELEASE( g_pFont10 );
    SAFE_RELEASE( g_pEffect10 );
    SAFE_RELEASE( g_pVertexLayout );
    SAFE_RELEASE( g_pSprite10 );
    SAFE_DELETE( g_pTxtHelper );
}


//--------------------------------------------------------------------------------------
// Вызывается непосредственно перед созданием устройства D3D9 или D3D10, 
// что позволяет приложению изменять параметры устройства по мере необходимости.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    if( pDeviceSettings->ver == DXUT_D3D9_DEVICE )
    {
        IDirect3D9* pD3D = DXUTGetD3D9Object();
        D3DCAPS9 Caps;
        pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps );

		// Если устройство не поддерживает HW T&L или 
		// не поддерживает вершинные шейдеры 1.1 в HW затем переключиться на SWVP.
        if( ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
            Caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        {
            pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

	// Для отладки вершинных шейдеров требуется либо REF, либо программная обработка вершин
        // и для отладки пиксельных шейдеров требуется REF.
#ifdef DEBUG_VS
        if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
        {
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
            pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
#endif
#ifdef DEBUG_PS
        pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    }

     // Для первого созданного устройства, если это REF-устройство, опционально отображать диалоговое окно с предупреждением
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
            ( DXUT_D3D10_DEVICE == pDeviceSettings->ver &&
              pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE ) )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }

    return true;
}


//----------------------------------------------------------------------------------------------
// Обработка обновлений сцены. Это вызывается независимо от того, какой D3D API используется.
//----------------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    // Обновите положение камеры на основе ввода пользователя
    g_Camera.FrameMove( fElapsedTime );
}


//--------------------------------------------------------------------------------------
// Обрабатывать сообщения в приложение
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Передавайте сообщения в вызовы диспетчера диалоговых ресурсов, 
    // чтобы состояние графического интерфейса обновлялось правильно.
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Передать сообщения в диалоговое окно настроек, если оно активно
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Дайте диалогам возможность сначала обработать сообщение
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Передайте все оставшиеся сообщения Windows на камеру, чтобы она могла реагировать на ввод пользователя.
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// Обрабатывать нажатия клавиш
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Обрабатывает события графического интерфейса.
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;
        case IDC_TOGGLEWARP:
            DXUTToggleWARP(); break;
        case IDC_CHANGEDEVICE:
            g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
    }
}
