#pragma once

unsigned load_dds(const char* imagepath);
void initText2D(const char* texturePath);
void printText2D(const char* text, int x, int y, int size);
void cleanupText2D();