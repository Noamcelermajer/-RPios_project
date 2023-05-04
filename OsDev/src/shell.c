#define _CRT_SECURE_NO_WARNINGS
#include "shell.h"
#include <printf.h>
#include <stdlib.h>
#include "delays.h"
#include "power.h"
#include "lfb.h"
#include "peripherals/base.h"

#define NUMBER_OF_BUILTIN 10
#define MAX_TOKENS 25
#define MAX_LEN 256

#define ECHO 0
#define LS 1
#define CAT 2
#define TOUCH 3
#define MKDIR 4
#define NANO 5
#define CLEAR 6
#define PRINT_PICTURE 7
#define REMOVE_PICTURE 8
#define HELP 9

#define MAIN_SHELL 1
#define PICTURE_OPTIONS 2

char* BuiltIn_Command[] = { "echo", "ls", "cat", "touch", "mkdir", "nano" , "clear", "print picture", "remove picture", "help"};

char cmd[MAX_LEN] = "";

int picture_on_frame_buffer = 0;

int strReplace(char* str, char match, char replace) {
	int count = 0;
	for (int i = 0; str[i] != 0; ++i) {
		if (str[i] == match) {
			str[i] = replace;
			++count;
		}
	}
	return count;
}

void** freeArray(void** pointerArray) {
	if (!pointerArray) {
		return NULL;
	}
	for (int i = 0; pointerArray[i] != 0; ++i) {
		free(pointerArray[i]);
		pointerArray[i] = NULL;
	}
	free(pointerArray);
	pointerArray = NULL;
	return pointerArray;
}

char** strcpyArray(char** strings) {
	int count = 0;
	while (strings[count++]);
	char** newArray = malloc(sizeof(char*) * count);
	count = 0;
	while (strings[count]) {
		newArray[count] = malloc(sizeof(char) * (1 + strlen(strings[count])));
		strcpy(newArray[count], strings[count]);
		++count;
	}
	newArray[count] = 0;
	return newArray;
}

int wordCount(char** words) {
	int count = 0;
	while (words[count]) {
		++count;
	}
	return count;
}

int indexOf(char* str, const char* chars) {
	for (int i = 0; str[i] != 0; ++i) {
		for (int j = 0; chars[j] != 0; ++j) {
			if (str[i] == chars[j])
				return i;
		}
	}
	return -1;
}

void printStrings(char** strs) {
	int index = 0;
	if (strs != NULL) {
		for (index = 0; strs[index] != NULL; ++index) {
			if (index == 0) {
				printf("Strings:");
			}
			else {
				printf(",");
			}
			printf("'%s'", strs[index]);
		}
	}
	if (index == 0) {
		printf("No strings");
	}
	printf("\r\n");
}

/*

char** splitString(char* buffer, const char* sep, const char* substrsyms) {
	char* tokens[MAX_TOKENS + 1];
	int index = 0;

	// Parse string, saved tokens point directly to buffer:
	char* token = strtok(buffer, sep);
	const char SINGLE_QUOTE[] = "\'";
	const char DOUBLE_QUOTE[] = "\"";

	while (token && index < MAX_TOKENS) {
		if (token[0] == SINGLE_QUOTE[0]) {
			token[strlen(token)] = ' ';
			token = strtok(token, SINGLE_QUOTE);
		}
		else if (token[0] == DOUBLE_QUOTE[0]) {
			token[strlen(token)] = ' ';
			token = strtok(token, DOUBLE_QUOTE);
		}
		tokens[index++] = token;
		token = strtok(NULL, sep);
	}
	tokens[index++] = NULL; // NULL terminated array

	// Allocate array of cstrings:
	char** returnTokens = malloc(sizeof(char*) * index);

	// Copy pointers, not content:
	index = 0;
	while (tokens[index]) {
		returnTokens[index] = tokens[index];
		++index;
	}
	returnTokens[index] = NULL; // NULL terminated array
	return returnTokens;
}

*/

int Is_A_Command(char* input)
{
	for (int i = 0; i < NUMBER_OF_BUILTIN; i++)
	{
		if (!strcmp(input, BuiltIn_Command[i]))
		{
			return i;
		}
	}
	return -1;
}

void Clean()
{
	for (int i = 0; i < 100; i++)
	{
		All_Variables[i].name = "";
		All_Variables[i].str = "";
		All_Variables[i].num = 0;
	}
}

int  getSubString(char* source, char* target, int from, int to)
{
	int length = 0;
	int i = 0, j = 0;

	//get length
	while (source[i++] != '\0')
		length++;

	if (from<0 || from>length) {
		return 1;
	}
	if (to > length) {
		return 1;
	}

	for (i = from, j = 0; i <= to; i++, j++) {
		target[j] = source[i];
	}

	//assign NULL at the end of string
	target[j] = '\0';

	return 0;
}

void Run_Shell()
{
	Num_of_variables = 0;
	uint32_t kernel_top = (uint32_t)&bss_end;
	init_memory(kernel_top + 20000000, PBASE);
	Clean();
	Handle_Clear();
	// printf("****************\r\n****WELCOME****\r\n**To Neos Shell*\r\n****************\r\n");
	// printf("\r\n\r\n");
	do
	{
		printf("username@'pwd'> ");
		simpleCmd(MAIN_SHELL);
		if (!Parse_Command(cmd))
		{
			printf("\r\nThere was an error please check ur syntax for help with command please type help!\r\n");
		}
	} while (1);
}

void clean_str(char* str)
{
	for (int i = 0; i < strlen(str); i++)
	{
		str[i] = 0;
	}
}

int Parse_Command(char* input_str)
{
	char token[MAX_LEN], name[MAX_LEN], value[MAX_LEN];
	clean_str(token);
	clean_str(name);
	clean_str(value);
	for (int i = 0; i < strlen(input_str); i++)
	{
		if (input_str[i] == ' ' && input_str[i + 1] == '=' && input_str[i + 2] == ' ')
		{
			getSubString(input_str, value, i + 3, strlen(input_str) - 1);
			Var_Assign(token, value);
			return 1;
		}
		token[i] = input_str[i];
		token[i + 1] = 0;
		switch (Is_A_Command(token))
		{
		case ECHO:

			if(input_str[i + 1] == ' ' || input_str[i + 1] == 0)
			{
				Handle_Echo(input_str);
				return 1;
			}
			return 0;
			break;

		case LS:

			if(only_spaces(input_str + i + 1))
			{
				Handle_Ls();
				return 1;
			}
			return 0;
			break;

		case CAT:

			if(input_str[i + 1] == ' ')
			{
				getSubString(input_str, value, i + 2, strlen(input_str) - 1);
				Handle_Cat(value);
				return 1;
			}
			break;

		case TOUCH:

			if(only_spaces(input_str + i + 1))
			{
				Handle_Touch(input_str);
				return 1;
			}
			return 0;
			break;

		case MKDIR:

			if(only_spaces(input_str + i + 1))
			{
				Handle_Mkdir(input_str);
				return 1;
			}
			return 0;
			break;

		case NANO:

			if(only_spaces(input_str + i + 1))
			{
				Handle_nano(input_str);
				return 1;
			}
			return 0;
			break;

		case CLEAR:

			if(only_spaces(input_str + i + 1))
			{
				Handle_Clear();
				return 1;
			}
			return 0;
			break;

		case PRINT_PICTURE:

			if(only_spaces(input_str + i + 1))
			{
				Handle_Print_Picture();
				return 1;
			}
			return 0;
			break;

		case REMOVE_PICTURE:

			if(only_spaces(input_str + i + 1))
			{
				Handle_Remove_Picture();
				return 1;
			}
			return 0;
			break;

		case HELP:

			if(only_spaces(input_str + i + 1))
			{
				Handle_Help();
				return 1;
			}
			return 0;
			break;

		default:
			break;
		}
	}

	char* str = 0;
	int num = 0;
	if(check_if_variable(input_str) && strcmp(input_str, ""))
	{
		str = Get_Str_Value_by_name(input_str);
		num = Get_Int_Value_by_name(input_str);
		if(str)
		{
			printf("%s", str);
		}
		else
		{
			printf("%d", num);
		}
		printf("\n\n\r");
		return 1;
	}

	if(!strcmp(input_str, ""))
	{
		printf("\r\n");
		return 1;
	}

	return 0;
}

int check_if_variable(char* input_str)
{
	for(int i = 0; i < Num_of_variables; i++)
	{
		if(!strcmp(All_Variables[i].name, input_str))
		{
			return 1;
		}
	}
	return 0;
}

int only_spaces(char* input_str)
{
	for(int i = 0; i < strlen(input_str); i++)
	{
		if(input_str[i] != ' ')
		{
			return 0;
		}
	}

	return 1;
}

void Var_Assign(char* name, char* value)
{
	// printf("name : %s, val : %s\r\n", name, value);
	int var_exists = 0, index = 0;
	if(check_if_variable(name))
	{
		for(int i = 0; !var_exists && i < Num_of_variables; i++)
		{
			if(!strcmp(All_Variables[i].name, name))
			{
				var_exists = 1;
				index = i;
			}
		}

		if(!check_if_number(value))
		{
			if(All_Variables[index].str !=  NULL)
			{
				free(All_Variables[index].str);
				All_Variables[index].str = NULL;
			}
			All_Variables[index].str = (char*)malloc(sizeof(char) * (strlen(value) + 1));
			// printf("%x\r\n", All_Variables[index].str);
			clean_str(All_Variables[index].str);
			strcpy(All_Variables[index].str, value);
		}
		else
		{
			All_Variables[index].num = 0;
			if(All_Variables[index].str != NULL)
			{
				free(All_Variables[index].str);
				All_Variables[index].str = NULL;
			}
			for(int i = 0; i < strlen(value); i++)
			{
				All_Variables[index].num = All_Variables[index].num * 10 + (value[i] - '0'); 
			}
		}
	}
	else
	{
		index = Num_of_variables;
		All_Variables[index].name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		// printf("%x\r\n", All_Variables[index].name);
		clean_str(All_Variables[index].name);
		strcpy(All_Variables[index].name, name);
		if (!check_if_number(value))
		{
			All_Variables[index].str = (char*)malloc(sizeof(char) * (strlen(value) + 1));
			// printf("%x\r\n", All_Variables[index].str);
			clean_str(All_Variables[index].str);
			strcpy(All_Variables[index].str, value);
		}
		else
		{
			All_Variables[index].num = 0;
			for(int i = 0; i < strlen(value); i++)
			{
				All_Variables[index].num = All_Variables[index].num * 10 + (value[i] - '0'); 
			}
		}
		Num_of_variables += 1;
	}

	printf("\r\n");
}

char* Get_Str_Value_by_name(char* name)
{
	for (int i = 0; i < Num_of_variables; i++)
	{
		if (!strcmp(All_Variables[i].name, name) && strcmp(All_Variables[i].str, ""))
		{
			return All_Variables[i].str;
		}
	}
	return 0;
}

int Get_Int_Value_by_name(char* name)
{
	for (int i = 0; i < Num_of_variables; i++)
	{
		if (!strcmp(All_Variables[i].name, name))
		{
			return All_Variables[i].num;
		}
	}
	return 0;
}


void splitStr(char* str, char  delim,char strings[10][25])
{
	int i=0, j=0, cnt=0;

	for (i = 0; i <= (strlen(str)); i++)
	{
		// if delim or NULL found, assign NULL into splitStrings[cnt]
		if (str[i] == delim || str[i] == '\0')
		{
			strings[cnt][j] = '\0';
			cnt++;  //for next word
			j = 0;    //for next word, init index to 0
		}
		else
		{
			strings[cnt][j] = str[i];
			j++;
		}
	}
}
void CleanDoubleArray(char strdns[10][25])
{
	for (int i = 1; i < 10; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			strdns[i][j] = 0;
		}
	}
}
void Handle_Echo(char* input_str)
{
	for(int i = 5; i < strlen(input_str); i++)
	{
		printf("%c", input_str[i]);
	}

	printf("\n\n\r");

}


void Handle_Ls()
{
	if(fat_getpartition())
	{
		fat_listdirectory();
	}
}


void Handle_Cat(char* name)
{
	int cluster = 0;
	if(fat_getpartition())
	{
		cluster = fat_getcluster(name);
		if(cluster)
		{
			printf("%s\r\n\n", fat_readfile(cluster));
		}
	}
}

void Handle_Touch(char* input_str) {}
void Handle_Mkdir(char* input_str) {}
void Handle_nano(char* input_str) {}


void Handle_Clear()
{
	printf("\e[1;1H\e[2J");
	printf("NeOS Raspberry Pi 3 Operating System. All Rights Reserved\r\n");
    printf("Version 1.0\r\n\n");
	printf("NeOS Shell - V1.0\r\n\n");
}


void Handle_Help()
{
	printf("\r\n");
	printf("echo TEXT  ->  print text\r\n\n");
	printf("ls  ->  list all the files of the root directory\r\n\n");
	printf("cat FILE  ->  print the text of the file if he exists\r\n\n");
	printf("clear  ->  clear the screen\r\n\n");
	printf("print picture  ->  print a picture to the screen (frame buffer)\r\n\n");
	printf("remove picture  ->  remove a picture of the screen (frame buffer)\r\n\n");
	printf("VARIABLE = NUMBER  ->  initialize an int variable\r\n\n");
	printf("VARIABLE = TEXT  ->  initialize a string variable\r\n\n");
	printf("VARIABLE  ->  print a value of a variable if he exists\r\n\n");
	printf("Shutdown (or shutdown)  ->  shutdown the system\r\n\n");
	printf("Reboot (or reboot)  ->  reboot the system\r\n\n");
}


void Handle_Print_Picture()
{
	printf("\r\n");
	printf("Choose what picture to print:\r\n\n");
	printf(" - Homer Simpson\r\n\n");
	printf(" - Star Wars Stormtrooper\r\n\n");
	printf("Write cancel to cancel\r\n\n");
	simpleCmd(PICTURE_OPTIONS);
	while(strcmp(cmd, "Homer Simpson") && strcmp(cmd, "Star Wars Stormtrooper") && strcmp(cmd, "cancel"))
	{
		printf("Invalid command! Try again!\r\n\n");
		simpleCmd(PICTURE_OPTIONS);
	}

	if(!strcmp(cmd, "Homer Simpson") || !strcmp(cmd, "Star Wars Stormtrooper"))
	{
		if(picture_on_frame_buffer)
		{
			lfb_removePicture();
		}
		lfb_showpicture(cmd);
		picture_on_frame_buffer = 1;
	}
}

void Handle_Remove_Picture()
{
	if(picture_on_frame_buffer)
	{
		lfb_removePicture();
		picture_on_frame_buffer = 0;
	}
}

int check_if_number(char* str)
{
	for (int i = 0; i < strlen(str); i++)
	{
		if (!(str[i] >= '0' && str[i] <= '9'))
		{
			return 0;
		}
	}
	return 1;
}


void simpleCmd(int option)
{
	clean_str(cmd);
    int i,cmdidx=0,cmdlast=0;
    char c;
    // read until Enter pressed
    while((c=uart_recv())!='\n')
	{
        if(c==27)
		{
            c=uart_recv();
            if(c=='[')
			{
                c=uart_recv();
                if(c=='C') c=3; else    // left
                if(c=='D') c=2; else    // right
                if(c=='3')
				{
                    c=uart_recv();
                    if(c=='~') c=1;     // delete
                }
            }
        }
        // Backspace
        if(c==8 || c==127)
		{
            if(cmdidx>0)
			{
                cmdidx--;
                for(i=cmdidx;i<cmdlast;i++) cmd[i]=cmd[i+1];
                cmdlast--;
            }
        }
		else
        // Delete
        if(c==1) {
            if(cmdidx<cmdlast) {
                for(i=cmdidx;i<cmdlast;i++) cmd[i]=cmd[i+1];
                cmdlast--;
            }
        } else
        // cursor left
        if(c==2) {
            if(cmdidx>0) cmdidx--;
        } else
        // cursor right
        if(c==3) {
            if(cmdidx<cmdlast) cmdidx++;
        } else {
            // is there a valid character and space to store it?
            if(c<' ' || cmdlast>=sizeof(cmd)-1) {
                continue;
            }
            // if we're not appending, move bytes after cursor
            if(cmdidx<cmdlast) {
                for(i=cmdlast;i>cmdidx;i--)
                    cmd[i]=cmd[i-1];
            }
            cmdlast++;
            cmd[cmdidx++]=c;
        }
        cmd[cmdlast]=0;

		if(option == MAIN_SHELL)
		{
			printf("\rusername@'pwd'> %s \r\e[%dC",cmd,cmdidx+16);
		}

		else if(option == PICTURE_OPTIONS)
		{
			printf("\r> %s \r\e[%dC",cmd,cmdidx+2);
		}
    }

    if((!strcmp(cmd, "Shutdown") || !strcmp(cmd, "shutdown")) && option == MAIN_SHELL)
    {
        printf("\e[1;1H\e[2J");
		for(int i = 0; i < Num_of_variables; i++)
		{
			if(All_Variables[i].name != NULL)
			{
				free(All_Variables[i].name);
				All_Variables[i].name = NULL;
			}
			if(All_Variables[i].str != NULL)
			{
				free(All_Variables[i].str);
				All_Variables[i].str = NULL;
			}
		}
        power_off();
    }

    else if((!strcmp(cmd, "Reboot") || !strcmp(cmd, "reboot")) && option == MAIN_SHELL)
    {
        printf("\e[1;1H\e[2J");
		for(int i = 0; i < Num_of_variables; i++)
		{
			if(All_Variables[i].name != NULL)
			{
				free(All_Variables[i].name);
				All_Variables[i].name = NULL;
			}
			if(All_Variables[i].str != NULL)
			{
				free(All_Variables[i].str);
				All_Variables[i].str = NULL;
			}
		}
        reboot();
    }

	printf("\r\n");

	/* if(!strcmp(cmd, "dump bss end") && option == MAIN_SHELL)
	{
		uart_dump(&bss_end);
	}
	*/
}