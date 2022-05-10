#pragma once
#include "JPorta.h"

#include <iostream>
#include <fstream>
#include "Vec2.h"
#include "Input.h"

#ifndef JPORTA
#include <SDL.h>
#undef main
#endif // !JPORTA

enum ShapeType { SQUARE = 0, TRIANGLE = 1, CIRCLE = 2 };

class Shape {
protected:
	Vec2 origo;
	Vec2 point;

#ifndef JPORTA
	SDL_Color color;

	void DrawCircle(SDL_Renderer* renderer, Vec2 origo, int r) {
		int x = r;
		int y = 0;
		int err = 0;
		int x0 = origo.x;
		int y0 = origo.y;

		while (x >= y)
		{
			SDL_RenderDrawPointF(renderer, x0 + x, y0 + y);
			SDL_RenderDrawPointF(renderer, x0 + y, y0 + x);
			SDL_RenderDrawPointF(renderer, x0 - y, y0 + x);
			SDL_RenderDrawPointF(renderer, x0 - x, y0 + y);
			SDL_RenderDrawPointF(renderer, x0 - x, y0 - y);
			SDL_RenderDrawPointF(renderer, x0 - y, y0 - x);
			SDL_RenderDrawPointF(renderer, x0 + y, y0 - x);
			SDL_RenderDrawPointF(renderer, x0 + x, y0 - y);

			if (err <= 0)
			{
				y += 1;
				err += 2 * y + 1;
			}

			if (err > 0)
			{
				x -= 1;
				err -= 2 * x + 1;
			}
		}
	}

	void DrawPoint(Vec2& P) {
		SDL_SetRenderDrawColor(debugRenderer, 255, 0, 0, 255);

		SDL_Rect rect = { P.x - 1, P.y - 1, 2, 2 };
		SDL_RenderDrawRect(debugRenderer, &rect);
	}
#endif // !JPORTA

	long double areaOfTriangle(Vec2& A, Vec2& B, Vec2& C) {
		auto a = (B - C).GetLength();
		auto b = (A - C).GetLength();
		auto c = (A - B).GetLength();

		auto s = (a + b + c) / 2;
		return sqrt(s * (s - a) * (s - b) * (s - c));
	}

public:
#ifndef JPORTA
	static SDL_Renderer* debugRenderer;
#endif // !JPORTA

	Shape(Vec2 _origo, Vec2 _point) : 
		origo(_origo), 
		point(_point)
#ifndef JPORTA
		,color({255, 255, 255, 255}) 
#endif // !JPORTA
	{}

	Vec2& GetOrigo() {
		return origo;
	}
	void SetOrigo(Vec2& _origo) {
		origo = origo;
	}

	Vec2& GetPoint() {
		return point;
	}
	void SetPoint(Vec2 _point) {
		point = _point;
	}

	void MoveTo(Vec2 position) {
		Vec2 offset = position - origo;
		origo += offset;
		point += offset;
	}
#ifndef JPORTA
	virtual void draw(SDL_Renderer* renderer) = 0;
	virtual void Update() = 0;
#endif // !JPORTA

	virtual bool IsPointInside(const Vec2) = 0;
	virtual bool IsInsideCircle(const double) = 0;

	virtual std::ofstream& Save(std::ofstream& file) = 0;

	virtual ~Shape() {
	}
};

#ifndef JPORTA
class DragAndDrop : virtual Shape {
	bool isBeingDragged;
	bool isBeingScaled;
	Vec2 dragStart;

	Vec2 dragStartOffset;

	unsigned dragStartListenerID;

public:
	static bool isDragging;

	DragAndDrop() : isBeingDragged(false), isBeingScaled(false), dragStart(Vec2()), dragStartOffset(Vec2()) {
		dragStartListenerID = Mouse::OnClick(MouseButton::LEFT, [=]() {
			if (!this) return;

			// check if click was near the defining point
			if ((point - Mouse::GetPosition()).GetLength() < 5 && !isDragging) {
				// start rotating and scaling
				color = { 209, 77, 150, 255 };
				isBeingScaled = true;
				isDragging = true;

				// register listener to mouse which will end scaling when the left button is released
				Mouse::OnRelease(MouseButton::LEFT, [&]() {
					color = { 255, 255, 255, 255 };
					isBeingScaled = false;
					isDragging = false;
				}, false);
			}

			// check if click was inside this shape
			else if (IsPointInside(Mouse::GetPosition()) && !isDragging) {
				// start of dragging
				color = { 179, 120, 255, 255 };
				isDragging = true;
				isBeingDragged = true;
				dragStartOffset = Mouse::GetPosition() - origo;

				// register listener to mouse which will end draggig when the left button is released
				Mouse::OnRelease(MouseButton::LEFT, [&]() {
					color = { 255, 255, 255, 255 };
					isBeingDragged = false;
					isDragging = false;
				}, false);
			}
		});
	}

	void Update() {
		if (this->isBeingDragged)
			MoveTo(Mouse::GetPosition() - dragStartOffset);
		if (this->isBeingScaled)
			SetPoint(Mouse::GetPosition());
	}

	~DragAndDrop() {
		Mouse::RemoveEventListener(dragStartListenerID);
	}
};
#endif // !JPORTA

class Square : public virtual Shape
#ifndef JPORTA
	,public DragAndDrop 
#endif // !JPORTA
{
public:
	Square(Vec2 origo, Vec2 point) : Shape(origo, point) {}

	bool IsPointInside(Vec2 P) {
		Vec2 A, B, C, D;
		A = point;
		B = (point - origo).Rotate(M_PI / 2.0) + origo;
		C = (point - origo).Rotate(M_PI) + origo;
		D = (point - origo).Rotate(3 * M_PI / 2.0) + origo;

		double APD, DPC, CPB, PBA;
		APD = areaOfTriangle(A, P, D);
		DPC = areaOfTriangle(D, P, C);
		CPB = areaOfTriangle(C, P, B);
		PBA = areaOfTriangle(P, B, A);

		if (APD + DPC + CPB + PBA > GetArea() + 0.001) return false;
		return true;
	}
	bool IsInsideCircle(const double) {
		return true;
	}

	long double GetArea() {
		return ((origo - point).GetLength() * 4.0 * (origo - point).GetLength()) / 2.0;
	}

#ifndef JPORTA
	void draw(SDL_Renderer* renderer) {
		Vec2 a, b, c, d;
		a = point;
		b = Vec2(point - origo).Rotate(M_PI / 2) + origo;
		c = (point - origo).Rotate(M_PI) + origo;
		d = (point - origo).Rotate((3 * M_PI) / 2) + origo;
		
		SDL_SetRenderDrawColor(renderer, 209, 77, 150, color.a);
		DrawCircle(renderer, point, 5);

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(renderer, a.x, a.y, b.x, b.y);
		SDL_RenderDrawLine(renderer, b.x, b.y, c.x, c.y);
		SDL_RenderDrawLine(renderer, c.x, c.y, d.x, d.y);
		SDL_RenderDrawLine(renderer, d.x, d.y, a.x, a.y);
	}
#endif // !JPORTA

	std::ofstream& Save(std::ofstream& file) {
		file << "1 " << origo.x << " " << origo.y << " " << point.x << " " << point.y << std::endl;
		return file;
	}
};

class Triangle : virtual public Shape
#ifndef JPORTA
	, public DragAndDrop
#endif // !JPORTA
{
public:
	Triangle(Vec2 origo, Vec2 point) : Shape(origo, point) {}

	bool IsPointInside(Vec2 P) {
		Vec2 A, B, C;
		A = point;
		B = (point - origo).Rotate(M_PI * 2.0 / 3.0) + origo;
		C = (point - origo).Rotate(M_PI * 4.0 / 3.0) + origo;

		double APC, CPB, BPA;
		APC = areaOfTriangle(A, P, C);
		CPB = areaOfTriangle(C, P, B);
		BPA = areaOfTriangle(B, P, A);

		if (APC + CPB + BPA > GetArea() + 0.001) return false;
		return true;
	}
	
	bool IsInsideCircle(const double) {
		return true;
	}

	long double GetArea() {
		Vec2 A, B, C;
		A = point;
		B = (point - origo).Rotate(M_PI * 2.0 / 3.0) + origo;
		C = (point - origo).Rotate(M_PI * 4.0 / 3.0) + origo;
		return areaOfTriangle(A, B, C);
	}

#ifndef JPORTA
	void draw(SDL_Renderer* renderer) {
		Vec2 a, b, c;

		a = point;
		b = (point - origo).Rotate(M_PI * 2 / 3) + origo;
		c = (point - origo).Rotate(M_PI * 4 / 3) + origo;

		SDL_SetRenderDrawColor(renderer, 209, 77, 150, color.a);
		DrawCircle(renderer, point, 5);

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(renderer, a.x, a.y, b.x, b.y);
		SDL_RenderDrawLine(renderer, b.x, b.y, c.x, c.y);
		SDL_RenderDrawLine(renderer, c.x, c.y, a.x, a.y);
	}
#endif // !JPORTA

	std::ofstream& Save(std::ofstream& file) {
		file << "2 " << origo.x << " " << origo.y << " " << point.x << " " << point.y << std::endl;
		return file;
	}

	~Triangle() {

	}
};

class Circle : virtual public Shape
#ifndef JPORTA
	, public DragAndDrop
#endif // !JPORTA
{
public:
	Circle(Vec2 origo, Vec2 point) : Shape(origo, point) {}

	bool IsPointInside(Vec2 P) {
		return (P - origo).GetLength() <= (origo - point).GetLength();
	}

	bool IsInsideCircle(const double) {
		return true;
	}

	long double GetArea() {
		double r = (origo - point).GetLength();
		return r * r * M_PI;
	}

#ifndef JPORTA
	void draw(SDL_Renderer* renderer) {
		SDL_SetRenderDrawColor(renderer, 209, 77, 150, color.a);
		DrawCircle(renderer, point, 5);

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		DrawCircle(renderer, origo, (origo - point).GetLength());
	}
#endif // !JPORTA

	std::ofstream& Save(std::ofstream& file) {
		file << "3 " << origo.x << " " << origo.y << " " << point.x << " " << point.y << std::endl;
		return file;
	}

	~Circle() {

	}
};
