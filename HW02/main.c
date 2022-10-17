#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <uchar.h>

#define BUFFSIZE 4096

unsigned char	buf_file[BUFFSIZE];

struct stat statbuf;

struct utf_8_struct {
	char32_t value;
	int	length;
};
/*
 *Поиск подходящего utf-8 символа идет по такому принципу:
 *в cp1251 до 0x80 кодировка совпадает с utf-8(как у "koi8-r", "iso-8859-5")
 *значит если код символа меньше - его переносим в файл экспорта как есть.
 *Далее для каждой кодировки хранится массив(блого кодировки 1-байтные).
 *Мы заполняем массив с 0 элемента значением utf-8(с указанием длины,
 *т.к. длина utf-8 от 1 до 4 байт), соответствующим значению 0x80
 *конвертируемой кодировки. 1-й элемент заполняем значением utf-8 для значения
 *0x81 и т.д.
 *Далее при конвертации если значение символа >= 0x80, то из этого значения вычитаем 0x80
 *и таким образом сразу получаем индекс массива, где хранится подходящий utf-8
 *ну и на вывод отправляем это значение с указанием длины
 *Еще чуть не забыл, из-за особенностей расположения строк в памяти я побайтно задом наперед
 *записал значения utf-8(при записи в файл они автоматически разворачиваются в нормальный вид)
 *
 * */

/* по 16 элементов массива в одну строку делаю, чтобы не сбиться*/
struct utf_8_struct cp1251[] =
{{0x82D0, 2}, {0x83D0, 2}, {0x9A80E2, 3}, {0x85C7, 2}, {0x9E80E2, 3}, {0xA680E2, 3}, {0xA080E2, 3}, {0xA180E2, 3}, {0xAC82E2, 3}, {0xB080E2, 3}, {0x99C6, 2}, {0xB980E2, 3}, {0x8AD0, 2}, {0x8CD0, 2}, {0x8BD0, 2}, {0x8FD0, 2}
, {0x92D1, 2}, {0x9880E2, 3}, {0x9980E2, 3}, {0x9C80E2, 3}, {0x9D80E2, 3}, {0xA280E2, 3}, {0x9380E2, 3}, {0x9480E2, 3}, {0x3F, 1}, {0xA284E2, 3}, {0x8BC7, 2}, {0xBA80E2, 3}, {0x9AD1, 2}, {0x9CD1, 2}, {0x9BD1, 2}, {0x9FD1, 2}
, {0xA0C2, 2}, {0x8ED0, 2}, {0x9ED1, 2}, {0x88D0, 2}, {0xA4C2, 2}, {0x90D2, 2}, {0xA6C2, 2}, {0xA7C2, 2}, {0x81D0,2 }, {0xA9C2,2 }, {0x84D0, 2}, {0xABC2, 2}, {0xACC2, 2}, {0xADC2, 2}, {0xAEC2, 2}, {0x87D0, 2}
, {0xB0C2, 2}, {0xB1C2, 2}, {0x86D0, 2}, {0x96D1, 2}, {0x91D2, 2}, {0xB5C2, 2}, {0xB6C2, 2}, {0xB7C2, 2}, {0x91D1, 2}, {0x9684E2, 3}, {0x94D1, 2}, {0xBBC2, 2}, {0x98D1, 2}, {0x85D0, 2}, {0x95D1, 2}, {0x97D1, 2}
, {0x90D0, 2}, {0x91D0, 2}, {0x92D0, 2}, {0x93D0, 2}, {0x94D0, 2}, {0x95D0, 2}, {0x96D0, 2}, {0x97D0, 2}, {0x98D0, 2}, {0x99D0, 2}, {0x9AD0, 2}, {0x9BD0, 2}, {0x9CD0, 2}, {0x9DD0, 2}, {0x9ED0, 2}, {0x9FD0, 2}
, {0xA0D0, 2}, {0xA1D0, 2}, {0xA2D0, 2}, {0xA3D0, 2}, {0xA4D0, 2}, {0xA5D0, 2}, {0xA6D0, 2}, {0xA7D0, 2}, {0xA8D0, 2}, {0xA9D0, 2}, {0xAAD0, 2}, {0xABD0, 2}, {0xACD0, 2}, {0xADD0, 2}, {0xAED0, 2}, {0xAFD0, 2}
, {0xB0D0, 2}, {0xB1D0, 2}, {0xB2D0, 2}, {0xB3D0, 2}, {0xB4D0, 2}, {0xB5D0, 2}, {0xB6D0, 2}, {0xB7D0, 2}, {0xB8D0, 2}, {0xB9D0, 2}, {0xBAD0, 2}, {0xBBD0, 2}, {0xBCD0, 2}, {0xBDD0, 2}, {0xBED0, 2}, {0xBFD0, 2}
, {0x80D1, 2}, {0x81D1, 2}, {0x82D1, 2}, {0x83D1, 2}, {0x84D1, 2}, {0x85D1, 2}, {0x86D1, 2}, {0x87D1, 2}, {0x88D1, 2}, {0x89D1, 2}, {0x8AD1, 2}, {0x8BD1, 2}, {0x8CD1, 2}, {0x8DD1, 2}, {0x8ED1, 2}, {0x8FD1, 2}
};

struct utf_8_struct koi8_r[] = 
{
{0x8094E2, 3}, {0x8294E2, 3}, {0x8C94E2, 3}, {0x9094E2, 3}, {0x9494E2, 3}, {0x9894E2, 3}, {0x9C94E2, 3}, {0xA494E2, 3}, {0xAC94E2, 3}, {0xB494E2, 3}, {0xBC94E2, 3}, {0x8096E2, 3}, {0x8496E2, 3}, {0x8896E2, 3}, {0x8C96E2, 3}, {0x9096E2, 3},
{0x9196E2, 3}, {0x9296E2, 3}, {0x9396E2, 3}, {0xA08CE2, 3}, {0xA096E2, 3}, {0x9988E2, 3}, {0x9A88E2, 3}, {0x8889E2, 3}, {0xA489E2, 3}, {0xA589E2, 3}, {0xA0C2, 2}, {0xA18CE2, 3}, {0xB0C2, 2}, {0xB2C2, 2}, {0xB7C2, 2}, {0xB7C3, 2},
{0x9095E2, 3}, {0x9195E2, 3}, {0x9295E2, 3}, {0x91D1, 2}, {0x9395E2, 3}, {0x9495E2, 3}, {0x9595E2, 3}, {0x9695E2, 3}, {0x9795E2, 3}, {0x9895E2, 3}, {0x9995E2, 3}, {0x9A95E2, 3}, {0x9B95E2, 3}, {0x9C95E2, 3}, {0x9D95E2, 3}, {0x9E95E2, 3},
{0x9F95E2, 3}, {0xA095E2, 3}, {0xA195E2, 3}, {0x81D0, 2}, {0xA295E2, 3}, {0xA395E2, 3}, {0xA495E2, 3}, {0xA595E2, 3}, {0xA695E2, 3}, {0xA795E2, 3}, {0xA895E2, 3}, {0xA995E2, 3}, {0xAA95E2, 3}, {0xAB95E2, 3}, {0xAC95E2, 3}, {0xA9C2, 2},
{0x8ED1, 2}, {0xB0D0, 2}, {0xB1D0, 2}, {0x86D1, 2}, {0xB4D0, 2}, {0xB5D0, 2}, {0x84D1, 2}, {0xB3D0, 2}, {0x85D1, 2}, {0xB8D0, 2}, {0xB9D0, 2}, {0xBAD0, 2}, {0xBBD0, 2}, {0xBCD0, 2}, {0xBDD0, 2}, {0xBED0, 2},
{0xBFD0, 2}, {0x8FD1, 2}, {0x80D1, 2}, {0x81D1, 2}, {0x82D1, 2}, {0x83D1, 2}, {0xB6D0, 2}, {0xB2D0, 2}, {0x8CD1, 2}, {0x8BD1, 2}, {0xB7D0, 2}, {0x88D1, 2}, {0x8DD1, 2}, {0x89D1, 2}, {0x87D1, 2}, {0x8AD1, 2},
{0xAED0, 2}, {0x90D0, 2}, {0x91D0, 2}, {0xA6D0, 2}, {0x94D0, 2}, {0x95D0, 2}, {0xA4D0, 2}, {0x93D0, 2}, {0xA5D0, 2}, {0x98D0, 2}, {0x99D0, 2}, {0x9AD0, 2}, {0x9BD0, 2}, {0x9CD0, 2}, {0x9DD0, 2}, {0x9ED0, 2},
{0x9FD0, 2}, {0xAFD0, 2}, {0xA0D0, 2}, {0xA1D0, 2}, {0xA2D0, 2}, {0xA3D0, 2}, {0x96D0, 2}, {0x92D0, 2}, {0xACD0, 2}, {0xABD0, 2}, {0x97D0, 2}, {0xA8D0, 2}, {0xADD0, 2}, {0xA9D0, 2}, {0xA7D0, 2}, {0xAAD0, 2}
};

struct utf_8_struct iso_8859_5[] =
{
{0x80C2, 2}, {0x81C2, 2}, {0x82C2, 2}, {0x83C2, 2}, {0x84C2, 2}, {0x85C2, 2}, {0x86C2, 2}, {0x87C2, 2}, {0x88C2, 2}, {0x89C2, 2}, {0x8AC2, 2}, {0x8BC2, 2}, {0x8CC2, 2}, {0x8DC2, 2}, {0x8EC2, 2}, {0x8FC2, 2}, 
{0x90C2, 2}, {0x91C2, 2}, {0x92C2, 2}, {0x93C2, 2}, {0x94C2, 2}, {0x95C2, 2}, {0x96C2, 2}, {0x97C2, 2}, {0x98C2, 2}, {0x99C2, 2}, {0x9AC2, 2}, {0x9BC2, 2}, {0x9CC2, 2}, {0x9DC2, 2}, {0x9EC2, 2}, {0x9FC2, 2}, 
{0xA0C2, 2}, {0x81D0, 2}, {0x82D0, 2}, {0x83D0, 2}, {0x84D0, 2}, {0x85D0, 2}, {0x86D0, 2}, {0x87D0, 2}, {0x88D0, 2}, {0x89D0, 2}, {0x8AD0, 2}, {0x8BD0, 2}, {0x8CD0, 2}, {0xADC2, 2}, {0x8ED0, 2}, {0x8FD0, 2}, 
{0x90D0, 2}, {0x91D0, 2}, {0x92D0, 2}, {0x93D0, 2}, {0x94D0, 2}, {0x95D0, 2}, {0x96D0, 2}, {0x97D0, 2}, {0x98D0, 2}, {0x99D0, 2}, {0x9AD0, 2}, {0x9BD0, 2}, {0x9CD0, 2}, {0x9DD0, 2}, {0x9ED0, 2}, {0x9FD0, 2},
{0xA0D0, 2}, {0xA1D0, 2}, {0xA2D0, 2}, {0xA3D0, 2}, {0xA4D0, 2}, {0xA5D0, 2}, {0xA6D0, 2}, {0xA7D0, 2}, {0xA8D0, 2}, {0xA9D0, 2}, {0xAAD0, 2}, {0xABD0, 2}, {0xACD0, 2}, {0xADD0, 2}, {0xAED0, 2}, {0xAFD0, 2},
{0xB0D0, 2}, {0xB1D0, 2}, {0xB2D0, 2}, {0xB3D0, 2}, {0xB4D0, 2}, {0xB5D0, 2}, {0xB6D0, 2}, {0xB7D0, 2}, {0xB8D0, 2}, {0xB9D0, 2}, {0xBAD0, 2}, {0xBBD0, 2}, {0xBCD0, 2}, {0xBDD0, 2}, {0xBED0, 2}, {0xBFD0, 2},
{0x80D1, 2}, {0x81D1, 2}, {0x82D1, 2}, {0x83D1, 2}, {0x84D1, 2}, {0x85D1, 2}, {0x86D1, 2}, {0x87D1, 2}, {0x88D1, 2}, {0x89D1, 2}, {0x8AD1, 2}, {0x8BD1, 2}, {0x8CD1, 2}, {0x8DD1, 2}, {0x8ED1, 2}, {0x8FD1, 2},
{0x9684E2, 3}, {0x91D1, 2}, {0x92D1, 2}, {0x93D1, 2}, {0x94D1, 2}, {0x95D1, 2}, {0x96D1, 2}, {0x97D1, 2}, {0x98D1, 2}, {0x99D1, 2}, {0x9AD1, 2}, {0x9BD1, 2}, {0x9CD1, 2}, {0xA7C2, 2}, {0x9ED1, 2}, {0x9FD1, 2}

};

struct utf_8_struct *p;

int
main(int argc, char *argv[])
{
	int	fd_input, fd_output; 
	int	i=0, n=0;
	int	charset=0;

	if (argc != 4){
		printf("Неверно введена команда\n"
			"Нужно ввести в таком формате:\n"
			 "\"./convert input_file charset output_file\"\n");
		return 1;
	}

	if (strcmp(argv[2], "cp1251") == 0)
		charset = 0;
	else if (strcmp(argv[2], "koi8-r") == 0)
                charset = 1;
	else if (strcmp(argv[2], "iso-8859-5") == 0)
                charset = 2;
	else {
		printf("%s\n", "Кодировка должны быть в одном из следующих форматов:\n"
				"cp1251, koi8-r, iso-8859-5");
                return 1;
	}

	if (access(argv[1], R_OK) < 0){
		printf("Файл %s недоступен для чтения\n", argv[1]);
		return 1;
	}

	if ((access(argv[3], F_OK) == 0) && (access(argv[3], W_OK) < 0)){

		printf("Файл %s недоступен для записи\n", argv[3]);
		return 1;
	}

	if ((fd_input = open(argv[1], O_RDONLY)) < 0){
		printf("Ошибка вызова open(чтение) для файла %s\n", argv[1]);
		return 1;
	}

	if ((fd_output = open(argv[3], O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR)) < 0){
		printf("Ошибка вызова open(запись) для файла %s\n", argv[3]);
		return 1;
	}

	if (fstat(fd_input, &statbuf) < 0){
                printf("Ошибка вызова функции fstat для файла %s\n", argv[1]);
                return 1;

	}

	if (!S_ISREG(statbuf.st_mode)){
		printf("Выбран необычный файл %s\n", argv[1]);
		return 1;
	}

	if (fstat(fd_output, &statbuf) < 0){
                printf("Ошибка вызова функции fstat для файла %s\n", argv[3]);
                return 1;

	}

	if (!S_ISREG(statbuf.st_mode)){
		printf("Выбран необычный файл %s\n", argv[3]);
		return 1;
	}


	while ((n = read(fd_input, buf_file, BUFFSIZE)) > 0){
		i = 0;
		while (i < n){

			if (buf_file[i] < (unsigned char) 0x80){
			
				if (write(fd_output, buf_file + i, 1) != 1)
					printf("%s %d\n", "Не записано элементов", errno);

			} else {

				if (charset == 0)
					p = cp1251;
				else if (charset == 1)
					p = koi8_r;
				else if (charset == 2)
					p = iso_8859_5;
				/*
				 *В указателе 'p' хранится ссылка на массив с нужной кодировкой
				 *Элементы в массиве расположены в порядке возрастания начисная с
				 *0x80, и можно по конвертируемому значение получить индекс
				 *Получив индекс сохраняем в указателе 'p' указатель на нужный
				 *элемент массива и далее отправляем этот указатель на запись
				 *с указанием длины
				 * */	
				p = p + (buf_file[i] - (unsigned char) 0x80);
				
				if (write(fd_output, p, p->length) != p->length)
					printf("%s %d\n", "Не записано элементов", errno);

			}

			i++;
		}
	}
	

	if (n < 0){
		printf("Ошибка чтения\n");
		return 1;
	}


	if (close(fd_input) < 0){
		printf("Ошибка вызова close(чтение) для файла %s\n", argv[1]);
		return 1;
	}

	if (close(fd_output) < 0){
		printf("Ошибка вызова close(запись) для файла %s\n", argv[3]);
		return 1;
	}

	return 0;

}
