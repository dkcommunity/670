//-----------------------------------------------------------------------------
// CD3DObject.cpp
//-----------------------------------------------------------------------------

#include "CD3DObject.h"
//#include "d3dfile.h"

//-----------------------------------------------------------------------------
//
// contructor/destructor
//
//-----------------------------------------------------------------------------
CD3DObject::CD3DObject()
{
	SetOrigin();

	// Vertices
	m_nVertices = 0;
	m_pVertices = NULL;

	// Indices
	m_nIndices = 0;
	m_pIndices = NULL;

	// Material
	m_pMaterial = NULL;

	// Object Hierarchy
	m_pParent	= NULL;	

	// ¸Ş¸ğ¸®°¡ ÀâÇôÀÖ´Â »óÅÂÀÎ°¡?
	m_bNew = false;
}

CD3DObject::~CD3DObject()
{
	if (m_pMaterial!=NULL)
	{
		delete m_pMaterial;
	}

	// array¸¦ ¸Ş¸ğ¸®¿¡¼­ Á¦°ÅÇÑ´Ù.
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
CD3DObject::Init(DWORD nVertices, DWORD nIndices)
{
	// °³¼ö°¡ ¾øÀ» °æ¿ì 
	if (nVertices==0 || nIndices==0) 
		return;

	// ÀÏ´Ü ÇØÁ¦
	Release();

	// ¸Ş¸ğ¸® Àâ±â
	m_nVertices = nVertices;
	m_pVertices = new D3DVERTEX [m_nVertices];

	// ¸Ş¸ğ¸® Àâ±â
	m_nIndices = nIndices;
	m_pIndices = new WORD [m_nIndices];

	m_bNew = true;
}


//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CD3DObject::Release()
{
	if (m_bNew)
	{
		//-------------------------------------
		// Release Vertices
		//-------------------------------------
		if (m_pVertices != NULL)
		{
			// ¸ğµç CVertices¸¦ Áö¿î´Ù.
			delete [] m_pVertices;

			m_pVertices = NULL;		
			m_nVertices = 0;
		}

		//-------------------------------------
		// Release Indices
		//-------------------------------------
		if (m_pIndices != NULL)
		{
			// ¸ğµç CIndices¸¦ Áö¿î´Ù.
			delete [] m_pIndices;

			m_pIndices = NULL;		
			m_nIndices = 0;
		}

		m_bNew = false;
	}
}

//-----------------------------------------------------------------------------
// Add Child
//-----------------------------------------------------------------------------
void				
CD3DObject::AddChild(CD3DObject* pObject)
{
	// list¿¡ Ãß°¡
	m_listChild.push_back( pObject );

	// Ãß°¡µÈ ÀÚ½ÄÀÇ parent·Î ¼³Á¤
	m_pParent = this;
}

//-----------------------------------------------------------------------------
// Release Child
//-----------------------------------------------------------------------------
// Child¿¡ µş¸° ¸ğµç Childµéµµ ¾ø¾Ø´Ù.
//-----------------------------------------------------------------------------
void				
CD3DObject::ReleaseChild()
{
	CD3DOBJECT_LIST::iterator iObject = m_listChild.begin();

	while (iObject != m_listChild.end())
	{
		// Child¿¡ µş¸° ¸ğµç ChildµéÀ» ¾ø¾Ø´Ù.
		(*iObject)->ReleaseChild();

		// ChildÀÇ Parent¸¦ ¾ø¾Ø´Ù.
		(*iObject)->m_pParent = NULL;

		// Child¸¦ ¾ø¾Ø´Ù.
		delete *iObject;

		// ´ÙÀ½..
		iObject++;
	}
}

//-----------------------------------------------------------------------------
// Set Origin
//-----------------------------------------------------------------------------
// ±âº» ÁÂÇ¥, ±âº» »óÅÂ(È¸Àü), ±âº» Å©±â...µîÀ¸·Î ÃÊ±âÈ­ÇÑ´Ù.
//-----------------------------------------------------------------------------
void
CD3DObject::SetOrigin()
{
	// »óÅÂ Matrix
	CD3DMath::GetIdentityMatrix( m_matStatus );

	// Position
	m_Position.x = 0;
	m_Position.y = 0;
	m_Position.z = 0;

	// Direction
	m_Direction.x = 0;
	m_Direction.y = 0;
	m_Direction.z = 0;

	// Size
	m_Scale.x = 1.0f;
	m_Scale.y = 1.0f;
	m_Scale.z = 1.0f;
}

//-----------------------------------------------------------------------------
// Draw (bRestore)
//-----------------------------------------------------------------------------
// WorldÁÂÇ¥°è¸¦ Áß½ÉÀ¸·Î
// LocalÁÂÇ¥°è(matStatus)¸¦ Àß~ ¸ÂÃç¼­ Ãâ·ÂÇØ¾ß ÇÑ´Ù.
//
// ObjectÀÇ LocalÁÂÇ¥°è¸¦ WorldÁÂÇ¥°è·Î ¹Ù²ã¼­ Ãâ·ÂÇÑ´Ù.
// --> »ç½ÇÀº WorldÁÂÇ¥°è¸¦ LocalÁÂÇ¥°è·Î ¹Ù²ã¼­ °°Àº È¿°ú¸¦ º¸°Ô ÇÑ´Ù.
//-----------------------------------------------------------------------------
// bRestoreWorld´Â ¿ø·¡ÀÇ WorldÁÂÇ¥°è¸¦ µÇ»ì·Á¾ß ÇÏ´Â°¡?¿¡ ´ëÇÑ °ÍÀÌ´Ù.
//-----------------------------------------------------------------------------
void
CD3DObject::Draw(bool bRestoreWorld)
{	
	if (m_pMaterial!=NULL)
		CDirect3D::GetDevice()->SetMaterial( m_pMaterial );

	D3DMATRIX matWorld, matMyWorld;			

	// WorldÁÂÇ¥°è¸¦ ¾ò´Â´Ù.
	D3DMATRIX worldTemp;
	CDirect3D::GetDevice()->GetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );		

	// ObjectÀÇ LocalÁÂÇ¥°è¸¦ WorldÁÂÇ¥°è·Î ¹Ù²Û´Ù.		
	CD3DMath::MultiplyMatrix(matMyWorld, m_matStatus, matWorld);
	CDirect3D::GetDevice()->SetTransform( D3DTRANSFORMSTATE_WORLD, &matMyWorld );		

	// Object¸¦ ±×¸°´Ù.
	if (m_pVertices)
	{		
		CDirect3D::GetDevice()->DrawIndexedPrimitive( 
									D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
									m_pVertices, m_nVertices, 
									m_pIndices,  m_nIndices, 0 );
	}

	//-----------------------------------------------------------
	// ChildµéÀ» ¸ğµÎ ±×¸°´Ù.
	//-----------------------------------------------------------
	CD3DOBJECT_LIST::iterator iObject = m_listChild.begin();

	while (iObject != m_listChild.end())
	{
		// Child¿¡ µş¸° ¸ğµç ChildµéÀ» ±×¸°´Ù.
		(*iObject)->Draw(matMyWorld, false);		

		// ´ÙÀ½..
		iObject++;
	}

	//-----------------------------------------------------------
	// ¿ø·¡ÀÇ ÁÂÇ¥°è¸¦ µÇ»ì·Á¾ß ÇÏ´Â °æ¿ì
	//-----------------------------------------------------------
	if (bRestoreWorld)		
	{	
		// ¿ø·¡ÀÇ WorldÁÂÇ¥·Î µÇµ¹¸°´Ù.
		CDirect3D::GetDevice()->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
	}
}

//-----------------------------------------------------------------------------
// Draw (ParentWorld, bRestore)
//-----------------------------------------------------------------------------
// ºÎ¸ğ ÁÂÇ¥°è(matParentWorld)¸¦ Áß½ÉÀ¸·Î
// LocalÁÂÇ¥°è(matStatus)¸¦ Àß~ ¸ÂÃç¼­ Ãâ·ÂÇØ¾ß ÇÑ´Ù.
//
// ObjectÀÇ LocalÁÂÇ¥°è¸¦ matParentWorld¿¡ ¸ÂÃç¼­ Ãâ·ÂÇÑ´Ù.
// --> »ç½ÇÀº matParentWorld¸¦ LocalÁÂÇ¥°è·Î ¹Ù²ã¼­ °°Àº È¿°ú¸¦ º¸°Ô ÇÑ´Ù.
//-----------------------------------------------------------------------------
// bRestoreWorld´Â ¿ø·¡ÀÇ WorldÁÂÇ¥°è¸¦ µÇ»ì·Á¾ß ÇÏ´Â°¡?¿¡ ´ëÇÑ °ÍÀÌ´Ù.
//-----------------------------------------------------------------------------
void
CD3DObject::Draw(D3DMATRIX& matParentWorld, bool bRestoreWorld)
{	
	if (m_pMaterial!=NULL)
		CDirect3D::GetDevice()->SetMaterial( m_pMaterial );

	D3DMATRIX matMyWorld;			

	// ObjectÀÇ LocalÁÂÇ¥°è¸¦ WorldÁÂÇ¥°è·Î ¹Ù²Û´Ù.		
	CD3DMath::MultiplyMatrix(matMyWorld, m_matStatus, matParentWorld);
	CDirect3D::GetDevice()->SetTransform( D3DTRANSFORMSTATE_WORLD, &matMyWorld );		

	// Object¸¦ ±×¸°´Ù.
	if (m_pVertices)
	{
		CDirect3D::GetDevice()->DrawIndexedPrimitive( 
									D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
									m_pVertices, m_nVertices, 
									m_pIndices,  m_nIndices, 0 );
	}

	//-----------------------------------------------------------
	// ¿ø·¡ÀÇ ÁÂÇ¥°è¸¦ µÇ»ì·Á¾ß ÇÏ´Â °æ¿ì
	//-----------------------------------------------------------
	if (bRestoreWorld)		
	{	
		// ¿ø·¡ÀÇ WorldÁÂÇ¥·Î µÇµ¹¸°´Ù.
		CDirect3D::GetDevice()->SetTransform( D3DTRANSFORMSTATE_WORLD, &matParentWorld );
	}
}

//-----------------------------------------------------------------------------
// Set Position
//-----------------------------------------------------------------------------
void				
CD3DObject::SetPosition(D3DVECTOR& position)		
{ 
	//-------------------------------------------------------
	// ¿ø·¡ÀÇ Å©±â
	//-------------------------------------------------------
	//if (m_Scale.x!=1 || m_Scale.y!=1 || m_Scale.z!=1)
		CD3DMath::GetScaleMatrix( m_matStatus, m_Scale );

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ È¸Àü °¢µµ
	//-------------------------------------------------------
	if (m_Direction.x!=0)
		CD3DMath::MultiplyRotateXMatrix( m_matStatus, m_Direction.x );

	if (m_Direction.y!=0)
		CD3DMath::MultiplyRotateYMatrix( m_matStatus, m_Direction.y );

	if (m_Direction.z!=0)
		CD3DMath::MultiplyRotateZMatrix( m_matStatus, m_Direction.z );

	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ positionÀ¸·Î ÀÌµ¿ÇÑ´Ù.
	//-------------------------------------------------------
	CD3DMath::MultiplyTranslateMatrix( m_matStatus, position );

	// positionÀ¸·Î ÀÌµ¿ÇÑ´Ù.
	m_Position = position; 

}

//-----------------------------------------------------------------------------
// Set Position
//-----------------------------------------------------------------------------
void				
CD3DObject::SetPosition(float x, float y, float z)	
{
	//-------------------------------------------------------
	// ¿ø·¡ÀÇ Å©±â
	//-------------------------------------------------------
	//if (m_Scale.x!=1 || m_Scale.y!=1 || m_Scale.z!=1)
		CD3DMath::GetScaleMatrix( m_matStatus, m_Scale );

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ È¸Àü °¢µµ
	//-------------------------------------------------------
	if (m_Direction.x!=0)
		CD3DMath::MultiplyRotateXMatrix( m_matStatus, m_Direction.x );

	if (m_Direction.y!=0)
		CD3DMath::MultiplyRotateYMatrix( m_matStatus, m_Direction.y );

	if (m_Direction.z!=0)
		CD3DMath::MultiplyRotateZMatrix( m_matStatus, m_Direction.z );

	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ positionÀ¸·Î ÀÌµ¿ÇÑ´Ù.
	//-------------------------------------------------------
	CD3DMath::MultiplyTranslateMatrix( m_matStatus, x, y, z );

	// positionÀ¸·Î ÀÌµ¿ÇÑ´Ù.
	m_Position.x = x; 
	m_Position.y = y; 
	m_Position.z = z; 
}

//-----------------------------------------------------------------------------
// Set Direction
//-----------------------------------------------------------------------------
void			
CD3DObject::SetDirection(D3DVECTOR& direction)	
{	
	//-------------------------------------------------------
	// ¿ø·¡ÀÇ Å©±â
	//-------------------------------------------------------
	//if (m_Scale.x!=1 || m_Scale.y!=1 || m_Scale.z!=1)
		CD3DMath::GetScaleMatrix( m_matStatus, m_Scale );

	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ È¸ÀüÇÑ´Ù.
	//-------------------------------------------------------
	CD3DMath::MultiplyRotateXMatrix( m_matStatus, direction.x );
	CD3DMath::MultiplyRotateYMatrix( m_matStatus, direction.y );
	CD3DMath::MultiplyRotateZMatrix( m_matStatus, direction.z );

	// direction¸¸Å­ È¸ÀüÇÑ´Ù.
	m_Direction = direction; 

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ À§Ä¡
	//-------------------------------------------------------
	if (m_Position.x!=0 || m_Position.y!=0 || m_Position.z!=0)
		CD3DMath::MultiplyTranslateMatrix( m_matStatus, m_Position );
}

//-----------------------------------------------------------------------------
// Set Direction
//-----------------------------------------------------------------------------
void			
CD3DObject::SetDirection(float x, float y, float z)	
{ 
	//-------------------------------------------------------
	// ¿ø·¡ÀÇ Å©±â
	//-------------------------------------------------------
//	if (m_Scale.x!=1 || m_Scale.y!=1 || m_Scale.z!=1)
		CD3DMath::GetScaleMatrix( m_matStatus, m_Scale );

	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ positionÀ¸·Î ÀÌµ¿ÇÑ´Ù.
	//-------------------------------------------------------
	CD3DMath::MultiplyRotateXMatrix( m_matStatus, x );
	CD3DMath::MultiplyRotateYMatrix( m_matStatus, y );
	CD3DMath::MultiplyRotateZMatrix( m_matStatus, z );

	// direction¸¸Å­ È¸ÀüÇÑ´Ù.
	m_Direction.x = x; 
	m_Direction.y = y; 
	m_Direction.z = z; 
	
	//-------------------------------------------------------
	// ¿ø·¡ÀÇ À§Ä¡
	//-------------------------------------------------------
	if (m_Position.x!=0 || m_Position.y!=0 || m_Position.z!=0)
		CD3DMath::MultiplyTranslateMatrix( m_matStatus, m_Position );

}

//-----------------------------------------------------------------------------
// Set Scale
//-----------------------------------------------------------------------------
void			
CD3DObject::SetScale(D3DVECTOR& Scale)	
{	
	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ scaleÇÑ´Ù.
	//-------------------------------------------------------
	CD3DMath::GetScaleMatrix( m_matStatus, Scale );

	// Scale¸¸Å­ ScaleÇÑ´Ù.
	m_Scale = Scale; 

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ È¸Àü °¢µµ
	//-------------------------------------------------------
	if (m_Direction.x!=0)
		CD3DMath::MultiplyRotateXMatrix( m_matStatus, m_Direction.x );

	if (m_Direction.y!=0)
		CD3DMath::MultiplyRotateYMatrix( m_matStatus, m_Direction.y );

	if (m_Direction.z!=0)
		CD3DMath::MultiplyRotateZMatrix( m_matStatus, m_Direction.z );

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ À§Ä¡
	//-------------------------------------------------------
	if (m_Position.x!=0 || m_Position.y!=0 || m_Position.z!=0)
		CD3DMath::MultiplyTranslateMatrix( m_matStatus, m_Position );

}

//-----------------------------------------------------------------------------
// Set Scale
//-----------------------------------------------------------------------------
void			
CD3DObject::SetScale(float x, float y, float z)	
{ 
	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ scaleÇÑ´Ù.
	//-------------------------------------------------------
	CD3DMath::GetScaleMatrix( m_matStatus, x, y, z );

	// Scale¸¸Å­ ScaleÇÑ´Ù.
	m_Scale.x = x; 
	m_Scale.y = y; 
	m_Scale.z = z; 

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ È¸Àü °¢µµ
	//-------------------------------------------------------
	if (m_Direction.x!=0)
		CD3DMath::MultiplyRotateXMatrix( m_matStatus, m_Direction.x );

	if (m_Direction.y!=0)
		CD3DMath::MultiplyRotateYMatrix( m_matStatus, m_Direction.y );

	if (m_Direction.z!=0)
		CD3DMath::MultiplyRotateZMatrix( m_matStatus, m_Direction.z );

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ À§Ä¡
	//-------------------------------------------------------
	if (m_Position.x!=0 || m_Position.y!=0 || m_Position.z!=0)
		CD3DMath::MultiplyTranslateMatrix( m_matStatus, m_Position );
}

//-----------------------------------------------------------------------------
// Move
//-----------------------------------------------------------------------------
// ÇöÀç À§Ä¡¿¡¼­ (x,y,z)¸¸Å­ ÀÌµ¿ÇÑ´Ù.
// ¸ğµç VerticesµéÀ» MoveÇÑ´Ù.
// ¿øÁ¡À¸·Î ÀÌµ¿ --> Move --> ¿ø·¡ ÁÂÇ¥·Î ÀÌµ¿...
//-----------------------------------------------------------------------------
void
CD3DObject::Move(float x, float y, float z)
{
	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ ScaleÇÑ´Ù.
	//-------------------------------------------------------
	//if (m_Scale.x!=1 || m_Scale.y!=1 || m_Scale.z!=1)
		CD3DMath::GetScaleMatrix( m_matStatus, m_Scale );

	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ È¸ÀüÇÑ´Ù.		
	//-------------------------------------------------------
	if (m_Direction.x!=0)
		CD3DMath::MultiplyRotateXMatrix( m_matStatus, m_Direction.x );
	if (m_Direction.y!=0)
		CD3DMath::MultiplyRotateYMatrix( m_matStatus, m_Direction.y );
	if (m_Direction.z!=0)
		CD3DMath::MultiplyRotateZMatrix( m_matStatus, m_Direction.z );

	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ positionÀ¸·Î ÀÌµ¿ÇÑ´Ù.
	//-------------------------------------------------------
	m_Position.x += x;
	m_Position.y += y;
	m_Position.z += z;

	CD3DMath::MultiplyTranslateMatrix( m_matStatus, m_Position );	
}

//-----------------------------------------------------------------------------
// Scale
//-----------------------------------------------------------------------------
// ÇöÀç Å©±â¿¡¼­ (x,y,z)¸¸Å­ °¢ ¹æÇâÀ¸·Î ¸ğµç VerticesµéÀ» ScaleÇÑ´Ù.
// ¿øÁ¡À¸·Î ÀÌµ¿ --> Scale --> ¿ø·¡ ÁÂÇ¥·Î ÀÌµ¿...
//-----------------------------------------------------------------------------
void
CD3DObject::Scale(float x, float y, float z)
{
	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ ScaleÇÑ´Ù.
	//-------------------------------------------------------
	// Scale¸¸Å­ ScaleÇÑ´Ù.
	m_Scale.x *= x; 
	m_Scale.y *= y; 
	m_Scale.z *= z; 	

	if (m_Scale.x < 0) m_Scale.x = 0.001f;
	if (m_Scale.y < 0) m_Scale.y = 0.001f;
	if (m_Scale.z < 0) m_Scale.z = 0.001f;

	CD3DMath::GetScaleMatrix( m_matStatus, m_Scale.x, m_Scale.y, m_Scale.z );
	

	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ È¸ÀüÇÑ´Ù.		
	//-------------------------------------------------------
	if (m_Direction.x!=0)
		CD3DMath::MultiplyRotateXMatrix( m_matStatus, m_Direction.x );
	if (m_Direction.y!=0)
		CD3DMath::MultiplyRotateYMatrix( m_matStatus, m_Direction.y );
	if (m_Direction.z!=0)
		CD3DMath::MultiplyRotateZMatrix( m_matStatus, m_Direction.z );

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ À§Ä¡
	//-------------------------------------------------------
	if (m_Position.x!=0 || m_Position.y!=0 || m_Position.z!=0)
		CD3DMath::MultiplyTranslateMatrix( m_matStatus, m_Position );
}

//-----------------------------------------------------------------------------
// Rotate
//-----------------------------------------------------------------------------
// ÇöÀç È¸ÀüµÈ °¢µµ¿¡¼­... (x,y,z)¸¸Å­
// ¸ğµç VerticesµéÀ» RotateÇÑ´Ù.
// ¿øÁ¡À¸·Î ÀÌµ¿ --> Rotate --> ¿ø·¡ ÁÂÇ¥·Î ÀÌµ¿...
//-----------------------------------------------------------------------------
void
CD3DObject::Rotate(float x, float y, float z)
{
	//-------------------------------------------------------
	// ¿ø·¡ÀÇ Å©±â
	//-------------------------------------------------------
	//if (m_Scale.x!=1 || m_Scale.y!=1 || m_Scale.z!=1)
		CD3DMath::GetScaleMatrix( m_matStatus, m_Scale );

	//-------------------------------------------------------
	// »óÅÂ Matrix¸¦ È¸ÀüÇÑ´Ù.		
	//-------------------------------------------------------
	m_Direction.x += x;
	m_Direction.y += y;
	m_Direction.z += z;
	CD3DMath::MultiplyRotateXMatrix( m_matStatus, m_Direction.x );
	CD3DMath::MultiplyRotateYMatrix( m_matStatus, m_Direction.y );
	CD3DMath::MultiplyRotateZMatrix( m_matStatus, m_Direction.z );

	//-------------------------------------------------------
	// ¿ø·¡ÀÇ À§Ä¡
	//-------------------------------------------------------
	if (m_Position.x!=0 || m_Position.y!=0 || m_Position.z!=0)
		CD3DMath::MultiplyTranslateMatrix( m_matStatus, m_Position );
}

//-----------------------------------------------------------------------------
// Set Material
//-----------------------------------------------------------------------------
void				
CD3DObject::SetMaterial(D3DMATERIAL9& mtrl)	
{ 
	if (m_pMaterial==NULL)
	{
		m_pMaterial = new D3DMATERIAL9; 
	}

	*m_pMaterial = mtrl; 
}

//-----------------------------------------------------------------------------
// Create Cube (size)
//-----------------------------------------------------------------------------
// Cube(À°¸éÃ¼)¿¡ ´ëÇÑ Vertices¿Í IndicesµéÀ» »ı¼ºÇÑ´Ù.
//-----------------------------------------------------------------------------
void
CD3DObject::CreateCube(float sizeX, float sizeY, float sizeZ)
{
	//#define NUM_CUBE_VERTICES (6*4)
	//#define NUM_CUBE_INDICES  (6*2*3)
	//---------------------------------------------
	// memory Àâ±â
	//---------------------------------------------
	Init(24, 36);

    // Define the normals for the cube
    D3DVECTOR n0( 0.0f, 0.0f,-1.0f ); // Front face
    D3DVECTOR n1( 0.0f, 0.0f, 1.0f ); // Back face
    D3DVECTOR n2( 0.0f, 1.0f, 0.0f ); // Bottom face
    D3DVECTOR n3( 0.0f,-1.0f, 0.0f ); // Top face
    D3DVECTOR n4( 1.0f, 0.0f, 0.0f ); // Left face
    D3DVECTOR n5(-1.0f, 0.0f, 0.0f ); // Right face

	D3DVERTEX*	pVertices	= m_pVertices;
	WORD*		pIndices	= m_pIndices;

    // Front face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX, sizeY,-sizeZ), n0, 0.01f, 0.01f ); 
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX, sizeY,-sizeZ), n0, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX,-sizeY,-sizeZ), n0, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX,-sizeY,-sizeZ), n0, 0.01f, 0.99f ); 

    // Back face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX, sizeY, sizeZ), n1, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX,-sizeY, sizeZ), n1, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX,-sizeY, sizeZ), n1, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX, sizeY, sizeZ), n1, 0.01f, 0.01f );

    // Top face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX, sizeY, sizeZ), n2, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX, sizeY, sizeZ), n2, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX, sizeY,-sizeZ), n2, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX, sizeY,-sizeZ), n2, 0.01f, 0.99f );

    // Bottom face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX,-sizeY, sizeZ), n3, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX,-sizeY,-sizeZ), n3, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX,-sizeY,-sizeZ), n3, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX,-sizeY, sizeZ), n3, 0.99f, 0.99f );

    // Right face
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX, sizeY,-sizeZ), n4, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX, sizeY, sizeZ), n4, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX,-sizeY, sizeZ), n4, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( sizeX,-sizeY,-sizeZ), n4, 0.01f, 0.99f );

    // Left face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX, sizeY,-sizeZ), n5, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX,-sizeY,-sizeZ), n5, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX,-sizeY, sizeZ), n5, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-sizeX, sizeY, sizeZ), n5, 0.01f, 0.01f );

    // Set up the indices for the cube
    *pIndices++ =  0+0;   *pIndices++ =  0+1;   *pIndices++ =  0+2;
    *pIndices++ =  0+2;   *pIndices++ =  0+3;   *pIndices++ =  0+0;
    *pIndices++ =  4+0;   *pIndices++ =  4+1;   *pIndices++ =  4+2;
    *pIndices++ =  4+2;   *pIndices++ =  4+3;   *pIndices++ =  4+0;
    *pIndices++ =  8+0;   *pIndices++ =  8+1;   *pIndices++ =  8+2;
    *pIndices++ =  8+2;   *pIndices++ =  8+3;   *pIndices++ =  8+0;
    *pIndices++ = 12+0;   *pIndices++ = 12+1;   *pIndices++ = 12+2;
    *pIndices++ = 12+2;   *pIndices++ = 12+3;   *pIndices++ = 12+0;
    *pIndices++ = 16+0;   *pIndices++ = 16+1;   *pIndices++ = 16+2;
    *pIndices++ = 16+2;   *pIndices++ = 16+3;   *pIndices++ = 16+0;
    *pIndices++ = 20+0;   *pIndices++ = 20+1;   *pIndices++ = 20+2;
    *pIndices++ = 20+2;   *pIndices++ = 20+3;   *pIndices++ = 20+0;
}

//-----------------------------------------------------------------------------
// Create Plane (size)
//-----------------------------------------------------------------------------
// Plane(Æò¸é)¿¡ ´ëÇÑ Vertices¿Í IndicesµéÀ» »ı¼ºÇÑ´Ù.
//-----------------------------------------------------------------------------
void
CD3DObject::CreatePlane(float size)
{
	//#define NUM_CUBE_VERTICES (2*4)
	//#define NUM_CUBE_INDICES  (2*2*3)
	//---------------------------------------------
	// memory Àâ±â
	//---------------------------------------------
	Init(8, 12);

    // Define the normals for the cube
    D3DVECTOR n0( 0.0f, 0.0f,-1.0f ); // Front face
    D3DVECTOR n1( 0.0f, 0.0f, 1.0f ); // Back face    

	D3DVERTEX*	pVertices	= m_pVertices;
	WORD*		pIndices	= m_pIndices;

    // Front face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-size, size, 0), n0, 0.01f, 0.01f ); 
    *pVertices++ = D3DVERTEX( D3DVECTOR( size, size, 0), n0, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( size,-size, 0), n0, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-size,-size, 0), n0, 0.01f, 0.99f ); 

    // Back face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-size, size, 0), n1, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-size,-size, 0), n1, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( size,-size, 0), n1, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( size, size, 0), n1, 0.01f, 0.01f );

    // Set up the indices for the cube
    *pIndices++ =  0+0;   *pIndices++ =  0+1;   *pIndices++ =  0+2;
    *pIndices++ =  0+2;   *pIndices++ =  0+3;   *pIndices++ =  0+0;
    *pIndices++ =  4+0;   *pIndices++ =  4+1;   *pIndices++ =  4+2;
    *pIndices++ =  4+2;   *pIndices++ =  4+3;   *pIndices++ =  4+0;  
}


//-----------------------------------------------------------------------------
// Create Cylinder (size)
//-----------------------------------------------------------------------------
// Cylinder(¿ø~±âµÕ)¿¡ ´ëÇÑ Vertices¿Í IndicesµéÀ» »ı¼ºÇÑ´Ù.
//
// r : ¹İÁö¸§ 
// l : ±âµÕÀÇ ±æÀÌ 
// n : ¿øÀ» ÀÌ·ç´Â »ï°¢Çü °³¼ö
//-----------------------------------------------------------------------------
void
CD3DObject::CreateCylinder(float r, float l, int n)
{
	// ÃÖ¼ÒÇÑ »ï°¢ÇüÀº µÇ¾î¾ß ÇÑ´Ù.
	if (n<3) n=3;

	//#define NUM_CUBE_VERTICES (6*4)
	//#define NUM_CUBE_INDICES  (6*2*3)
	//---------------------------------------------
	// memory Àâ±â
	//---------------------------------------------
	Init(6*n+2, 12*n);
	//Init(6*n+2, 6*n);

    // Define the normals for the cube
    D3DVECTOR n0( 0.0f, 1.0f, 0.0f ); // Top face
    D3DVECTOR n1( 0.0f,-1.0f, 0.0f ); // Bottom face
	D3DVECTOR n2( 0.0f, 0.0f, 0.0f );
    
	float l2 = l / 2;

	// Top face
	float	x=0.0f,
			z=0.0f, 
			xo, zo;

	float t = 0.0f;
	float degree = 360.0f / n;
	float tu = 0.0f;
	float tv = 0.0f;
	float tdeg = 1.0f / n;
	float rad;

	int iV = 0, iI = 0;
	m_pVertices[iV++] = D3DVERTEX( D3DVECTOR(0, l2, 0), n0, 0, 0 );	// 0
	m_pVertices[iV++] = D3DVERTEX( D3DVECTOR(0, -l2, 0), n1, 0, 0 );	// 1

	for (int i=0; i<=n; t+=degree, tu+=tdeg, tv+=tdeg, i++)
	{	
		xo = x; 
		zo = z;
		rad = t * g_DEGTORAD;
		z = r * (float)sin( rad );
		x = r * (float)cos( rad );
	
		if (i!=n)
		{
			m_pVertices[iV++] = D3DVERTEX( D3DVECTOR(x, l2, z), n0, 1, 1);		// 2
			m_pVertices[iV++] = D3DVERTEX( D3DVECTOR(x, -l2, z), n1, 1, 1);	// 3
		}		

		if (i!=0)
		{
			n2.x = xo + x;
			n2.y = 0;
			n2.z = zo + z;
			CD3DMath::NormalizeVector( n2 );			
			m_pVertices[iV++] = D3DVERTEX( D3DVECTOR(xo, l2, zo), n2, tu, 0 );	// 4
			m_pVertices[iV++] = D3DVERTEX( D3DVECTOR(x, l2, z), n2, tu+tdeg, 0 );		// 5
			m_pVertices[iV++] = D3DVERTEX( D3DVECTOR(x, -l2, z), n2, tu+tdeg, 1 );	// 6
			m_pVertices[iV++] = D3DVERTEX( D3DVECTOR(xo, -l2, zo), n2, tu, 1 );	// 7
		
			if (i==1)
			{
				// Top face
				m_pIndices[iI++] = 0;
				m_pIndices[iI++] = iV-6;
				m_pIndices[iI++] = iV-8;

				// Bottom face
				m_pIndices[iI++] = 1;
				m_pIndices[iI++] = iV-7;
				m_pIndices[iI++] = iV-5;
			}
			else if (i==n)
			{
				// Top face
				m_pIndices[iI++] = 0;
				m_pIndices[iI++] = 2;
				m_pIndices[iI++] = iV-10;

				// Bottom face
				m_pIndices[iI++] = 1;
				m_pIndices[iI++] = iV-9;
				m_pIndices[iI++] = 3;
			}	
			else
			{
				// Top face
				m_pIndices[iI++] = 0;
				m_pIndices[iI++] = iV-6;
				m_pIndices[iI++] = iV-12;

				// Bottom face
				m_pIndices[iI++] = 1;
				m_pIndices[iI++] = iV-11;
				m_pIndices[iI++] = iV-5;
			}

			// Side face
			m_pIndices[iI++] = iV-4;
			m_pIndices[iI++] = iV-3;
			m_pIndices[iI++] = iV-2;

			m_pIndices[iI++] = iV-2;
			m_pIndices[iI++] = iV-1;
			m_pIndices[iI++] = iV-4;		
		}
	}
}

//-----------------------------------------------------------------------------
// Create TilePlane (size)
//-----------------------------------------------------------------------------
// TilePlane¿¡ ´ëÇÑ Vertices¿Í IndicesµéÀ» »ı¼ºÇÑ´Ù.
// Texture´Â ÀÏ´Ü... Å×½ºÆ®¿ë..
// 256*256¿¡ (64*32)TileÀÌ 4*8°³ Ãâ·ÂµÇ¾î ÀÖ´Â°É »ç¿ëÇß´Ù.
//-----------------------------------------------------------------------------
void
CD3DObject::CreateTilePlane(int width, int height, float firstX, float firstY, float size)
{
	//---------------------------------------------
	// memory Àâ±â
	//---------------------------------------------
	Init(4*width*height, 6*width*height);
	
	// Define the normals for the cube
	D3DVERTEX*	pVertices	= m_pVertices;
	WORD*		pIndices	= m_pIndices;    
	
	D3DVECTOR n0( 0.0f, 0.0f,-1.0f ); // Front face   	
	
	int i, j;
	float x,y;

	float lenX = 1.0f/4.0f;
	float lenY = 1.0f/8.0f;
	float left, right, up, down;

	y = firstY * size;
	int n = 0;
	for (i=0; i<height; i++)
	{		
		x = firstX * size;
		for (j=0; j<width; j++)
		{	
			// TextureÀÇ ºñÀ²
			up = (float)(rand()%8)/8.0f;
			down = up + lenY;
			left = (float)(rand()%4)/4.0f;
			right = left + lenX;		
				
			// Front face
			*pVertices++ = D3DVERTEX( D3DVECTOR( x,		y, 0),		n0, left,	up ); 
			*pVertices++ = D3DVERTEX( D3DVECTOR( x+size,y, 0),		n0, right,	up );
			*pVertices++ = D3DVERTEX( D3DVECTOR( x+size,y-size, 0), n0, right,	down );
			*pVertices++ = D3DVERTEX( D3DVECTOR( x,		y-size, 0), n0, left,	down );
    
			// Set up the indices for the cube
			*pIndices++ =  n+0;   *pIndices++ =  n+1;   *pIndices++ =  n+2;
			*pIndices++ =  n+2;   *pIndices++ =  n+3;   *pIndices++ =  n+0;   

			x += size;		

			n += 4;
		}
		y -= size;
	}
}

//-----------------------------------------------------------------------------
// Load X
//-----------------------------------------------------------------------------
/*
bool
CD3DObject::LoadX(char *filename)
{	
	// Áö±İ ÀâÇôÀÖ´Â ¸Ş¸ğ¸®¸¦ Á¦°ÅÇÑ´Ù.
	Release();	

	m_pFile = new CD3DFile();

	HRESULT hr1 = m_pFile->Load( filename );
    HRESULT hr2 = m_pFile->GetMeshVertices( "", &m_pVertices,
                                                  &m_nVertices );
    HRESULT hr3 = m_pFile->GetMeshIndices( "", &m_pIndices,
                                                 &m_nIndices );

    if( FAILED(hr1|hr2|hr3) )
    {
        MessageBox( NULL, TEXT("Could not load X file."),
                    NULL, MB_OK|MB_ICONERROR );
        return false;//E_FAIL;
    }   

    return true;//S_OK;
}
*/


//-----------------------------------------------------------------------------
// Apply ShpereMap
//-----------------------------------------------------------------------------
void
CD3DObject::ApplySphereMap()
{
    // Get the current world-view matrix
    D3DMATRIX matWorld, matView, matWV;
	CDirect3D::GetDevice()->GetTransform( D3DTRANSFORMSTATE_VIEW,  &matView );
    CDirect3D::GetDevice()->GetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
	//CD3DMath::MultiplyMatrix( matWV, matWorld, matView );
	CD3DMath::MultiplyMatrix( matWV, matWorld, matView );
	CD3DMath::MultiplyMatrix( matWV, m_matStatus, matWV );	

    // Extract world-view matrix elements for speed
    float m11 = matWV._11,   m21 = matWV._21,   m31 = matWV._31;
    float m12 = matWV._12,   m22 = matWV._22,   m32 = matWV._32;
    float m13 = matWV._13,   m23 = matWV._23,   m33 = matWV._33;

    // Loop through the vertices, transforming each one and calculating
    // the correct texture coordinates.
    for( DWORD i = 0; i < m_nVertices; i++ )
    {
        float nx = m_pVertices[i].nx;
        float ny = m_pVertices[i].ny;
        float nz = m_pVertices[i].nz;

        // Check the z-component, to skip any vertices that face backwards
        if( nx*m13 + ny*m23 + nz*m33 > 0.0f )
            continue;

        // Assign the spheremap's texture coordinates
        m_pVertices[i].tu = 0.5f * ( 1.0f + ( nx*m11 + ny*m21 + nz*m31 ) );
        m_pVertices[i].tv = 0.5f * ( 1.0f - ( nx*m12 + ny*m22 + nz*m32 ) );
    }
}
