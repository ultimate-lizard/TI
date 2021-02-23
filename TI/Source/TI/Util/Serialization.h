#pragma once

#include <string>

void writeInt(char* dst, int src);
void readInt(int& dst, const char* src);

void writeFloat(char* dst, float src);
void readFloat(float& dst, const char* src);

void writeString(char* dst, const std::string& src);
void readString(std::string& dst, char* src, int size);
