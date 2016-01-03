#pragma once
//Took some of this stuff from katatem because I liked his macros.


#define Args(...) void* (__thiscall*)(void*, ##__VA_ARGS__)
#define extends : public

