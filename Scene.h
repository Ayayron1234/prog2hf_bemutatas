#pragma once
#include "JPorta.h"

#include <fstream>
#include "Shape.h"
#include "Vector.h"

#ifndef JPORTA
#include "Input.h"
#endif // !JPORTA

#ifndef JPORTA
class Window {
	bool* isRunning;
	ShapeType* newShapeType;

	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Color backgroundColor;

	void initSDL() {
		int flags = SDL_WINDOW_RESIZABLE | SDL_RENDERER_PRESENTVSYNC | SDL_WINDOW_ALLOW_HIGHDPI;

		// Initialise SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			SDL_Log("SDL cound not be initialised: %s\n", SDL_GetError());
			return;
		}

		// Create window
		window = SDL_CreateWindow("test window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 480, flags);
		if (window == NULL)
		{
			SDL_Log("SDL couldn't open %s window: %s\n", "test window", SDL_GetError());
			return;
		}

		// Create rendering context
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == NULL)
		{
			SDL_Log("SDL couldn't create rendering context: %s", SDL_GetError());
			return;
		}
		Shape::debugRenderer = renderer;
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderClear(renderer);
	}

	void handleSDLEvents() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT: {
				*isRunning = false;
			} break;

				// handle mouse events
			case SDL_MOUSEBUTTONDOWN: {
				Mouse::setButton((int)event.button.button);
			}
			case SDL_MOUSEBUTTONUP: {
				if (event.button.type == SDL_MOUSEBUTTONUP)
					Mouse::clearButton(event.button.button);
			} break;

				// handle keyboard events
			case SDL_KEYDOWN: {
				if (event.key.keysym.sym == SDLK_1) *newShapeType = SQUARE;
				if (event.key.keysym.sym == SDLK_2) *newShapeType = TRIANGLE;
				if (event.key.keysym.sym == SDLK_3) *newShapeType = CIRCLE;
			} break;
			case SDL_KEYUP: {

			}
			default:
				break;
			}
		}
	}

	void render() {
		SDL_RenderPresent(renderer);

		SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
		SDL_RenderClear(renderer);
	}

public:
	Window(bool* runStatePointer, ShapeType* newShapeTypePtr) 
		: isRunning(runStatePointer) 
		,backgroundColor({ 80, 80, 80, 255 }),
		newShapeType(newShapeTypePtr)
	{
		initSDL();
	}

	void Update() {
		render();
		handleSDLEvents();
	}

	~Window() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	friend class Scene;
};
#endif // !JPORTA

class Scene {
#ifndef JPORTA
	Window window;

	ShapeType newShapeType;
#endif // !JPORTA
	bool isRunning;

	Vector<Shape*> shapes;

public:
	Scene() : isRunning(true)
#ifndef JPORTA
		, window(&isRunning, &newShapeType),
		newShapeType(SQUARE)
#endif // !JPORTA
	{}

	bool IsRunning() {
		return isRunning;
	}

#ifndef JPORTA
	void SetBackgroundColor(SDL_Color color) {
		window.backgroundColor = color;
	}

	SDL_Renderer* GetRenderer() {
		return window.renderer;
	}

	ShapeType& GetNewShapeType() {
		return newShapeType;
	}
#endif // !JPORTA

	void AddShape(Shape* shape) {
		shapes.push_back(shape);
	}

	void RemoveShape(Shape* shape) {
		Shape* tmp = shape;
		for (unsigned i = 0; i < shapes.size(); i++)
			if (shapes.at(i) == shape) {
				shapes.splice(i, 1);
				break;
			}
		delete tmp;
	}

	void Quit() {
		isRunning = false;
	}

#ifndef JPORTA
	void ForEachShape(std::function<void(Shape*)> callback) {
		for (unsigned i = 0; i < shapes.size(); i++)
			callback(shapes.at(i));
	}
	void ForEachShape(std::function<void(Shape*, unsigned)> callback) {
		for (unsigned i = 0; i < shapes.size(); i++)
			callback(shapes.at(i), i);
	}
#endif // !JPORTA

	void Save() {
		std::ofstream file("save.txt");
		
		for (size_t i = 0; i < shapes.size(); i++)
			shapes.at(i)->Save(file);

		file.close();
	}

	void Load() {
		std::ifstream file("save.txt");

		size_t type;
		Vec2 origo, point;
		while (file >> type >> origo.x >> origo.y >> point.x >> point.y) {
			switch (type)
			{
			case 1: { 
				std::cout << "Square(";
				AddShape(new Square(origo, point)); 
			} break;
			case 2: {
				std::cout << "Triangle(";
				AddShape(new Triangle(origo, point));
			} break;
			case 3: {
				std::cout << "Circle(";
				AddShape(new Circle(origo, point));
			}
			default:
				break;
			}
			std::cout << "origo(" << origo.x << ", " << origo.y << "), point(" << point.x << ", " << point.y << "));" << std::endl;
		}

		file.close();
	}

	void Update() {
#ifndef JPORTA
		ForEachShape([&](Shape* shape) {
			shape->Update();
			shape->draw(GetRenderer());
			});

		window.Update();
#endif // !JPORTA
	}

	~Scene() {
		for (size_t i = 0; i < shapes.size(); i++) {
			delete shapes.at(i);
		}
	}
};
