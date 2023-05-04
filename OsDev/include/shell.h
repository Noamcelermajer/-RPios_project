#pragma once
#include "strings.h"
#include "fat.h"
#include "mini_uart.h"

extern unsigned char bss_end;

int strReplace(char* str, char match, char replace);

void** freeArray(void** pointerArray);

char** strcpyArray(char** strings);

int wordCount(char** words);

int indexOf(char* str, const char* chars);

void printStrings(char** strs);

char** splitString(char* buffer, const char* sep, const char* substrsyms);

typedef struct Var
{
	char* name;
	
	int num;
	char* str;
	
}Var;


typedef union TypeData
{
	int num;
	char* str;
}TypeData;

void clean_str(char * str);

int Is_A_Command(char* input);

int  getSubString(char* source, char* target, int from, int to);

void Clean();

Var All_Variables[100];
int Num_of_variables;

void Run_Shell();

int check_if_number(char* str);

int Parse_Command(char* input_str);

void Var_Assign(char* name, char* value);

char* Get_Str_Value_by_name(char* name);

int Get_Int_Value_by_name(char* name);

int check_if_variable(char* input_str);

int only_spaces(char* input_str);

void Handle_Echo(char* input_str);

void Handle_Ls();

void Handle_Cat(char* name);

void Handle_Touch(char* input_str);

void Handle_Mkdir(char* input_str);

void Handle_nano(char* input_str);

void Handle_Clear();

void Handle_Print_Picture();

void Handle_Remove_Picture();

void Handle_Help();

void simpleCmd(int option);