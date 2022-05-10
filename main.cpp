#include "JPorta.h"

#include "Shape.h"
#include "Scene.h"

#ifndef JPORTA
#include <SDL.h>
#undef main
#include <SDL_image.h>
#endif // !JPORTA

int main(void) {
	{
	// set up scene
	Scene scene;
#ifndef JPORTA
	scene.SetBackgroundColor({ 89, 116, 117, 255 });
#endif // !JPORTA
	scene.Load();

#ifndef JPORTA
	// create new shapes:
	Shape* newShape = nullptr;
	Vec2 origo, point;
	Mouse::OnClick(MouseButton::RIGHT, [&]() {
		origo = Mouse::GetPosition();
		
		switch (scene.GetNewShapeType()) {
		case SQUARE: newShape = new Square(origo, origo); break;
		case TRIANGLE: newShape = new Triangle(origo, origo); break;
		case CIRCLE: newShape = new Circle(origo, origo); break;
		default: break;
		}

		Mouse::OnRelease(MouseButton::RIGHT, [&]() {
			scene.AddShape(newShape);
			newShape = nullptr;
		}, false);
	});

	// delete shape
	Mouse::OnClick(MouseButton::MIDDLE, [&]() {
		scene.ForEachShape([&](Shape* shape) {
			if (shape->IsPointInside(Mouse::GetPosition())) {
				scene.RemoveShape(shape);
			}
		});
	});
#endif // JPORTA


#ifndef JPORTA
	SDL_Texture** selectionTextures = new SDL_Texture*[3];
	selectionTextures[SQUARE] = IMG_LoadTexture(scene.GetRenderer(), "./textures/square_selected2.png");
	selectionTextures[TRIANGLE] = IMG_LoadTexture(scene.GetRenderer(), "./textures/triangle_selected2.png");
	selectionTextures[CIRCLE] = IMG_LoadTexture(scene.GetRenderer(), "./textures/circle_selected2.png");

	// main loop
	while (scene.IsRunning()) {
		scene.Update();

		SDL_Rect srcRect = { 0, 0, 112, 32 };
		SDL_Rect dst = { 4, 4, 168, 48 };
		SDL_RenderCopy(scene.GetRenderer(), selectionTextures[scene.GetNewShapeType()], &srcRect, &dst);

		// show the shape that is currently being created
		if (newShape != nullptr) {
			newShape->SetPoint(Mouse::GetPosition());
			newShape->draw(scene.GetRenderer());
		}
	}

	for (size_t i = 0; i < 3; i++) SDL_DestroyTexture(selectionTextures[i]);
	delete[] selectionTextures;
#endif // !JPORTA

	scene.Save();
	}

	return 0;
}
