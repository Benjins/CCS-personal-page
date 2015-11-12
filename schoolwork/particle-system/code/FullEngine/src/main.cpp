#include "../header/int/Scene.h"
#include "../header/int/EditorScene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Mat4.h"
#include "../header/int/PhysicsSim.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/Collider.h"
#include "../test/testing.h"
#include "../header/int/Vector4.h"
#include <iomanip>

#include "../header/int/SourceParser.h"

#if defined(MEM_CHECK) || defined(TESTING)
#if defined(_WIN32) || defined(_WIN64)
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <crtdbg.h>
#endif
#endif
#if defined(MEM_CHECK) || defined(TESTING)
#  include <assert.h>
#endif

#include <iostream>
using std::cout; using std::endl;

int main(int argc, char** argv){

#if (defined(_WIN32) || defined(_WIN64)) && (defined(TESTING) || defined(MEM_CHECK))
	//Windows memory leak checking
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

#ifdef TESTING 
	int retVal = RunAllTests();
#else

#if defined(ASSETS)
	if(argc < 2){
		cout << "\nError: Please supply name of asset file.\n";
		return -1;
	}
	ProcessAssetFile(string(argv[1]));

	return 0;
#endif

#if defined(INTROSPECTION)
	ParseSourceFiles(&argv[1], argc-1);

	return 0;
#endif

#if !defined(EDITOR)
	Scene& x = Scene::getInstance(argc, argv);
#else
	Scene& x = EditorScene::getInstance(argc, argv);
#endif

	x.Init();
	x.LoadScene("Editor_Scene.xml");

	x.Start();

#endif

#if (defined(_WIN32) || defined(_WIN64)) && (defined(MEM_CHECK) || defined(TESTING))
	assert(_CrtCheckMemory());
#endif

#ifdef TESTING 
	return retVal;
#else

	return 0;
#endif
}