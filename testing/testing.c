#include <stdio.h>
#include <unistd.h>

/*This code tests the const qualifier*/
int main(void) {
	const int five = 5;

	const int *ptr = &five;
	const char *sally = "Port";
	sally = "Hohn";
	// This does not work
	// sally[0] = 'J';
	printf("const char *sally: %s\n", sally);

	// Does not work
	// char * const bob = "Port";
	// bob = "John";

	char *g = getwd(NULL);
	printf("This is g: %s\n", g);
	return 0;
}
