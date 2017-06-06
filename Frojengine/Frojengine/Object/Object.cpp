///////////////////////
//
//  Object.cpp
//
///////////////////////

#include "Object.h"

CObject::CObject()
{
	m_Name = nullptr;

	m_Pos = VECTOR3(0.0f, 0.0f, 0.0f);
	m_Rot = VECTOR3(0.0f, 0.0f, 0.0f);
	m_Scale = VECTOR3(1.0f, 1.0f, 1.0f);

	m_Look = VECTOR3(0.0f, 1.0f, 0.0f);

	XMStoreFloat4x4(&m_PosM, XMMatrixIdentity());
	XMStoreFloat4x4(&m_RotM, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ScaleM, XMMatrixIdentity());
	XMStoreFloat4x4(&m_WorldM, XMMatrixIdentity());

	m_pModel = nullptr;

	m_pParent = nullptr;
	m_Children.clear();
}

CObject::CObject(const CObject& obj)
{

}

CObject::~CObject()
{

}


bool CObject::Create(void(*AddDeleteList)(CObject*), LPCWSTR name, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale, CModel* pModel, CObject* parent)
{
	this->AddDeleteList = AddDeleteList;

	m_Name = name;
	m_Pos = pos;
	m_Rot = rot;
	m_Scale = scale;

	m_pModel = pModel;
	SetParent(parent);

	return true;
}


void CObject::Destroy()
{
	AddDeleteList(this);
}


void CObject::Release()
{
	SetParent(nullptr);

	list<CObject*>::iterator temp;
	for (list<CObject*>::iterator iter = m_Children.begin(); iter != m_Children.end();)
	{
		temp = iter;
		++iter;

		(*temp)->Release();
		delete (*temp);
		(*iter) = nullptr;
	}
}

void CObject::Update(float deltaTime)
{
	MATRIXA mPos;
	mPos = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);

	MATRIXA mRot;
	mRot = XMMatrixRotationRollPitchYaw(m_Rot.x, m_Rot.y, m_Rot.z);

	MATRIXA mScale;
	mScale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	MATRIXA mWorld = mScale * mRot * mPos;

	VECTOR look = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	look = XMVector3TransformCoord(look, mRot);
	XMStoreFloat3(&m_Look, look);

	if (m_pParent != nullptr)
	{
		mWorld *= XMLoadFloat4x4(&m_pParent->m_WorldM);
	}

	XMStoreFloat4x4(&m_PosM, mPos);
	XMStoreFloat4x4(&m_RotM, mRot);
	XMStoreFloat4x4(&m_ScaleM, mScale);
	XMStoreFloat4x4(&m_WorldM, mWorld);

	for (list<CObject*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
	{
		(*iter)->Update(deltaTime);
	}
}



void CObject::Render()
{
	m_pModel->Draw();
}



void CObject::SetParent(CObject* parent)
{
	if (m_pParent != nullptr)
	{
		m_pParent->m_Children.remove(this);
		m_pParent = nullptr;
	}

	m_pParent = parent;
}

void CObject::AddChild(CObject* child)
{
	m_Children.push_back(child);
}

CObject* CObject::GetParent()
{
	return m_pParent;
}

CObject* CObject::GetChild(LPCWSTR childName)
{
	for (list<CObject*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
	{
		if (wcscmp((*iter)->m_Name, childName) == 0)
		{
			return (*iter);
		}
	}

	return nullptr;
}


////////////////////////////////////////////////////////////////////////////// 
//
// 메쉬/기하 정보 읽기
//
bool LoadMesh(void(*AddDeleteList)(CObject*), void(*AddSceneList)(CObject*), LPDEVICE pDevice, LPCWSTR fileName, CObject* o_pObject)
{
	wifstream file(fileName);

	vector<CMesh> meshes;
	vector<XMFLOAT3> vecPos;		// obj 파일의 Position 정보들이 저장되는 vector
	vector<XMFLOAT3> vecTex;		// obj 파일의 Texture 정보들이 저장되는 vector
	vector<XMFLOAT3> vecNor;		// obj 파일의 Normal 정보들이 저장되는 vector
	unordered_map<wstring, int> indexHash;		// 중복되는 index들을 걸러내기 위해 해쉬테이블을 사용
	unordered_map<wstring, int> groupHash;		// 그룹들을 구분하기위해 해쉬테이블을 사용
	wchar_t c;
	wchar_t fileTemp[128];
	wstring groupName;
	wstring str;
	int indexNum = 0;							// 순차적인 index정보를 저장하기 위한 변수
	int groupNum = 0;							// 순차적인 group정보를 저장하기 위한 변수

												// 파일을 제대로 읽었는가
	if (file.fail())
		return false;

	// 제대로 읽었으면 파일의 끝까지 읽는다
	while (!file.eof())
	{
		XMFLOAT3 tempFloat3;
		VertexFormat tempVF;
		int tempFaceNum = 0;

		// 파일의 문장 첫 글자를 읽고
		file.get(c);

		// 그 글자에 따라서 필요한 데이터를 추출

		// vertex에 대한 정보라면
		if (c == 'v')
		{
			// 한글자를 더 읽어서
			file.get(c);
			if (c == ' ') { file >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z; vecPos.push_back(tempFloat3); }		// 공백이 나오면 position값이므로 vecPos에 값을 넣고
			else if (c == 't') { file >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z; vecTex.push_back(tempFloat3); }		// 't'가 나오면 Texture값이므로 vecTex에 값을 넣고
			else if (c == 'n') { file >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z; vecNor.push_back(tempFloat3); }		// 'n'이 나오면 Normal값이므로 vecNor에 값을 넣는다

			while (c != '\n')
			{
				file.get(c);
			}
		}

		// group에 대한 정보라면
		else if (c == 'g')
		{
			file.getline(fileTemp, sizeof(fileTemp));

			bool isDefault = false;
			int sp = 0, ep = 0, i = 0, eof = 0;;

			while (fileTemp[i] != '\0') { i++; }

			eof = i;
			i = 0;

			while (true)
			{
				if (i == eof)
				{
					isDefault = true;
					groupName = L"default";

					if (groupHash.find(groupName) == groupHash.end())
					{
						groupHash.insert(unordered_map<wstring, int>::value_type(groupName, groupNum++));

						CMesh mesh;
						mesh.SetMeshName(groupName.c_str());
						meshes.push_back(mesh);
					}

					break;
				}

				else if (fileTemp[i] != ' ' && fileTemp[i] != '\n' && fileTemp[i] != '\t')
				{

					sp = i;
					break;
				}



				i++;
			}

			i = eof;

			while (!isDefault)
			{
				i--;

				if (fileTemp[i] != ' ' && fileTemp[i] != '\n' && fileTemp[i] != '\t')
				{
					ep = i + 1;

					wchar_t temp[128];

					memcpy_s(temp, sizeof(temp), &fileTemp[sp], (ep - sp) * 2);
					temp[ep - sp] = '\0';
					groupName = temp;

					if (groupHash.find(groupName) == groupHash.end())
					{
						groupHash.insert(unordered_map<wstring, int>::value_type(groupName, groupNum++));

						CMesh mesh;
						mesh.SetMeshName(groupName.c_str());
						meshes.push_back(mesh);
					}

					break;
				}
			}
		}

		// face(Index)에 대한 정보라면
		else if (c == 'f')
		{
#define GROUP_MESH_VERTICES meshes[groupHash[groupName]].m_Vertices
#define GROUP_MESH_INDICES meshes[groupHash[groupName]].m_Indices
			if (groupHash.size() == 0)
			{
				groupName = L"default";
				groupHash.insert(unordered_map<wstring, int>::value_type(L"default", groupNum++));

				CMesh mesh;
				mesh.SetMeshName(groupName.c_str());
				meshes.push_back(mesh);
			}

			// 해당 face의 정보가 pos,tex,nor값이 들어있는지 분석하기위해 StartPoint와 EndPoint를 세팅
			int sp = 0, ep = 0;
			wchar_t dummy[32];
			bool eof = false;

			// 파일을 한줄 읽어서 face에 대한 정보를 가져온 후
			file.getline(fileTemp, sizeof(fileTemp));

			while (!eof)
			{
				{
					int i = 0;
					while (true)
					{
						i++;

						// 공백이 나오면 한 단어의 끝이므로 sp와 ep를 세팅
						if (fileTemp[ep + i] == ' ')
						{
							sp = ep + 1;
							ep = ep + i;
							break;
						}

						// 널문자가 나오면 문장의 끝이므로 sp와 ep를 세팅 후 eof를 true로 바꿈으로써 face 루프를 종료하도록 한다
						else if (fileTemp[ep + i] == '\0')
						{
							sp = ep + 1;
							ep = ep + i;
							eof = true;
							break;
						}
					}

					// i == 1이고 eof가 true라면 읽기위한 남은 데이터가 공백뿐이므로 빠져나간다
					if (i == 1 && eof)
						break;
				}

				// dummy에 ep와 sp를 이용해 index하나를 복사한다
				memcpy_s(dummy, sizeof(dummy), &fileTemp[sp], (ep - sp) * 2);
				dummy[ep - sp] = '\0';

				// hash가 string 형식으로 되어있으므로 string에 대입후
				str = dummy;

				// hash에 존재하지 않는다면
				if (indexHash.find(str) == indexHash.end())
				{
					// hash에 새로운 key(obj파일의 data)와 value(중복되지 않는 index)를 집어넣는다
					indexHash.insert(unordered_map<wstring, int>::value_type(str, indexNum++));

					int i = 0;
					tempVF.position = vecPos[_wtoi(dummy) - 1];		// 첫 숫자는 무조건 position에 대한 인덱스이므로 바로 tempVF에 집어넣고

					while (true)
					{
						// 숫자 다음에 '/'가 온다면 normal이나 texture 값이 존재 하는것이므로
						if (dummy[i] == '/')
						{
							// '/'이 바로 다음에 온다면 다음값이 노말값이 존재하고 texture값은 존재하지 않는것이고
							if (dummy[i + 1] == '/')
							{
								tempVF.normal = vecNor[_wtoi(&dummy[i + 2]) - 1];
								break;
							}

							else
							{
								// '/'가 없다면 texture에 대한 숫자가 존재하므로 texture에 대한 index를 tempVF에 집어넣고
								tempVF.texture = vecTex[_wtoi(&dummy[i + 1]) - 1];

								while (true)
								{
									i++;

									// '/'이 한번 더 나온다면 그다음 값은 normal값으로
									if (dummy[i] == '/')
									{
										// normal값에 대한 index를 tempVF에 집어넣는다
										tempVF.normal = vecNor[_wtoi(&dummy[i + 1]) - 1];
										break;
									}

									// '\0'이 나왔다면 문자의 끝이므로 normal값은 존재하지 않으므로 반복문을 종료한다
									else if (dummy[1] == '\0')
									{
										break;
									}
								}
								break;
							}
						}

						else if (dummy[i] == '\0')
							break;

						i++;
					}

					GROUP_MESH_VERTICES.push_back(tempVF);
				}

				// 한 face안의 index개수를 증가시킨다
				tempFaceNum++;

				// 만약 한 face안에 index가 4개라면 사각형을 의미하므로 삼각형 2개로 만들기 위해 index 2개를 더 추가해준다
				if (tempFaceNum == 4)
				{
					GROUP_MESH_INDICES.push_back(GROUP_MESH_INDICES[GROUP_MESH_INDICES.size() - 3]);
					GROUP_MESH_INDICES.push_back(GROUP_MESH_INDICES[GROUP_MESH_INDICES.size() - 2]);
					GROUP_MESH_INDICES.push_back(indexHash[dummy]);
				}

				else
					GROUP_MESH_INDICES.push_back(indexHash[dummy]);
			}
		}
		else
		{
			while (c != '\n')
			{
				file.get(c);
			}
		}
	}

	if (meshes.size() == 0)
	{
		return false;
	}

	else if (meshes.size() == 1)
	{
		CModel* model = new CModel;
		model->Create(pDevice, &meshes[0], nullptr);

		CObject* obj = new CObject;
		obj->Create(AddDeleteList, meshes[0].m_Name, VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(1.0f, 1.0f, 1.0f), model);

		AddSceneList(obj);

		o_pObject = obj;
	}

	else
	{
		CObject* parent = new CObject;
		parent->Create(AddDeleteList, fileName, VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(1.0f, 1.0f, 1.0f));

		AddSceneList(parent);

		for (UINT i = 0; i < meshes.size(); i++)
		{
			CModel* model = new CModel;
			model->Create(pDevice, &meshes[i], nullptr);

			CObject* obj = new CObject;
			obj->Create(AddDeleteList, meshes[i].m_Name, VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(1.0f, 1.0f, 1.0f), model, parent);

			AddSceneList(obj);
		}

		o_pObject = parent;
	}

	return true;
}