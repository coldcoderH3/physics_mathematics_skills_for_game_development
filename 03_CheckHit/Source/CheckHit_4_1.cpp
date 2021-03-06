﻿//------------------------------------------------------------
// CheckHit_4_1.cpp
// 圆形与扇形的碰撞检测
// 
//------------------------------------------------------------

#include <windows.h>
#include <math.h>

#define PI					3.14159265f			// 圆周率
#define VIEW_WIDTH			640					// 画面宽度
#define VIEW_HEIGHT			480					// 画面高度
#define CIRCLE_VEL			5.0f				// 圆形速度


struct F_CIRCLE {
	float			x, y;				// 中心位置
	float			r;					// 半径
};

struct F_FAN {
	float			x, y;				// 中心位置
	float			vx1, vy1;			// 向量1
	float			vx2, vy2;			// 向量2
	float			fAngle1, fAngle2;	// 各向量的角度
	float			r;					// 半径
};


F_CIRCLE			crCircleA;			// 圆形Ａ
F_FAN				faFanB;				// 扇形Ｂ


int InitShapes( void )						// 只在程序开始时调用一次
{
	crCircleA.x  = 100.0f;  crCircleA.y = 100.0f;
	crCircleA.r = 70.0f;
	faFanB.x  = 200.0f;  faFanB.y  = 200.0f;
	faFanB.fAngle1 = -PI / 6.0f;
	faFanB.fAngle2 = PI / 3.0f;
	faFanB.r = 200.0f;
	faFanB.vx1 = faFanB.r * cosf( faFanB.fAngle1 );  faFanB.vy1 = faFanB.r * sinf( faFanB.fAngle1 );
	faFanB.vx2 = faFanB.r * cosf( faFanB.fAngle2 );  faFanB.vy2 = faFanB.r * sinf( faFanB.fAngle2 );

	return 0;
}


int CheckHit( F_CIRCLE *pcrCircle, F_FAN *pfaFan )		// 碰撞检测
{
	int				nResult = false;

	float			dx, dy;							// 位置坐标之差
	float			fAlpha, fBeta;
	float			fDelta;
	float			ar;								// 两圆半径之和
	float			fDistSqr;
	float			a, b, c;
	float			d;
	float			t;

	dx = pcrCircle->x - pfaFan->x;  dy = pcrCircle->y - pfaFan->y;
	fDistSqr = dx * dx + dy * dy;
	if ( fDistSqr < pcrCircle->r * pcrCircle->r ) {
		nResult = true;
	}
	else {
		fDelta = pfaFan->vx1 * pfaFan->vy2 - pfaFan->vx2 * pfaFan->vy1;
		fAlpha = (  dx * pfaFan->vy2 - dy * pfaFan->vx2 ) / fDelta;
		fBeta  = ( -dx * pfaFan->vy1 + dy * pfaFan->vx1 ) / fDelta;
		if ( ( fAlpha >= 0.0f ) && ( fBeta >= 0.0f ) ) {
			ar = pfaFan->r + pcrCircle->r;
			if ( fDistSqr <= ar * ar ) {
				nResult = true;
			}
		}
		else {
			a = pfaFan->vx1 * pfaFan->vx1 + pfaFan->vy1 * pfaFan->vy1;
			b = -( pfaFan->vx1 * dx + pfaFan->vy1 * dy );
			c = dx * dx + dy * dy - pcrCircle->r * pcrCircle->r;
			d = b * b - a * c;
			if ( d >= 0.0f ) {
				t = ( -b - sqrtf( d ) ) / a;
				if ( ( t >= 0.0f )  && ( t <= 1.0f ) ) {
					nResult = true;
				}
			}
			a = pfaFan->vx2 * pfaFan->vx2 + pfaFan->vy2 * pfaFan->vy2;
			b = -( pfaFan->vx2 * dx + pfaFan->vy2 * dy );
			c = dx * dx + dy * dy - pcrCircle->r * pcrCircle->r;
			d = b * b - a * c;
			if ( d >= 0.0f ) {
				t = ( -b - sqrtf( d ) ) / a;
				if ( ( t >= 0.0f )  && ( t <= 1.0f ) ) {
					nResult = true;
				}
			}
		}
	}

	return nResult;
}


int MoveRect( void )						// 通过键盘输入移动矩形A
{
//	float			fVelocity;

	// 左方向键被按下时向左移动
	if ( GetAsyncKeyState( VK_LEFT ) ) {
		crCircleA.x -= CIRCLE_VEL;
		if ( crCircleA.x < 0.0f ) crCircleA.x = 0.0f;
	}
	// 右方向键被按下时向右移动
	if ( GetAsyncKeyState( VK_RIGHT ) ) {
		crCircleA.x += CIRCLE_VEL;
		if ( crCircleA.x > VIEW_WIDTH ) crCircleA.x = VIEW_WIDTH;
	}
	// 上方向键被按下时向上移动
	if ( GetAsyncKeyState( VK_UP ) ) {
		crCircleA.y -= CIRCLE_VEL;
		if ( crCircleA.y < 0.0f ) crCircleA.y = 0.0f;
	}
	// 下方向键被按下时向下移动
	if ( GetAsyncKeyState( VK_DOWN ) ) {
		crCircleA.y += CIRCLE_VEL;
		if ( crCircleA.y > VIEW_HEIGHT ) crCircleA.y = VIEW_HEIGHT;
	}

	return 0;
}


//------------------------------------------------------------
// 下面通过DirectX显示

#include <stdio.h>
#include <windows.h>
#include <tchar.h>								// 处理Unicode及宽字符文本

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>


#define MAX_BUFFER_VERTEX				10000	// 最大缓冲顶点数


// 链接库
#pragma comment( lib, "d3d11.lib" )   // D3D11库文件
#pragma comment( lib, "d3dx11.lib" )


// 安全释放宏
#ifndef SAFE_RELEASE
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release(); ( p )=NULL; } }
#endif


// 顶点结构体
struct CUSTOMVERTEX {
    XMFLOAT4	v4Pos;
    XMFLOAT4	v4Color;
	XMFLOAT2	v2UV;
};

// 着色器常数结构体
struct CBNeverChanges
{
    XMMATRIX mView;
};

// 图片纹理结构体
struct TEX_PICTURE {
	ID3D11ShaderResourceView	*pSRViewTexture;
	D3D11_TEXTURE2D_DESC		tdDesc;
	int							nWidth, nHeight;
};


// 全局变量
UINT  g_nClientWidth;							// 渲染区域的宽度
UINT  g_nClientHeight;							// 渲染区域的高度

HWND        g_hWnd;         // 窗口宽度


ID3D11Device			*g_pd3dDevice;			// 适配器
IDXGISwapChain			*g_pSwapChain;			// 渲染数据暂存
ID3D11DeviceContext		*g_pImmediateContext;	// 适配器上下文
ID3D11RasterizerState	*g_pRS;					// 渲染状态
ID3D11RenderTargetView	*g_pRTV;				// 渲染目标
D3D_FEATURE_LEVEL       g_FeatureLevel;			// 适配器的功能级别

ID3D11Buffer			*g_pD3D11VertexBuffer;
ID3D11BlendState		*g_pbsAlphaBlend;
ID3D11VertexShader		*g_pVertexShader;
ID3D11PixelShader		*g_pPixelShader;
ID3D11InputLayout		*g_pInputLayout;
ID3D11SamplerState		*g_pSamplerState;

ID3D11Buffer			*g_pCBNeverChanges = NULL;

TEX_PICTURE				g_tRect_A, g_tRect_B;
TEX_PICTURE				g_tBack;

// 渲染顶点缓冲
CUSTOMVERTEX g_cvVertices[MAX_BUFFER_VERTEX];
int							g_nVertexNum = 0;
ID3D11ShaderResourceView	*g_pNowTexture = NULL;


// Direct3D初始化
HRESULT InitD3D( void )
{
    HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL  FeatureLevelsRequested[6] = { D3D_FEATURE_LEVEL_11_0,
													 D3D_FEATURE_LEVEL_10_1,
													 D3D_FEATURE_LEVEL_10_0,
													 D3D_FEATURE_LEVEL_9_3,
													 D3D_FEATURE_LEVEL_9_2,
													 D3D_FEATURE_LEVEL_9_1 };
	UINT               numLevelsRequested = 6;
	D3D_FEATURE_LEVEL  FeatureLevelsSupported;

	// 创建适配器
	hr = D3D11CreateDevice( NULL,
					D3D_DRIVER_TYPE_HARDWARE, 
					NULL, 
					0,
					FeatureLevelsRequested, 
					numLevelsRequested,
					D3D11_SDK_VERSION, 
					&g_pd3dDevice,
					&FeatureLevelsSupported,
					&g_pImmediateContext );
	if( FAILED ( hr ) ) {
		return hr;
	}

	// 获得工厂
	IDXGIDevice * pDXGIDevice;
	hr = g_pd3dDevice->QueryInterface( __uuidof( IDXGIDevice ), ( void ** )&pDXGIDevice );
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent( __uuidof( IDXGIAdapter ), ( void ** )&pDXGIAdapter );
	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent( __uuidof( IDXGIFactory ), ( void ** )&pIDXGIFactory);

	// 创建渲染数据
    DXGI_SWAP_CHAIN_DESC	sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = g_nClientWidth;
	sd.BufferDesc.Height = g_nClientHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	hr = pIDXGIFactory->CreateSwapChain( g_pd3dDevice, &sd, &g_pSwapChain );

	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pIDXGIFactory->Release();

	if( FAILED ( hr ) ) {
		return hr;
	}

    // 生成渲染目标
    ID3D11Texture2D			*pBackBuffer = NULL;
    D3D11_TEXTURE2D_DESC BackBufferSurfaceDesc;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't get backbuffer." ), _T( "Error" ), MB_OK );
        return hr;
    }
    pBackBuffer->GetDesc( &BackBufferSurfaceDesc );
    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRTV );
    SAFE_RELEASE( pBackBuffer );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't create render target view." ), _T( "Error" ), MB_OK );
        return hr;
    }

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, NULL );

    // 设置渲染状态
    D3D11_RASTERIZER_DESC drd;
	ZeroMemory( &drd, sizeof( drd ) );
	drd.FillMode				= D3D11_FILL_SOLID;
	drd.CullMode				= D3D11_CULL_NONE;
	drd.FrontCounterClockwise	= FALSE;
	drd.DepthClipEnable			= TRUE;
    hr = g_pd3dDevice->CreateRasterizerState( &drd, &g_pRS );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't create rasterizer state." ), _T( "Error" ), MB_OK );
        return hr;
    }
    g_pImmediateContext->RSSetState( g_pRS );

    // 设置VIEWPORT
    D3D11_VIEWPORT vp;
    vp.Width    = ( FLOAT )g_nClientWidth;
    vp.Height   = ( FLOAT )g_nClientHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    return S_OK;
}


// 创建可编程着色器
HRESULT MakeShaders( void )
{
    HRESULT hr;
    ID3DBlob* pVertexShaderBuffer = NULL;
    ID3DBlob* pPixelShaderBuffer = NULL;
    ID3DBlob* pError = NULL;

    DWORD dwShaderFlags = 0;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
    // 编译
    hr = D3DX11CompileFromFile( _T( "Basic_2D.fx" ), NULL, NULL, "VS", "vs_4_0_level_9_1",
								dwShaderFlags, 0, NULL, &pVertexShaderBuffer, &pError, NULL );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't open Basic_2D.fx" ), _T( "Error" ), MB_OK );
        SAFE_RELEASE( pError );
        return hr;
    }
    hr = D3DX11CompileFromFile( _T( "Basic_2D.fx" ), NULL, NULL, "PS", "ps_4_0_level_9_1",
								dwShaderFlags, 0, NULL, &pPixelShaderBuffer, &pError, NULL );
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pError );
        return hr;
    }
    SAFE_RELEASE( pError );
    
    // 创建顶点着色器
    hr = g_pd3dDevice->CreateVertexShader( pVertexShaderBuffer->GetBufferPointer(),
										   pVertexShaderBuffer->GetBufferSize(),
										   NULL, &g_pVertexShader );
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pPixelShaderBuffer );
        return hr;
    }
    // 创建像素着色器
    hr = g_pd3dDevice->CreatePixelShader( pPixelShaderBuffer->GetBufferPointer(),
										  pPixelShaderBuffer->GetBufferSize(),
										  NULL, &g_pPixelShader );
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pPixelShaderBuffer );
        return hr;
    }

    // 输入缓冲的输入形式
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	UINT numElements = ARRAYSIZE( layout );
	// 创建输入布局
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements,
										  pVertexShaderBuffer->GetBufferPointer(),
										  pVertexShaderBuffer->GetBufferSize(),
										  &g_pInputLayout );
    SAFE_RELEASE( pVertexShaderBuffer );
    SAFE_RELEASE( pPixelShaderBuffer );
    if ( FAILED( hr ) ) {
        return hr;
    }

    // 创建着色器常数缓冲
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof( bd ) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( CBNeverChanges );
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBNeverChanges );
    if( FAILED( hr ) )
        return hr;

	// 变换矩阵
    CBNeverChanges	cbNeverChanges;
	XMMATRIX		mScreen;
    mScreen = XMMatrixIdentity();
	mScreen._11 =  2.0f / g_nClientWidth;
	mScreen._22 = -2.0f / g_nClientHeight;
	mScreen._41 = -1.0f;
	mScreen._42 =  1.0f;
	cbNeverChanges.mView = XMMatrixTranspose( mScreen );
	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

    return S_OK;
}


// 载入纹理
int LoadTexture( TCHAR *szFileName, TEX_PICTURE *pTexPic, int nWidth, int nHeight,
				 int nTexWidth, int nTexHeight )
{
    HRESULT						hr;
	D3DX11_IMAGE_LOAD_INFO		liLoadInfo;
	ID3D11Texture2D				*pTexture;

	ZeroMemory( &liLoadInfo, sizeof( D3DX11_IMAGE_LOAD_INFO ) );
	liLoadInfo.Width = nTexWidth;
	liLoadInfo.Height = nTexHeight;
	liLoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	liLoadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	hr = D3DX11CreateShaderResourceViewFromFile( g_pd3dDevice, szFileName, &liLoadInfo,
												 NULL, &( pTexPic->pSRViewTexture ), NULL );
    if ( FAILED( hr ) ) {
        return hr;
    }
	pTexPic->pSRViewTexture->GetResource( ( ID3D11Resource ** )&( pTexture ) );
	pTexture->GetDesc( &( pTexPic->tdDesc ) );
	pTexture->Release();

	pTexPic->nWidth = nWidth;
	pTexPic->nHeight = nHeight;

	return S_OK;
}


// 渲染模式对象初始化
int InitDrawModes( void )
{
    HRESULT				hr;

	// 混合状态
    D3D11_BLEND_DESC BlendDesc;
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
    BlendDesc.RenderTarget[0].BlendEnable           = TRUE;
    BlendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    BlendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    BlendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = g_pd3dDevice->CreateBlendState( &BlendDesc, &g_pbsAlphaBlend );
    if ( FAILED( hr ) ) {
        return hr;
    }

    // 采样器
    D3D11_SAMPLER_DESC samDesc;
    ZeroMemory( &samDesc, sizeof( samDesc ) );
    samDesc.Filter          = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressV        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressW        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.ComparisonFunc  = D3D11_COMPARISON_ALWAYS;
    samDesc.MaxLOD          = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &samDesc, &g_pSamplerState );
    if ( FAILED( hr ) ) {
        return hr;
    }

    return S_OK;
}


// 图形初始化
HRESULT InitGeometry( void )
{
    HRESULT hr = S_OK;

    // 顶点缓冲作成
    D3D11_BUFFER_DESC BufferDesc;
    BufferDesc.Usage                = D3D11_USAGE_DYNAMIC;
    BufferDesc.ByteWidth            = sizeof( CUSTOMVERTEX ) * MAX_BUFFER_VERTEX;
    BufferDesc.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags       = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags            = 0;

    D3D11_SUBRESOURCE_DATA SubResourceData;
    SubResourceData.pSysMem             = g_cvVertices;
    SubResourceData.SysMemPitch         = 0;
    SubResourceData.SysMemSlicePitch    = 0;
    hr = g_pd3dDevice->CreateBuffer( &BufferDesc, &SubResourceData, &g_pD3D11VertexBuffer );
    if ( FAILED( hr ) ) {
        return hr;
    }

	// 创建纹理
	g_tRect_A.pSRViewTexture =  NULL;
	g_tRect_B.pSRViewTexture =  NULL;
	hr = LoadTexture( _T( "13_A.bmp" ), &g_tRect_A, 256, 256, 256, 256 );
    if ( FAILED( hr ) ) {
 		MessageBox( NULL, _T( "Can't open 13_A.bmp" ), _T( "Error" ), MB_OK );
       return hr;
    }
	hr = LoadTexture( _T( "13_B.bmp" ), &g_tRect_B, 256, 256, 256, 256 );
    if ( FAILED( hr ) ) {
 		MessageBox( NULL, _T( "Can't open 13_B.bmp" ), _T( "Error" ), MB_OK );
        return hr;
    }
	hr = LoadTexture( _T( "12.bmp" ), &g_tBack, 640, 480, 1024, 512 );
	if ( FAILED( hr ) ) {
 		MessageBox( NULL, _T( "Can't open 12.BMP" ), _T( "Error" ), MB_OK );
        return hr;
    }

	return S_OK;
}


// 终止处理
int Cleanup( void )
{
    SAFE_RELEASE( g_tRect_A.pSRViewTexture );
    SAFE_RELEASE( g_tRect_B.pSRViewTexture );
    SAFE_RELEASE( g_tBack.pSRViewTexture );
    SAFE_RELEASE( g_pD3D11VertexBuffer );

    SAFE_RELEASE( g_pSamplerState );
    SAFE_RELEASE( g_pbsAlphaBlend );
    SAFE_RELEASE( g_pInputLayout );
    SAFE_RELEASE( g_pPixelShader );
    SAFE_RELEASE( g_pVertexShader );
    SAFE_RELEASE( g_pCBNeverChanges );

    SAFE_RELEASE( g_pRS );									// 渲染状态

	// 清除状态
	if ( g_pImmediateContext ) {
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Flush();
	}

    SAFE_RELEASE( g_pRTV );									// 渲染目标

    // 渲染数据
    if ( g_pSwapChain != NULL ) {
        g_pSwapChain->SetFullscreenState( FALSE, 0 );
    }
    SAFE_RELEASE( g_pSwapChain );

    SAFE_RELEASE( g_pImmediateContext );					// 适配器上下文
    SAFE_RELEASE( g_pd3dDevice );							// 适配器

	return 0;
}


// 窗口处理
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


// 顺序输出等待渲染的图形矩阵
int FlushDrawingPictures( void )
{
	HRESULT			hr;

	if ( ( g_nVertexNum > 0 ) && g_pNowTexture ) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = g_pImmediateContext->Map( g_pD3D11VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		if ( SUCCEEDED( hr ) ) {
			CopyMemory( mappedResource.pData, &( g_cvVertices[0] ), sizeof( CUSTOMVERTEX ) * g_nVertexNum );
			g_pImmediateContext->Unmap( g_pD3D11VertexBuffer, 0 );
		}
		g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pNowTexture );
		g_pImmediateContext->Draw( g_nVertexNum, 0 );
	}
	g_nVertexNum = 0;
	g_pNowTexture = NULL;

	return 0;
}


// 图形的渲染
int DrawPicture( float x, float y, TEX_PICTURE *pTexPic )
{
	if ( g_nVertexNum > ( MAX_BUFFER_VERTEX - 6 ) ) return -1;	// 顶点从缓冲溢出时不渲染

	// 纹理切换时输出图形矩阵
	if ( ( pTexPic->pSRViewTexture != g_pNowTexture ) && g_pNowTexture ) {
		FlushDrawingPictures();
	}

	// 顶点设置
	g_cvVertices[g_nVertexNum + 0].v4Pos   = XMFLOAT4( x,                         y,                          0.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 0].v4Color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 0].v2UV    = XMFLOAT2( 0.0f, 0.0f );
	g_cvVertices[g_nVertexNum + 1].v4Pos   = XMFLOAT4( x,                         y + pTexPic->nHeight, 0.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 1].v4Color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 1].v2UV    = XMFLOAT2( 0.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 2].v4Pos   = XMFLOAT4( x + pTexPic->nWidth, y,                          0.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 2].v4Color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 2].v2UV    = XMFLOAT2( 1.0f, 0.0f );
	g_cvVertices[g_nVertexNum + 3] = g_cvVertices[g_nVertexNum + 1];
	g_cvVertices[g_nVertexNum + 4].v4Pos   = XMFLOAT4( x + pTexPic->nWidth, y + pTexPic->nHeight, 0.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 4].v4Color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 4].v2UV    = XMFLOAT2( 1.0f, 1.0f );
	g_cvVertices[g_nVertexNum + 5] = g_cvVertices[g_nVertexNum + 2];
	g_nVertexNum += 6;
	g_pNowTexture = pTexPic->pSRViewTexture;

	return 0;
}


// 渲染圆形图形（带颜色）
int DrawCircleWithColor( float x, float y, float r, TEX_PICTURE *pTexPic, int nColor )
{
	int				i;
	int				nDivide_num;
	float			fAngle1, fAngle2;
	float			fAngleDelta;
	float			fRed, fGreen, fBlue, fAlpha;

	if ( g_nVertexNum > ( MAX_BUFFER_VERTEX - 6 ) ) return -1;	// 顶点从缓冲溢出时不渲染

	// 纹理切换时输出图形矩阵
	if ( ( pTexPic->pSRViewTexture != g_pNowTexture ) && g_pNowTexture ) {
		FlushDrawingPictures();
	}

	// 颜色抽出
	fRed   = ( float )( ( nColor >> 16 ) & 0xff ) / 255.0f;
	fGreen = ( float )( ( nColor >>  8 ) & 0xff ) / 255.0f;
	fBlue  = ( float )(   nColor         & 0xff ) / 255.0f;
	fAlpha = ( float )( ( nColor >> 24 ) & 0xff ) / 255.0f;

	nDivide_num = ( int )( 2.0f * PI * r / 5.0f ) + 1;
	fAngleDelta = 2.0f * PI / nDivide_num;
	fAngle1 = 0.0f;  fAngle2 = fAngleDelta;
	for ( i = 0; i < nDivide_num; i++ ) {
		// 顶点设置
		g_cvVertices[g_nVertexNum + 0].v4Pos   = XMFLOAT4( x + r * cosf( fAngle1 ), y + r * sinf( fAngle1 ), 0.0f, 1.0f );
		g_cvVertices[g_nVertexNum + 0].v4Color = XMFLOAT4( fRed, fGreen, fBlue, fAlpha );
		g_cvVertices[g_nVertexNum + 0].v2UV    = XMFLOAT2( 0.5f + 0.5f * cosf( fAngle1 ), 0.5f + 0.5f * sinf( fAngle1 ) );
		g_cvVertices[g_nVertexNum + 1].v4Pos   = XMFLOAT4( x, y, 0.0f, 1.0f );
		g_cvVertices[g_nVertexNum + 1].v4Color = XMFLOAT4( fRed, fGreen, fBlue, fAlpha );
		g_cvVertices[g_nVertexNum + 1].v2UV    = XMFLOAT2( 0.5f, 0.5f );
		g_cvVertices[g_nVertexNum + 2].v4Pos   = XMFLOAT4( x + r * cosf( fAngle2 ), y + r * sinf( fAngle2 ), 0.0f, 1.0f );
		g_cvVertices[g_nVertexNum + 2].v4Color = XMFLOAT4( fRed, fGreen, fBlue, fAlpha );
		g_cvVertices[g_nVertexNum + 2].v2UV    = XMFLOAT2( 0.5f + 0.5f * cosf( fAngle2 ), 0.5f + 0.5f * sinf( fAngle2 ) );
		g_nVertexNum += 3;
		fAngle1 += fAngleDelta;
		fAngle2 += fAngleDelta;
	}
	g_pNowTexture = pTexPic->pSRViewTexture;

	return 0;
}


// 絵の扇形描画(色付き)
int DrawFanWithColor( float x, float y, float r, float fStartfAngle, float fEndAngle, TEX_PICTURE *pTexPic, int nColor )
{
	int				i;
	int				nDivide_num;
	float			fAngle1, fAngle2;
	float			fAngleDelta;
	float			fRed, fGreen, fBlue, fAlpha;

	if ( g_nVertexNum > ( MAX_BUFFER_VERTEX - 6 ) ) return -1;	// 顶点从缓冲溢出时不渲染

	// 纹理切换时输出图形矩阵
	if ( ( pTexPic->pSRViewTexture != g_pNowTexture ) && g_pNowTexture ) {
		FlushDrawingPictures();
	}

	// 颜色抽出
	fRed   = ( float )( ( nColor >> 16 ) & 0xff ) / 255.0f;
	fGreen = ( float )( ( nColor >>  8 ) & 0xff ) / 255.0f;
	fBlue  = ( float )(   nColor         & 0xff ) / 255.0f;
	fAlpha = ( float )( ( nColor >> 24 ) & 0xff ) / 255.0f;

	nDivide_num = ( int )( ( fEndAngle - fStartfAngle ) * r / 5.0f ) + 1;
	fAngleDelta = ( fEndAngle - fStartfAngle ) / nDivide_num;
	fAngle1 = fStartfAngle;
	fAngle2 = fStartfAngle + fAngleDelta;
	for ( i = 0; i < nDivide_num; i++ ) {
		// 顶点设置
		g_cvVertices[g_nVertexNum + 0].v4Pos   = XMFLOAT4( x + r * cosf( fAngle1 ), y + r * sinf( fAngle1 ), 0.0f, 1.0f );
		g_cvVertices[g_nVertexNum + 0].v4Color = XMFLOAT4( fRed, fGreen, fBlue, fAlpha );
		g_cvVertices[g_nVertexNum + 0].v2UV    = XMFLOAT2( 0.0f, 0.0f );//0.5f + 0.5f * cosf( fAngle1 ), 0.5f + 0.5f * sinf( fAngle1 ) );
		g_cvVertices[g_nVertexNum + 1].v4Pos   = XMFLOAT4( x, y, 0.0f, 1.0f );
		g_cvVertices[g_nVertexNum + 1].v4Color = XMFLOAT4( fRed, fGreen, fBlue, fAlpha );
		g_cvVertices[g_nVertexNum + 1].v2UV    = XMFLOAT2( 0.0f, 0.0f );
		g_cvVertices[g_nVertexNum + 2].v4Pos   = XMFLOAT4( x + r * cosf( fAngle2 ), y + r * sinf( fAngle2 ), 0.0f, 1.0f );
		g_cvVertices[g_nVertexNum + 2].v4Color = XMFLOAT4( fRed, fGreen, fBlue, fAlpha );
		g_cvVertices[g_nVertexNum + 2].v2UV    = XMFLOAT2( 0.0f, 0.0f );
		g_nVertexNum += 3;
		fAngle1 += fAngleDelta;
		fAngle2 += fAngleDelta;
	}
	g_pNowTexture = pTexPic->pSRViewTexture;

	return 0;
}



// 显示到画面
HRESULT Render( void )
{
    // 清除画面
	XMFLOAT4	v4Color = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
    g_pImmediateContext->ClearRenderTargetView( g_pRTV, ( float * )&v4Color );

    // 采样器初始化
    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerState );
    g_pImmediateContext->RSSetState( g_pRS );
    
    // 渲染设置
    UINT nStrides = sizeof( CUSTOMVERTEX );
    UINT nOffsets = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pD3D11VertexBuffer, &nStrides, &nOffsets );
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    g_pImmediateContext->IASetInputLayout( g_pInputLayout );

    // 着色器设置
    g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

    // 渲染
	int			nRectA_Color;
    g_pImmediateContext->OMSetBlendState( NULL, NULL, 0xFFFFFFFF );
	DrawPicture( 0.0f, 0.0f, &g_tBack );
	FlushDrawingPictures();
    g_pImmediateContext->OMSetBlendState( g_pbsAlphaBlend, NULL, 0xFFFFFFFF );
	DrawFanWithColor( faFanB.x, faFanB.y, faFanB.r, faFanB.fAngle1, faFanB.fAngle2, &g_tRect_A, 0xc0ffffff );
	if ( CheckHit( &crCircleA, &faFanB ) ) {
		nRectA_Color = 0xc0ff0000;
	}
	else {
		nRectA_Color = 0xc0ffffff;
	}
	DrawCircleWithColor( crCircleA.x, crCircleA.y, crCircleA.r, &g_tRect_A, nRectA_Color );

    // 显示
	FlushDrawingPictures();

    return S_OK;
}


// 程序入口
int WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE, LPTSTR, int )
{
	LARGE_INTEGER			nNowTime, nLastTime;		// 当前时刻及上一次的时刻
	LARGE_INTEGER			nTimeFreq;					// 时间单位

    // 画面大小
    g_nClientWidth  = VIEW_WIDTH;						// 宽度
    g_nClientHeight = VIEW_HEIGHT;						// 高度

	// Register the window class
    WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
                      _T( "D3D Sample" ), NULL };
    RegisterClassEx( &wc );

	RECT rcRect;
	SetRect( &rcRect, 0, 0, g_nClientWidth, g_nClientHeight );
	AdjustWindowRect( &rcRect, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( _T( "D3D Sample" ), _T( "CheckHit_4_1" ),
						   WS_OVERLAPPEDWINDOW, 100, 20, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top,
						   GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D() ) && SUCCEEDED( MakeShaders() ) )
    {
        // Create the shaders
        if( SUCCEEDED( InitDrawModes() ) )
        {
			if ( SUCCEEDED( InitGeometry() ) ) {					// 创建图形

				// Show the window
				ShowWindow( g_hWnd, SW_SHOWDEFAULT );
				UpdateWindow( g_hWnd );

				InitShapes();									// 物体初始化
				
				QueryPerformanceFrequency( &nTimeFreq );			// 时间单位
				QueryPerformanceCounter( &nLastTime );				// 初始化1帧前的时刻

				// Enter the message loop
				MSG msg;
				ZeroMemory( &msg, sizeof( msg ) );
				while( msg.message != WM_QUIT )
				{
					Render();
					MoveRect();
					do {
						if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
						{
							TranslateMessage( &msg );
							DispatchMessage( &msg );
						}
						QueryPerformanceCounter( &nNowTime );
					} while( ( ( nNowTime.QuadPart - nLastTime.QuadPart ) < ( nTimeFreq.QuadPart / 90 ) ) &&
							 ( msg.message != WM_QUIT ) );
					while( ( ( nNowTime.QuadPart - nLastTime.QuadPart ) < ( nTimeFreq.QuadPart / 60 ) ) &&
						   ( msg.message != WM_QUIT ) )
					{
						QueryPerformanceCounter( &nNowTime );
					}
					nLastTime = nNowTime;
					g_pSwapChain->Present( 0, 0 );					// 显示
				}
			}
        }
    }

    // Clean up everything and exit the app
    Cleanup();
    UnregisterClass( _T( "D3D Sample" ), wc.hInstance );
    return 0;
}
