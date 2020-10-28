// SDL Tutorial 5 - main.cpp : starter for lab.
// Collision Detection
#include <string>
#include <stdio.h>
#include <SDL.h>

using namespace std;

#define SAFERELEASE(x) { if (x) { delete x; x = NULL;} }

SDL_Window* window = NULL;
SDL_Surface* renderFrame = NULL;

//[3] Create an enumeration which lists the
//keys used in this tutorial.
enum KeyStates {
	LEFT = 0,
	RIGHT,
	UP,
	DOWN,
	KEYSTATES_COUNT
};


//[4] Create an array to store key states
//each key will be set to true if pressed
bool keyStates[KeyStates::KEYSTATES_COUNT];

//[5] Create a sphere structure to handle collisions
struct Sphere {
	friend class Sprite;
	Sphere(int x, int y, int radius) : x(x), y(y), radius(radius) ;
	bool CollisionWithSphere(Sphere& s) const {
		int diffX = s.x > x ? s.x - x : x - s.x;
		int diffY = s.y > y ? s.y - y : y - s.y;
		int vectorLength = sqrt(diffX * diffX + diffY * diffY);
		return vectorLength < (radius + s.radius);
		return false;
	}
private:
	int x, y;
	int radius;
};

//[6] Create a sprite structure which implements
//SDLs functionality of loading and drawing images
//along with collision detection
struct Sprite {
public:
	Sprite(string file) {
		this->surface = SDL_LoadBMP(file.c_str());
		if (this->surface == NULL)
		{
			printf("Unable to load image %s! SDL Error: %s\n", file,
				SDL_GetError());
		}
		this->rect.x = 0;
		this->rect.y = 0;
		this->rect.w = surface->w;
		this->rect.h = surface->h;
		boundingSphere = new Sphere(rect.x + (rect.w / 2),
			rect.y + (rect.h / 2), rect.w / 2);
		visible = true;
	}

	virtual ~Sprite() {
		SDL_FreeSurface(this->surface);
		SAFERELEASE(boundingSphere);
	}

	void SetPosition(int x, int y) {
		this->rect.x = x;
		this->rect.y = y;
		this->boundingSphere->y = y;
	}

	void Render() {
		if (this->visible)
			SDL_BlitSurface(surface, NULL, renderFrame, &rect);
	}

	bool SphereCollision(Sprite& other) {
		return boundingSphere->CollisionWithSphere(*other.boundingSphere);
	}

	void SetVisible(bool visible) {
		this->visible = visible;
	}

	int getX() {
		return rect.x;
	}

	int getY() {
		return rect.y;
	}

private:
	Sphere* boundingSphere;
	SDL_Surface* surface;
	SDL_Rect rect;
	bool visible;
};

//[7] Create two instances of sprites
Sprite* ship;
Sprite* burst;

void Initialise() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return;
	}
	else {
		window = SDL_CreateWindow("SDL Tutorial 2 - Input", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 768, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return;
		}
		else {
			renderFrame = SDL_GetWindowSurface(window);
		}
	}

}

void LoadContent() {
	//[8] load in the two sprites. Set the
	// ships position and the burst to not draw.
	ship = new Sprite("ship.bmp");
	burst = new Sprite("burst.bmp");
	ship->SetPosition(1280 / 2, 768 / 2);
	burst->SetVisible(false);
}

bool Update() {
	
	SDL_Event e;

	//[9] implement a loop to check all SDL events
	while (SDL_PollEvent(&e) != 0) {
	while (true) {
		
		//[10] Handle the X button at top right
		//of the window
		if (e.type == SDL_QUIT) {
			return false;
		}
		
		//[11] Detect keydown events
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_LEFT:
				keyStates[LEFT] = true;
				break;
			case SDLK_RIGHT:
				keyStates[RIGHT] = true;
				break;
			case SDLK_UP:
				keyStates[UP] = true;
				break;
			case SDLK_DOWN:
				keyStates[DOWN] = true;
				break;
			}

		}

		//[12] Detect key up events
		
		if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_LEFT:
				keyStates[LEFT] = false;
				break;
			case SDLK_RIGHT:
				keyStates[RIGHT] = false;
				break;
			case SDLK_UP:
				keyStates[UP] = false;
				break;
			case SDLK_DOWN:
				keyStates[DOWN] = false;
				break;
			}
		}

		//[13] detect mouse events
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			Sint32 mx = e.motion.x;
			Sint32 my = e.motion.y;
			printf("Mouse pressed at position %i , %i \n", mx, my);
			burst->SetPosition(mx, my);
			burst->SetVisible(true);
		}
		
	}

	//[14] Move the ship when arrow keys are pressed
	if (keyStates[UP]) {
		ship->SetPosition(ship->getX(), ship->getY() - 5);
	}
	if (keyStates[DOWN]) {
		ship->SetPosition(ship->getX(), ship->getY() + 5);
	}
	if (keyStates[LEFT]) {
		ship->SetPosition(ship->getX() - 5, ship->getY());
	}
	if (keyStates[RIGHT]) {
		ship->SetPosition(ship->getX() + 5, ship->getY());
	}

	//[15] Check for collision between ship and burst
	if (ship->SphereCollision(*burst)) {
		burst->SetVisible(false);
	}
	
	return true;
}

	void Draw(); {
	//clear screen to cornflower blue
	SDL_FillRect(renderFrame, NULL, 0x6495ed);

	//[16] render the ship and burst
	burst->Render();
	ship->Render();

	SDL_UpdateWindowSurface(window);
}

	void UnloadContent(); {
	//[17] Release ship and burst sprites
	SAFERELEASE(ship);
	SAFERELEASE(burst);
}

	void Exit(); {
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
}


	int main(int argc, char* argv[])
		; {
	Initialise();

	LoadContent();

	bool gameLoop = true;

	while (gameLoop) {
		gameLoop = Update();
		Draw();
		SDL_Delay(16);//60fps
	}

	UnloadContent();

	Exit();

    return 0;
}