#pragma once
#ifndef _SHADER_MANAGER_H_
#define _SHADER_MANAGER_H_

class CShaderManager
{
private:
	static unordered_map<wstring, CShader*> shaderMap;
	
public:
	CShaderManager();
	CShaderManager(const CShaderManager& obj);
	~CShaderManager();

	bool Create(LPDEVICE pDevice);

	static CShader* GetShader(wstring shaderName);
	void Release();
};

#endif