#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFSIZE 4096
#define PATH_MAX 255

char	buf_file[BUFFSIZE];
char	buf_file_name[PATH_MAX];
struct stat statbuf;

int
main(int argc, char *argv[])
{
	int	fd; 
	int	i=0, j=0, k=0, l=0, n=0;
	int	find_jpg=0, find_zip=0;
	int 	type_find=0; 


	if (argc != 2){
		printf("Неверно введена команда\n"
			"Нужно ввести в таком формате:\n"
			 "\"./start name_yiur_file\"\n");
		exit(1);
	}

	if (access(argv[1], R_OK) < 0){
		printf("Файл недоступен для чтения\n");
		exit(1);
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Ошибка вызова open\n");
		exit(1);
	}

	if (fstat(fd, &statbuf) < 0){
                printf("Ошибка вызова функции fstat\n");
                exit(1);

	}

	if (!S_ISREG(statbuf.st_mode)){
		printf("Выбран необычный файл\n");
		exit(1);
	}


	while ((n = read(fd, buf_file, BUFFSIZE)) > 0){
		i = 0;
		while (i < n){
			switch (type_find) {
			
				//Ищем сигнатуру начала jpeg-файла
				case 0:
				if ((buf_file[i]==-1 && k == 0) 
					|| (buf_file[i]==-40 && k == 1)){
					if (++k == 2){
						k = 0;
						find_jpg = 1;
						type_find = 1;
					}	
				} else {
					k = 0;
				}
				break;
	
				/*
				*Ищем сигнатуру zip
				*27-й символ после сигнатуры это название папки/файла
				*Поэтому ставим счетчик символов на 26 и признак
				*выполнения другого поиска
				*/
				case 1:
				if ((buf_file[i]==0x50 && k == 0) 
					|| (buf_file[i]==0x4b && k == 1)
					|| (buf_file[i]==0x03 && k == 2)
					|| (buf_file[i]==0x04 && k == 3)){
					if (++k == 4){
						k = 0;
						l = 26;
						find_zip = 1;
						type_find = 2;
					}	
				} else {
					k = 0;
				}
				break;
				
				/*
				 *Осчитываем 26 символов и после этого
				 *начинаем заполнять название каталога/файла в
				 *специальный буфер. При нахождении символа 0x00
				 *удаляем последние 3 символа(они незначащие) и ставим признак
				 *конца строки для вывода на печать
				 * */
				case 2:
				if (l == 0){
		
					if (buf_file[i] == 0x00) {
						/*ставим конец строки затирая
					 	* 3 последних ненужных символа
					  	*/
						buf_file_name[j-3] = '\0';
						//Выводим имя из архива
	                                        printf("%s\n", buf_file_name);
						j = 0;
						type_find = 1;
					
					} else if (j < PATH_MAX){
                                                buf_file_name[j++] = buf_file[i];
					}
					

				} else {
					l--;
				}
				break;
	

				default:
				break;
	
			}
		i++;
		}
	}
	

	if (n < 0){
		printf("Ошибка чтения\n");
		exit(1);
	}

	if (find_jpg == 0)
		printf("Не найдена сигнатура jpg\n");
	else if (find_zip == 0)
		printf("Не найдено сигнатура zip архива\n");

	exit(0);

}
