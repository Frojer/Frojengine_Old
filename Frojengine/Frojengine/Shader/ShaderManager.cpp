#include "..\Frojengine.h"

unordered_map<wstring, CShader*> CShaderManager::shaderMap;

CShaderManager::CShaderManager()
{

}

CShaderManager::CShaderManager(const CShaderManager& obj)
{

}

CShaderManager::~CShaderManager()
{

}



bool CShaderManager::Create(LPDEVICE pDevice)
{
	m_pDevice = pDevice;
	m_pDevice->GetImmediateContext(&m_pDXDC);

	if (!CreateShader(L"fx/defaultShader.fx"))
		return false;

	if (!CreateShader(L"fx/defaultLineShader.fx"))
		return false;

	return true;
}



bool CShaderManager::CreateShader(LPDEVICE pDevice, LPCWSTR fileName)
{
	bool result;

	CShader* pShader = new CShader;

	result = pShader->Create(pDevice, fileName);

	if (!result)
	{
		pShader->Release();
		delete pShader;
		return false;
	}

	wstring name = fileName;
	shaderMap.insert(unordered_map<wstring, CShader*>::value_type(name, pShader));

	return true;
}



CShader* CShaderManager::GetShader(wstring shaderName)
{
	return shaderMap[shaderName];
}



void CShaderManager::Release()
{
	unordered_map<wstring, CShader*>::iterator temp;
	for (unordered_map<wstring, CShader*>::iterator iter = shaderMap.begin(); iter != shaderMap.end();)
	{
		temp = iter;
		iter++;

		(*temp).second->Release();
		delete (*temp).second;
	}
}