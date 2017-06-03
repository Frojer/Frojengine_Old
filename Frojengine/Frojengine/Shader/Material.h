#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class CMaterial
{


public:
	CMaterial();
	CMaterial(const CMaterial& obj);
	~CMaterial();

	bool Create();
	void Release();
};

#endif