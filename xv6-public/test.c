#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char* argv[])
{
	int child = getpid();
	int parent = getppid();
	printf(1,"My pid is %d\n",child);
	printf(1,"My pid is %d\n",parent);
	exit();
}
