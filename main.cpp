#include "JPorta.h"

#include "Shape.h"
#include "Scene.h"

#ifndef JPORTA
#include <SDL.h>
#undef main
#include <SDL_image.h>
#endif // !JPORTA

#include <sstream>
#include "gtest_lite.h"
#include "memtrace.h"

#ifdef JPORTA
int main(void) {
	TEST(Vec2, all) {
		Vec2 a(10, 10);
		Vec2 b(20, 20);

		EXPECT_EQ(30, (a + b).x());
		EXPECT_EQ(-10, (a - b).x());
		EXPECT_DOUBLE_EQ(M_PI / 4, a.GetAngle());
		Vec2 c(3, 4);
		EXPECT_EQ(5, c.GetLength());
		EXPECT_EQ(5, c.GetLength());
	} END

	TEST(Segment, all) {
		Vec2 A(0, 0); Vec2 B(10, 0); Vec2 C(10, 10); Vec2 D(0, 10);
		Segment AC(A, C);
		Segment BD(B, D);
		EXPECT_EQ(true, AC.IsOverlappingWithSegment(BD));

		Segment AD(A, D);
		Segment BC(B, C);
		EXPECT_EQ(false, AD.IsOverlappingWithSegment(BC));
	} END

	TEST(Shape, IsPointInside) {
		Square square(Vec2(0, 0), Vec2(100, 100));
		EXPECT_EQ(true, square.IsPointInside(Vec2(50, 50)));
		EXPECT_EQ(false, square.IsPointInside(Vec2(150, 150)));

		Triangle triangle(Vec2(0, 0), Vec2(100, 100));
		EXPECT_EQ(true, triangle.IsPointInside(Vec2(50, 50)));
		EXPECT_EQ(false, triangle.IsPointInside(Vec2(150, 150)));

		Circle circle(Vec2(0, 0), Vec2(100, 100));
		EXPECT_EQ(true, circle.IsPointInside(Vec2(50, 50)));
		EXPECT_EQ(false, circle.IsPointInside(Vec2(150, 150)));
	} END

	TEST(Shape, IsOverlappingShape) {
		class InvalidShape : public Shape {
		public:
			InvalidShape() : Shape(Vec2(), Vec2()) {}
			Vector<Segment> GetSegments() {
				Vector<Segment> vec;
				vec.push_back(Segment());
				vec.push_back(Segment());
				return vec;
			}
			bool IsOverlappingShape(Shape* shape) { return false; }
			bool IsPointInside(const Vec2) { return false; }
			std::ostream& Save(std::ostream& os) { return os; }
		};

		Square square1(Vec2(0, 0), Vec2(100, 100));
		Triangle triangle1(Vec2(0, 0), Vec2(100, 100));
		Circle circle1(Vec2(0, 0), Vec2(100, 100));

		InvalidShape inv;
		EXPECT_EQ(false, inv.IsOverlappingShape(&square1));
		EXPECT_EQ(false, inv.IsPointInside(Vec2()));
		EXPECT_EQ(false, square1.IsOverlappingShape(&inv));
		EXPECT_EQ(false, triangle1.IsOverlappingShape(&inv));
		EXPECT_EQ(false, circle1.IsOverlappingShape(&inv));
		std::stringstream oss;
		inv.Save(oss);
		EXPECT_STREQ("", oss.str().c_str());

		Square square2(Vec2(0, 0), Vec2(100, 100));
		Triangle triangle2(Vec2(0, 0), Vec2(100, 100));
		Circle circle2(Vec2(0, 0), Vec2(100, 100));

		EXPECT_EQ(true, square1.IsOverlappingShape(&square2));
		EXPECT_EQ(true, square1.IsOverlappingShape(&triangle1));
		EXPECT_EQ(true, square1.IsOverlappingShape(&circle1));

		EXPECT_EQ(true, triangle1.IsOverlappingShape(&square1));
		EXPECT_EQ(true, triangle1.IsOverlappingShape(&triangle2));
		EXPECT_EQ(true, triangle1.IsOverlappingShape(&circle1));

		EXPECT_EQ(true, circle1.IsOverlappingShape(&square1));
		EXPECT_EQ(true, circle1.IsOverlappingShape(&triangle1));
		EXPECT_EQ(true, circle1.IsOverlappingShape(&circle2));

		square2.MoveTo(Vec2(500, 500));
		triangle2.MoveTo(Vec2(500, 500));
		circle2.MoveTo(Vec2(500, 500));

		EXPECT_EQ(false, square1.IsOverlappingShape(&square2));
		EXPECT_EQ(false, square1.IsOverlappingShape(&triangle2));
		EXPECT_EQ(false, square1.IsOverlappingShape(&circle2));

		EXPECT_EQ(false, triangle1.IsOverlappingShape(&square2));
		EXPECT_EQ(false, triangle1.IsOverlappingShape(&triangle2));
		EXPECT_EQ(false, triangle1.IsOverlappingShape(&circle2));
		
		EXPECT_EQ(false, circle1.IsOverlappingShape(&square2));
		EXPECT_EQ(false, circle1.IsOverlappingShape(&triangle2));
		EXPECT_EQ(false, circle1.IsOverlappingShape(&circle2));
	} END

	TEST(Shape, MoveTo) {
		Square square(Vec2(0, 0), Vec2(100, 100));
		square.MoveTo(Vec2(50, 50));
		EXPECT_EQ(50, square.GetOrigo().x());
		EXPECT_EQ(150, square.GetPoint().x());
	} END

	TEST(Shape, SetPoint) {
		Square square(Vec2(0, 0), Vec2(100, 100));
		square.SetPoint(Vec2(50, 50));
		EXPECT_EQ(50, square.GetPoint().x());
	} END

	TEST(Shape, GetSegments) {
		Square square(Vec2(0, 0), Vec2(100, 100));
		Triangle triangle(Vec2(0, 0), Vec2(100, 100));
		Circle circle(Vec2(500, 500), Vec2(600, 600));

		EXPECT_EQ(4, square.GetSegments().size());
		EXPECT_EQ(3, triangle.GetSegments().size());
		EXPECT_EQ(0, circle.GetSegments().size());
	} END

	TEST(Shape, Save) {
		Square square(Vec2(0, 0), Vec2(100, 100));
		Triangle triangle(Vec2(0, 0), Vec2(100, 100));
		Circle circle(Vec2(500, 500), Vec2(600, 600));

		std::stringstream sstream;
		square.Save(sstream);
		triangle.Save(sstream);
		circle.Save(sstream);

		EXPECT_STREQ("1 0 0 100 100\n2 0 0 100 100\n3 500 500 600 600\n", sstream.str().c_str());
	} END

	TEST(Scene, Add_RemoveShape) {
		Scene scene;
		Square* testSquare = new Square(Vec2(0, 0), Vec2(100, 100));

		scene.AddShape(testSquare);
		EXPECT_EQ(1, scene.GetNumberOfShapes());

		EXPECT_THROW(scene.AddShape(testSquare), const char*);

		scene.RemoveShape(testSquare);
		EXPECT_EQ(0, scene.GetNumberOfShapes());
	} END

	TEST(Scene, Save_and_Load) {
		{
			Scene scene;
			scene.SetSaveFileName("test.txt");
		
			scene.AddShape(new Square(Vec2(0, 1), Vec2(2, 3)));
			scene.AddShape(new Triangle(Vec2(4, 5), Vec2(6, 7)));
			scene.AddShape(new Circle(Vec2(8, 9), Vec2(10, 11)));
			scene.Save();
		}

		Scene scene;
		std::string debugOutput = "";
		std::stringstream debugOutputStream(debugOutput);
		scene.SetSaveFileName("test.txt");

		std::ofstream of;
		of.open(scene.GetSaveFileName(), std::ios_base::app);
		of << "6 1 1 1 1";
		of.close();

		scene.Load(debugOutputStream);
		EXPECT_STREQ("Square(origo(0, 1), point(2, 3));\nTriangle(origo(4, 5), point(6, 7));\nCircle(origo(8, 9), point(10, 11));\nShape with serialisation id:6 does not exists.\n", debugOutputStream.str().c_str());		

		of.open(scene.GetSaveFileName(), std::ofstream::out | std::ofstream::trunc);
		of.close();
	} END

	TEST(Vector, all) {
		Vector<char> vec;
		EXPECT_EQ(0, vec.size());
		EXPECT_EQ(2, vec.capacity());
		vec.push_back('a');
		vec.push_back('b');
		vec.push_back('c');
		EXPECT_EQ(4, vec.capacity());
		vec.push_back('d');
		EXPECT_EQ(4, vec.size());
		EXPECT_EQ('a', vec.at(0));
		EXPECT_EQ('b', vec.at(1));
		EXPECT_EQ('c', vec.at(2));
		vec.splice(1, 2);
		EXPECT_EQ(2, vec.size());
		EXPECT_EQ('a', vec.at(0));
		EXPECT_EQ('d', vec.at(1));
		vec.pop_back();
		EXPECT_EQ(1, vec.size());
		EXPECT_EQ('a', vec.at(0));
	} END

	return 0;
}
#endif // JPORTA


#ifndef JPORTA
int main(void) {
	{
	// set up scene
	Scene scene;
	scene.SetBackgroundColor({ 89, 116, 117, 255 });
	scene.Load();

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

	SDL_Texture** selectionTextures = new SDL_Texture*[3];
	selectionTextures[SQUARE] = IMG_LoadTexture(scene.GetRenderer(), "./square_selected2.png");
	selectionTextures[TRIANGLE] = IMG_LoadTexture(scene.GetRenderer(), "./triangle_selected2.png");
	selectionTextures[CIRCLE] = IMG_LoadTexture(scene.GetRenderer(), "./circle_selected2.png");

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

	scene.Save();
	}

	return 0;
}
#endif // !JPORTA
