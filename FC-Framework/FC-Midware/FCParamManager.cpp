#include "FCParamManager.h"

static const char NAME[20] = "FCParamManager";
static const char VERSION[20] = "V0001";
static FCParamManager* mInstance = NULL;

FCParamManager::FCParamManager():
	VModule(NAME, VERSION)
{
}

FCParamManager::~FCParamManager()
{

}

FCParamManager* FCParamManager::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCParamManager();
	}
	return mInstance;
}

void FCParamManager::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

void FCParamManager::TaskCustomCallback(void const * argument)
{

}
