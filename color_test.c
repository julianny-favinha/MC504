#include <stdio.h>

#define ANSI_COLOR_GRAY   "\x1b[30m"
#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_PURPLE "\x1b[35m"
#define ANSI_COLOR_CYAN   "\x1b[36m"
#define ANSI_COLOR_WHITE  "\x1b[37m"
#define ANSI_COLOR_RESET  "\x1b[0m"

#define PRINT_RED(str)    printf("%s%s%s", ANSI_COLOR_RED, str, ANSI_COLOR_RESET)
#define PRINT_GREEN(str)  printf("%s%s%s", ANSI_COLOR_GREEN, str, ANSI_COLOR_RESET)
#define PRINT_YELLOW(str) printf("%s%s%s", ANSI_COLOR_YELLOW, str, ANSI_COLOR_RESET)
#define PRINT_BLUE(str)   printf("%s%s%s", ANSI_COLOR_BLUE, str, ANSI_COLOR_RESET)
#define PRINT_PURPLE(str) printf("%s%s%s", ANSI_COLOR_PURPLE, str, ANSI_COLOR_RESET)
#define PRINT_CYAN(str)   printf("%s%s%s", ANSI_COLOR_CYAN, str, ANSI_COLOR_RESET)
#define PRINT_WHITE(str)  printf("%s%s%s", ANSI_COLOR_WHITE, str, ANSI_COLOR_RESET)
#define PRINT_GRAY(str)   printf("%s%s%s", ANSI_COLOR_GRAY, str, ANSI_COLOR_RESET)

int main() {
	PRINT_GRAY("This is in Gray Color\n");
	PRINT_RED("This is in Red Color\n");
	PRINT_GREEN("This is in Green Color\n");
	PRINT_YELLOW("This is in Yellow Color\n");
	PRINT_BLUE("This is in Blue Color\n");
	PRINT_PURPLE("This is in Purple Color\n");
	PRINT_CYAN("This is in Cyan Color\n");
	PRINT_WHITE("This is in White Color\n");
	return 0;
}
