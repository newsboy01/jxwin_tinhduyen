// S3PRoleList.cpp: implementation of the S3PRoleList class.
//
//////////////////////////////////////////////////////////////////////

#include "S3PRoleList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

S3PRoleList::S3PRoleList(std::string cAccName) : 
	S3PTableObjList<S3PRole>("Role_Info", "iid", "cAccName", cAccName)
{
}

S3PRoleList::~S3PRoleList()
{

}
