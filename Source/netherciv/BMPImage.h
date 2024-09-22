// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "vector"

struct Color {
	float r, g, b;

	Color();
	Color(float r, float g, float b);
	~Color();
};

/**
 * 
 */
class NETHERCIV_API BMPImage
{
public:
	BMPImage();
	BMPImage(int width, int height);
	~BMPImage();

	Color GetColor(int x, int y) const;
	void SetColor(const Color& color, int x, int y);
	void PrintColors();

	void Read(const char* path);
	void Export(const char* path);

private:
	int m_width;
	int m_height;
	std::vector<Color> m_colors;
};
