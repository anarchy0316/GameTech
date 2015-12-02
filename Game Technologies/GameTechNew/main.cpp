#include <nclgl\Window.h>
#include "UT_TaskScheduler.h"
#include "MyScene.h"
#include <ncltech\PhysicsEngine.h>

Scene* scene = NULL;

int Quit(bool pause = false, const string &reason = "") {	
	if (scene)
	{
		delete scene;
		scene = NULL;
	}

	Window::Destroy();

	if (pause) {
		std::cout << reason << std::endl;
		system("PAUSE");
	}

	return 0;
}

int main()
{
	//------------------
	//--- UNIT TESTS ---
	//------------------

	//Uncomment for TaskScheduler Unit Test 
	//UT_TaskScheduler::UT_TaskScheduler();



	//-------------------
	//--- MAIN ENGINE ---
	//-------------------

	//Initialise the Window
	if (!Window::Initialise("Game Technologies", 1280, 800, false))
	{
		return Quit(true, "Window failed to initialise!");
	}
	//Window::GetWindow().ShowOSPointer(false);
	//Window::GetWindow().LockMouseToWindow(true);

	//Initialise the PhysicsEngine
	PhysicsEngine::Instance();

	//Initialise the Scene
	scene = new MyScene(Window::GetWindow());
	if (!scene->HasInitialised())
	{
		return Quit(true, "Renderer failed to initialise!");
	}
	int i = 0;
	//Create main game-loop
	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float msec = Window::GetWindow().GetTimer()->GetTimedMS();	//How many milliseconds since last update?



		scene->UpdateScene(msec);
		scene->RenderScene();	

		PhysicsEngine::Instance()->Update(msec * 0.001f);
	}

	//Cleanup
	return Quit();
}