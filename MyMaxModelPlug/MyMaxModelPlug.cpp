//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
//				3DXI file exporter project template
//				For a more in-depth exemple of a 3DXI exporter,
//				please refer to maxsdk\samples\igame\export.
// AUTHOR:		Jean-Francois Yelle - created Mar.20.2007
//***************************************************************************/

#include "MyMaxModelPlug.h"

#include "3dsmaxsdk_preinclude.h"
#include "IGame/IGame.h"
#include "IGame/IGameModifier.h" 
#include "ModelStruct.h"

#include <string>
#include <map>
#include <vector>
#include <fstream>

using namespace std;


#define MyMaxModelPlug_CLASS_ID	Class_ID(0x4c1da649, 0x886f2394)

class MyMaxModelPlug : public SceneExport 
{
	public:
		
		virtual int				ExtCount();					// Number of extensions supported
		virtual const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		virtual const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		virtual const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		virtual const TCHAR *	AuthorName();				// ASCII Author name
		virtual const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		virtual const TCHAR *	OtherMessage1();			// Other message #1
		virtual const TCHAR *	OtherMessage2();			// Other message #2
		virtual unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		virtual void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		virtual BOOL SupportsOptions(int ext, DWORD options);
		virtual int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		MyMaxModelPlug();
		virtual ~MyMaxModelPlug();

	protected:
		int GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode = NULL);
		void EraseFileNameSuffix(const char* input, string& output);
		void AddFileNameSuffix(string& fileName, const char* suffix);

		bool ExportBones();
		bool ExportMesh(IGameNode* pNode);

	private:
		static HWND hParams;

		IGameScene* m_pIGame;
		int m_TopNodeNum;
		map<int, int> m_BoneIDMap;

		string m_ModelFileName;
		string m_BoneFileName;
		string m_MeshFileName;

		MaxModelHead m_ModelHead;
		vector<MaxBone> m_Bones;
};



class MyMaxModelPlugClassDesc : public ClassDesc2 {
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return new MyMaxModelPlug(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return MyMaxModelPlug_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("MyMaxModelPlug"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};

static MyMaxModelPlugClassDesc MyMaxModelPlugDesc;
ClassDesc2* GetMyMaxModelPlugDesc() { return &MyMaxModelPlugDesc; }





INT_PTR CALLBACK MyMaxModelPlugOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static MyMaxModelPlug *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (MyMaxModelPlug *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- MyMaxModelPlug -------------------------------------------------------
MyMaxModelPlug::MyMaxModelPlug()
{

}

MyMaxModelPlug::~MyMaxModelPlug() 
{

}

int MyMaxModelPlug::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *MyMaxModelPlug::Ext(int n)
{		
	#pragma message	(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("model/mesh/bone");
}

const TCHAR *MyMaxModelPlug::LongDesc()
{
	#pragma message	(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\""))
	return _T("");
}
	
const TCHAR *MyMaxModelPlug::ShortDesc() 
{			
	#pragma message	(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("MyMaxModelExport");
}

const TCHAR *MyMaxModelPlug::AuthorName()
{			
	#pragma message	(TODO("Return ASCII Author name"))
	return _T("Levy");
}

const TCHAR *MyMaxModelPlug::CopyrightMessage() 
{	
	#pragma message	(TODO("Return ASCII Copyright message"))
	return _T("");
}

const TCHAR *MyMaxModelPlug::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *MyMaxModelPlug::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int MyMaxModelPlug::Version()
{				
	#pragma message	(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void MyMaxModelPlug::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL MyMaxModelPlug::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int MyMaxModelPlug::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
{
	int i;
	if (i_currentNode == NULL)
	{
		i_currentNode = GetCOREInterface()->GetRootNode();
	}
	else // IGame will crash 3ds Max if it is initialised with the root node.
	{
	    i_nodeTab.AppendNode(i_currentNode);
	}
	for (i = 0; i < i_currentNode->NumberOfChildren(); i++)
	{
		GetSceneNodes(i_nodeTab, i_currentNode->GetChildNode(i));
	}
	return i_nodeTab.Count();
}

void MyMaxModelPlug::EraseFileNameSuffix(const char* input, string& output) {
	string name(input);

	int suffixStratIndex = name.find_last_of('.');
	int nameLength = name.size();

	if (suffixStratIndex < nameLength) {
		output = name.substr(0, suffixStratIndex);
	}
}

void MyMaxModelPlug::AddFileNameSuffix(string& fileName, const char* suffix) {
	fileName.append(suffix);
}

bool MyMaxModelPlug::ExportBones() {
	// 确定场景动画播放时间，以及帧数
	m_ModelHead.boneNum = 0;
	m_ModelHead.meshNum = 0;
	m_ModelHead.startTime = m_pIGame->GetSceneStartTime();
	m_ModelHead.endTime = m_pIGame->GetSceneEndTime();
	m_ModelHead.frameInterval = m_pIGame->GetSceneTicks();
	m_ModelHead.frameNum = (m_ModelHead.endTime - m_ModelHead.startTime) / m_ModelHead.frameInterval;

	for (int i = 0; i < m_TopNodeNum; ++i) {
		IGameNode* pNode = m_pIGame->GetTopLevelNode(i);
		IGameObject* pObj = pNode->GetIGameObject();

		if (pObj->GetIGameType() == IGameObject::IGAME_BONE) {
			int oldID = pNode->GetNodeID();

			// 如果这个ID不在IDMap中，添加新的ID映射
			if (m_BoneIDMap.find(oldID) == m_BoneIDMap.end()) {
				int newID = m_BoneIDMap.size();
				m_BoneIDMap.insert(make_pair(oldID, newID));
			}

			// 获取骨骼信息
			MaxBone bone;
			GMatrix originMatrix = pNode->GetObjectTM(m_ModelHead.startTime);
			GMatrix inverseMatrix = originMatrix.Inverse();

			for (int time = m_ModelHead.startTime; time < m_ModelHead.endTime; time += m_ModelHead.frameInterval) {
				GMatrix objectMatrix = pNode->GetObjectTM(time);
				GMatrix frameMatrix = inverseMatrix * objectMatrix;

				bone.frameMatrixs.push_back(frameMatrix);
			}

			m_Bones.push_back(bone);
		}
		else if (pObj->GetIGameType() == IGameObject::IGAME_MESH) {
			// 统计网格个数
			++m_ModelHead.meshNum;
		}
	}

	m_ModelHead.boneNum = m_Bones.size();

	// ************************* 写入文本文件 *************************
	ofstream boneFile;
	boneFile.open(m_BoneFileName.c_str(), ios::out);

	boneFile << "Bone Number: " << m_ModelHead.boneNum << endl;
	boneFile << "Start Time: " << m_ModelHead.startTime << endl;
	boneFile << "End Time: " << m_ModelHead.endTime << endl;
	boneFile << "Frame Interval: " << m_ModelHead.frameInterval << endl;
	boneFile << "Frame Number: " << m_ModelHead.frameNum << endl;

	vector<MaxBone>::iterator boneIterator = m_Bones.begin();
	int boneID = 0;
	while (boneIterator != m_Bones.end()) {
		boneFile << "Bone-" << boneID << ":" << endl;

		for (int frameIndex = 0; frameIndex < m_ModelHead.frameNum; ++frameIndex) {
			for (int i = 0; i < 4; ++i) {
				Point4 row = (*boneIterator).frameMatrixs[frameIndex].GetRow(i);

				boneFile << row.x << " " << row.y << " " << row.z << " " << row.w << endl;
			}

			boneFile << "======" << endl;
		}
		boneFile << endl;

		boneIterator++;
		++boneID;
	}


	// ************************* 写入二进制文件 *************************
	ofstream modelFile;
	modelFile.open(m_ModelFileName.c_str(), ios::out | ios::binary);

	modelFile.write((char*)&m_ModelHead, sizeof(MaxModelHead));
	//float* boneData = new float[4 * 4 * m_ModelHead.frameNum * m_ModelHead.boneNum];
	for (int boneIndex = 0; boneIndex < m_ModelHead.boneNum; ++boneIndex) {
		for (int frameIndex = 0; frameIndex < m_ModelHead.frameNum; ++frameIndex) {
			modelFile.write((char*)m_Bones[boneIndex].frameMatrixs[frameIndex].GetAddr(), sizeof(float) * 4 * 4);
		}
	}

	modelFile.close();

	return true;
}

bool MyMaxModelPlug::ExportMesh(IGameNode* pNode) {
	IGameMesh* pMesh = (IGameMesh*)(pNode->GetIGameObject());
	pMesh->InitializeData();

	MaxMesh mesh;
	strcpy(mesh.head.name, pNode->GetName());
	mesh.head.vertexNum = pMesh->GetNumberOfVerts();
	mesh.head.triangleNum = pMesh->GetNumberOfFaces();

	// 获得SKIN信息
	bool hasSkin = pMesh->IsObjectSkinned();
	int skinnedVertexNum = 0;

	if (hasSkin) {
		IGameSkin * pSkin = pMesh->GetIGameSkin();
		int skinnedVertexNum = pSkin->GetNumOfSkinnedVerts();
		if (skinnedVertexNum != mesh.head.vertexNum) {
			// 只要模型有蒙皮，必须所有顶点都蒙皮
			return false;
		}

		for (int vertexIndex = 0; vertexIndex < mesh.head.vertexNum; ++vertexIndex) {
			MaxVertex vertex;

			Point3 position = pMesh->GetVertex(vertexIndex);
			vertex.x = position.x;
			vertex.y = position.y;
			vertex.z = position.z;
			vertex.boneID[0] = -1;
			vertex.boneID[1] = -1;

			int boneNum = pSkin->GetNumberOfBones(vertexIndex);
			if (boneNum > 2 || boneNum < 1) {
				// 一个顶点必须被1个或2个骨骼绑定
				return false;
			}

			for (int boneIndex = 0; boneIndex < boneNum; ++boneIndex) {
				// 记录骨骼索引
				int oldID = pSkin->GetBoneID(vertexIndex, boneIndex);
				map<int, int>::const_iterator it = m_BoneIDMap.find(oldID);
				if (it == m_BoneIDMap.end()) {
					// 若程序运行正确，表中一定存在对应的骨骼ID
					return false;
				}

				vertex.boneID[boneIndex] = it->second;
				vertex.blend[boneIndex] = pSkin->GetWeight(vertexIndex, boneIndex);	// 记录骨骼权重
			}

			mesh.vertices.push_back(vertex);
		}
	}
	else {
		for (int vertexIndex = 0; vertexIndex < mesh.head.vertexNum; ++vertexIndex) {
			MaxVertex vertex;

			Point3 position = pMesh->GetVertex(vertexIndex);
			vertex.x = position.x;
			vertex.y = position.y;
			vertex.z = position.z;

			mesh.vertices.push_back(vertex);
		}
	}

	vector<Point3> vertexNormals(mesh.head.vertexNum, Point3(0.0f, 0.0f, 0.0f));

	Tab<int> textureMaps = pMesh->GetActiveMapChannelNum();
	vector<Point3> vertexUVWs(mesh.head.vertexNum, Point3(-999.999f, -999.999f, -999.999f));

	// ****************** 计算顶点法线 *******************
	for (int faceIndex = 0; faceIndex < mesh.head.triangleNum; ++faceIndex) {
		FaceEx* pFace = pMesh->GetFace(faceIndex);

		MaxVertex* pV[3];

		for (int i = 0; i < 3; ++i) {
			pV[i] = &(mesh.vertices[pFace->vert[i]]);
		}

		// 求得三个顶点两两之间的向量
		Point3 vector10(pV[1]->x - pV[0]->x, pV[1]->y - pV[0]->y, pV[1]->z - pV[0]->z);
		Point3 vector21(pV[2]->x - pV[1]->x, pV[2]->y - pV[1]->y, pV[2]->z - pV[1]->z);
		Point3 vector02(pV[0]->x - pV[2]->x, pV[0]->y - pV[2]->y, pV[0]->z - pV[2]->z);

		vector10 = vector10.Normalize();
		vector21 = vector21.Normalize();
		vector02 = vector02.Normalize();

		// 求得夹角作为法线权值
		float angle[3];
		angle[0] = acos(-(vector10.x * vector02.x + vector10.y * vector02.y + vector10.z * vector02.z));
		angle[1] = acos(-(vector10.x * vector21.x + vector10.y * vector21.y + vector10.z * vector21.z));
		angle[2] = acos(-(vector21.x * vector02.x + vector21.y * vector02.y + vector21.z * vector02.z));

		// 计算加权后的法线
		for (int i = 0; i < 3; i++) {
			if (angle[i] < 0.0f) {
				// angle一定大于0
				return false;
			}

			vertexNormals[pFace->vert[i]] += angle[i] * pMesh->GetNormal(pFace, i);
		}
	}

	// ****************** 获取纹理贴图UV坐标 *******************
	for (int faceIndex = 0; faceIndex < mesh.head.triangleNum; ++faceIndex) {
		FaceEx* pFace = pMesh->GetFace(faceIndex);

		//求得该三角形对应的三个顶点坐标，以及uvw坐标
		for (int i = 0; i < 3; ++i) {
			Point3 tempUVW;
			// ？？？这里只有最后一次循环生效
			for (int texMapIndex = 0; texMapIndex < textureMaps.Count(); ++texMapIndex) {
				// 在3ds Max中，同一顶点可能会具有不同的UV坐标
				DWORD indices[3];
				if (pMesh->GetMapFaceIndex(textureMaps[texMapIndex], faceIndex, indices)) {
					tempUVW = pMesh->GetMapVertex(textureMaps[texMapIndex], indices[i]);
				}
				else {
					tempUVW = pMesh->GetMapVertex(textureMaps[texMapIndex], pFace->vert[i]);
				}
			}

			// V坐标修正
			tempUVW.y = 1.0f - tempUVW.y;	

			// 如果对应顶点UVW坐标尚未初始化
			if (vertexUVWs[pFace->vert[i]] == Point3(-999.999f, -999.999f, -999.999f)) {
				vertexUVWs[pFace->vert[i]] = tempUVW;
			}
			// 如果对应顶点坐标UVW坐标已经初始化且与tempUVW不同，则建立一个新顶点并加入网格
			else if (vertexUVWs[pFace->vert[i]] != tempUVW) {
				mesh.vertices.push_back(mesh.vertices[pFace->vert[i]]);
				vertexNormals.push_back(vertexNormals[pFace->vert[i]]);
				vertexUVWs.push_back(tempUVW);

				pFace->vert[i] = mesh.head.vertexNum++;
			}
		}
	}

	// ************************* 写入文本文件 *************************
	ofstream meshFile;
	string meshName(mesh.head.name);
	AddFileNameSuffix(meshName, ".mesh");
	meshFile.open(meshName, ios::out);
	meshFile << "Mesh Name: " << mesh.head.name << endl;
	meshFile << "Vertex Number: "	<< mesh.head.vertexNum	<< endl;
	meshFile << "Triangle Number: " << mesh.head.triangleNum << endl;
	meshFile << endl;
	meshFile << "Vertex Data: " << endl;

	for (int i = 0; i < mesh.head.vertexNum; ++i) {
		if (vertexNormals[i].x == 0.0f && vertexNormals[i].y == 0.0f && vertexNormals[i].z == 0.0f) {
			vertexNormals[i].x = 1.0f; 
			vertexNormals[i].y = 1.0f; 
			vertexNormals[i].z = 1.0f;
		}

		vertexNormals[i] = vertexNormals[i].Normalize();

		mesh.vertices[i].nX = vertexNormals[i].x;
		mesh.vertices[i].nY = vertexNormals[i].y;
		mesh.vertices[i].nZ = vertexNormals[i].z;

		mesh.vertices[i].u = vertexUVWs[i].x;
		mesh.vertices[i].v = vertexUVWs[i].y;

		meshFile << "Vertex-" << i << " " << mesh.vertices[i].x << " " << mesh.vertices[i].y << " " << mesh.vertices[i].z << " " 
			<< mesh.vertices[i].nX << " " << mesh.vertices[i].nY << " " << mesh.vertices[i].nZ << " " 
			<< mesh.vertices[i].u << " " << mesh.vertices[i].v << " " 
			<< mesh.vertices[i].boneID[0] << " " << mesh.vertices[i].blend[0] << " " << mesh.vertices[i].boneID[1] << " " << mesh.vertices[i].blend[1] << endl;
	}

	//输出每个面
	meshFile << endl;
	meshFile << "Index Data: " << endl;
	for (int i = 0; i < mesh.head.triangleNum; ++i) {
		FaceEx * pFace = pMesh->GetFace(i);
		meshFile << "TriangleIndex-" << i << " " << pFace->vert[0] << " " << pFace->vert[1] << " " << pFace->vert[2] << endl;
	}

	meshFile.close();


	// ************************* 写入二进制文件 *************************
	unsigned short* indices = new unsigned short[mesh.head.triangleNum * 3];
	for (int i = 0; i < mesh.head.triangleNum; ++i) {
		FaceEx * pFace = pMesh->GetFace(i);
		indices[i * 3 + 0] = pFace->vert[0];
		indices[i * 3 + 1] = pFace->vert[1];
		indices[i * 3 + 2] = pFace->vert[2];
	}

	ofstream modelFile;
	modelFile.open(m_ModelFileName.c_str(), ios::app | ios::binary);

	modelFile.write((char*)&(mesh.head), sizeof(MaxMeshHead));
	for (int i = 0; i < mesh.head.vertexNum; ++i) {
		modelFile.write((char*)&(mesh.vertices[i]), sizeof(MaxVertex));
	}
	modelFile.write((char*)indices, sizeof(unsigned short)* 3 * mesh.head.triangleNum);

	modelFile.close();
	
	return true;
}

int	MyMaxModelPlug::DoExport(const TCHAR* name,ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options) {
	// This is where the file export operation occur.

	/*if (!suppressPrompts) {
		DialogBoxParam(hInstance,
			MAKEINTRESOURCE(IDD_PANEL),
			GetActiveWindow(),
			MyMaxModelPlugOptionsDlgProc, (LPARAM)this);
	}*/

	IGameConversionManager* pConversionManager = GetConversionManager();
	pConversionManager->SetCoordSystem(IGameConversionManager::IGAME_D3D);
		

	// Construct a tab with all this scene's nodes.
	// We could also only take the node currently selected, etc.
	INodeTab lNodes;
	GetSceneNodes(lNodes);
	
	// Initialise 3DXI (formerly IGame) object
	// For more information, please see 3ds Max SDK topic PG: Programming with 3DXI.
	m_pIGame = GetIGameInterface();
	m_pIGame->InitialiseIGame(lNodes);

	// 获取文件名
	string fileName;
	EraseFileNameSuffix(name, fileName);

	m_ModelFileName = fileName;
	m_BoneFileName = fileName;

	AddFileNameSuffix(m_ModelFileName, ".model");
	AddFileNameSuffix(m_BoneFileName, ".bone");

	// 获取顶层节点个数
	m_TopNodeNum = m_pIGame->GetTopLevelNodeCount();

	// 3ds Max中的骨骼ID是乱序的，导出时会为骨骼分配新的顺序ID，并建立<oldID -> newID>的映射
	if (!ExportBones()) {
		return false;
	}

	// 遍历节点，读取网格数据
	for (int i = 0; i < m_TopNodeNum; i++) {
		IGameNode* pNode = m_pIGame->GetTopLevelNode(i);
		IGameObject* pObj = pNode->GetIGameObject();

		if (pObj->GetIGameType() == IGameObject::IGAME_MESH) {
			if (!ExportMesh(pNode)) {
				return false;
			}
		}
	}
	

	#pragma message	(TODO("Initialize and open your file to write"))
	#pragma message	(TODO("Access IGameScene object to extract useful information."))

	#pragma message	(TODO("return TRUE if the file was exported properly."))
	
	return true;
}


 
