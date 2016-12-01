#pragma once

#include <stdint.h>
#include "canvas.h"

namespace kr
{
	class WebCanvas2D :public WebCanvas
	{
	public:
		WebCanvas2D(int width, int height) noexcept;
		~WebCanvas2D() noexcept;

		struct Composite
		{
			Composite& operator =(const char * str) noexcept;
		};
		struct StrokeStyle
		{
			StrokeStyle& operator =(uint32_t style) noexcept;
		};
		struct FillStyle
		{
			FillStyle& operator =(uint32_t style) noexcept;
		};
		StrokeStyle strokeStyle;
		FillStyle fillStyle;
		Composite globalCompositeOperation;

		void save() noexcept;
		void restore() noexcept;
		void translate(float x, float y) noexcept;
		void rotate(float radian) noexcept;
		void rect(float x, float y, float width, float height) noexcept;
		void oval(float x, float y, float width, float height) noexcept;
		void clearRect(float x, float y, float width, float height) noexcept;
		void strokeRect(float x, float y, float width, float height) noexcept;
		void fillRect(float x, float y, float width, float height) noexcept;
		void strokeOval(float x, float y, float width, float height) noexcept;
		void fillOval(float x, float y, float width, float height) noexcept;

		void beginPath() noexcept;
		void closePath() noexcept;
		void stroke() noexcept;
		void fill() noexcept;
		void clip() noexcept;
		void moveTo(float x, float y) noexcept;
		void lineTo(float x, float y) noexcept;
		void arc(float x, float y, float radius, float startAngle, float endAngle, bool counterClockwise = false) noexcept;
		void quadraticCurveTo(float cpx, float cpy, float x, float y) noexcept;

		void exRoundRect(float x, float y, float width, float height, float radius) noexcept;
	};
}
