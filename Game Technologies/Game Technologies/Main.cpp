/******************************************************************************
Class:
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: We have a revised 'main' file this time around. The old main file
was sometimes quite hard to debug when classes failed to initialise, so I 
have made a dedicated 'Quit' function, which will destroy everything, and 
will halt and post a quit message if anything has gone wrong. The Window class
now also brings the console window into focus ini ts destructor, so that we
can immediately see which class has caused initialisation to end. 

You may like to take this further and have each class initialised in the 
main file to have a number of 'error strings' to output in this function in 
order to quickly track down what has gone wrong - it shouldn't be too hard to
implement this, so I'll leave it up to you!

Note that this time the Window is a single class, instantiated and destroyed
via static functions. Noone ever used the ability to have multiple renderers
and windows to good effect, and it made the code more complex, so I have 
removed these abilities in the code download for this module! The less things
that can go wrong, the more time you potentially have to have fun with physics!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#include <thread>
#include "../../nclgl/Window.h"
#include "MyGame.h"

#pragma comment(lib, "nclgl.lib")

int Quit(bool pause = false, const string &reason = "") {
	PhysicsSystem::Destroy();
	Window::Destroy();
	Renderer::Destroy();

	if(pause) {
		std::cout << reason << std::endl;
		system("PAUSE");
	}

	return 0;
}

void physicsLoop(GameClass* game, bool& running) {
	GameTimer timer;
	while (running) {
		float msec = timer.GetTimedMS();
		game->UpdatePhysics(msec);
	}
}

int main() {
	if(!Window::Initialise("Game Technologies", 1280,800,false)) {
		return Quit(true, "Window failed to initialise!");
	}

	if(!Renderer::Initialise()) {
		return Quit(true, "Renderer failed to initialise!");
	}

	PhysicsSystem::Initialise();

	MyGame* game = new MyGame();

	Window::GetWindow().LockMouseToWindow(true);
	Window::GetWindow().ShowOSPointer(false);

	bool running = true;
	std::thread physics(physicsLoop, game, std::ref(running));
	while(Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float msec = Window::GetWindow().GetTimer()->GetTimedMS();	//How many milliseconds since last update?

		game->UpdateRendering(msec);	//Update our 'sybsystem' logic (renderer and physics!)
		game->UpdateGame(msec);	//Update our game logic
	}
	running = false;
	physics.join();

	delete game;	//Done with our game now...bye bye!
	return Quit();
}

