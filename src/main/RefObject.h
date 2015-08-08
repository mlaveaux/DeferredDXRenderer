#ifndef REFOBJECT_H
#define REFOBJECT_H

#include "main/GlobalFunctions.h"

class RefObject
{
public:
	RefObject()
	{
		m_refCounter = new unsigned int(1);
	}
	~RefObject()
	{
		*m_refCounter--;

		if(*m_refCounter == 0)
		{
			SAFE_DELETE(m_refCounter);
			release();
		}
	}
protected:
	virtual void release() = 0;
private:
	unsigned int* m_refCounter;
};


class RefContainer
{

};

#endif