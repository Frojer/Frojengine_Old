#pragma once
#ifndef _temp_
#define _temp_

class temp
{


public:
	temp();
	temp(const temp& obj);
	~temp();

	bool Create();
	void Release();
};

#endif