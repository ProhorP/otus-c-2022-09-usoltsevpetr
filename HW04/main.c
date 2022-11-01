#include <stdio.h>
//#include <stdlib.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <dlfcn.h>

int
main(int argc, char *argv[])
{

	if (argc != 2){
		error_exit("Неверно введена команда\n"
			"Нужно ввести в таком формате:\n"
			 "\"./start name_yiur_file\"\n");
	}


	return 0;

}

void
error_exit(char *str){

        printf("%s\n", str);
        exit(1);

}

