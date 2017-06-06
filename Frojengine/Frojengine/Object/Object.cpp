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
// �޽�/���� ���� �б�
//
bool LoadMesh(void(*AddDeleteList)(CObject*), void(*AddSceneList)(CObject*), LPDEVICE pDevice, LPCWSTR fileName, CObject* o_pObject)
{
	wifstream file(fileName);

	vector<CMesh> meshes;
	vector<XMFLOAT3> vecPos;		// obj ������ Position �������� ����Ǵ� vector
	vector<XMFLOAT3> vecTex;		// obj ������ Texture �������� ����Ǵ� vector
	vector<XMFLOAT3> vecNor;		// obj ������ Normal �������� ����Ǵ� vector
	unordered_map<wstring, int> indexHash;		// �ߺ��Ǵ� index���� �ɷ����� ���� �ؽ����̺��� ���
	unordered_map<wstring, int> groupHash;		// �׷���� �����ϱ����� �ؽ����̺��� ���
	wchar_t c;
	wchar_t fileTemp[128];
	wstring groupName;
	wstring str;
	int indexNum = 0;							// �������� index������ �����ϱ� ���� ����
	int groupNum = 0;							// �������� group������ �����ϱ� ���� ����

												// ������ ����� �о��°�
	if (file.fail())
		return false;

	// ����� �о����� ������ ������ �д´�
	while (!file.eof())
	{
		XMFLOAT3 tempFloat3;
		VertexFormat tempVF;
		int tempFaceNum = 0;

		// ������ ���� ù ���ڸ� �а�
		file.get(c);

		// �� ���ڿ� ���� �ʿ��� �����͸� ����

		// vertex�� ���� �������
		if (c == 'v')
		{
			// �ѱ��ڸ� �� �о
			file.get(c);
			if (c == ' ') { file >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z; vecPos.push_back(tempFloat3); }		// ������ ������ position���̹Ƿ� vecPos�� ���� �ְ�
			else if (c == 't') { file >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z; vecTex.push_back(tempFloat3); }		// 't'�� ������ Texture���̹Ƿ� vecTex�� ���� �ְ�
			else if (c == 'n') { file >> tempFloat3.x >> tempFloat3.y >> tempFloat3.z; vecNor.push_back(tempFloat3); }		// 'n'�� ������ Normal���̹Ƿ� vecNor�� ���� �ִ´�

			while (c != '\n')
			{
				file.get(c);
			}
		}

		// group�� ���� �������
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

		// face(Index)�� ���� �������
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

			// �ش� face�� ������ pos,tex,nor���� ����ִ��� �м��ϱ����� StartPoint�� EndPoint�� ����
			int sp = 0, ep = 0;
			wchar_t dummy[32];
			bool eof = false;

			// ������ ���� �о face�� ���� ������ ������ ��
			file.getline(fileTemp, sizeof(fileTemp));

			while (!eof)
			{
				{
					int i = 0;
					while (true)
					{
						i++;

						// ������ ������ �� �ܾ��� ���̹Ƿ� sp�� ep�� ����
						if (fileTemp[ep + i] == ' ')
						{
							sp = ep + 1;
							ep = ep + i;
							break;
						}

						// �ι��ڰ� ������ ������ ���̹Ƿ� sp�� ep�� ���� �� eof�� true�� �ٲ����ν� face ������ �����ϵ��� �Ѵ�
						else if (fileTemp[ep + i] == '\0')
						{
							sp = ep + 1;
							ep = ep + i;
							eof = true;
							break;
						}
					}

					// i == 1�̰� eof�� true��� �б����� ���� �����Ͱ� ������̹Ƿ� ����������
					if (i == 1 && eof)
						break;
				}

				// dummy�� ep�� sp�� �̿��� index�ϳ��� �����Ѵ�
				memcpy_s(dummy, sizeof(dummy), &fileTemp[sp], (ep - sp) * 2);
				dummy[ep - sp] = '\0';

				// hash�� string �������� �Ǿ������Ƿ� string�� ������
				str = dummy;

				// hash�� �������� �ʴ´ٸ�
				if (indexHash.find(str) == indexHash.end())
				{
					// hash�� ���ο� key(obj������ data)�� value(�ߺ����� �ʴ� index)�� ����ִ´�
					indexHash.insert(unordered_map<wstring, int>::value_type(str, indexNum++));

					int i = 0;
					tempVF.position = vecPos[_wtoi(dummy) - 1];		// ù ���ڴ� ������ position�� ���� �ε����̹Ƿ� �ٷ� tempVF�� ����ְ�

					while (true)
					{
						// ���� ������ '/'�� �´ٸ� normal�̳� texture ���� ���� �ϴ°��̹Ƿ�
						if (dummy[i] == '/')
						{
							// '/'�� �ٷ� ������ �´ٸ� �������� �븻���� �����ϰ� texture���� �������� �ʴ°��̰�
							if (dummy[i + 1] == '/')
							{
								tempVF.normal = vecNor[_wtoi(&dummy[i + 2]) - 1];
								break;
							}

							else
							{
								// '/'�� ���ٸ� texture�� ���� ���ڰ� �����ϹǷ� texture�� ���� index�� tempVF�� ����ְ�
								tempVF.texture = vecTex[_wtoi(&dummy[i + 1]) - 1];

								while (true)
								{
									i++;

									// '/'�� �ѹ� �� ���´ٸ� �״��� ���� normal������
									if (dummy[i] == '/')
									{
										// normal���� ���� index�� tempVF�� ����ִ´�
										tempVF.normal = vecNor[_wtoi(&dummy[i + 1]) - 1];
										break;
									}

									// '\0'�� ���Դٸ� ������ ���̹Ƿ� normal���� �������� �����Ƿ� �ݺ����� �����Ѵ�
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

				// �� face���� index������ ������Ų��
				tempFaceNum++;

				// ���� �� face�ȿ� index�� 4����� �簢���� �ǹ��ϹǷ� �ﰢ�� 2���� ����� ���� index 2���� �� �߰����ش�
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