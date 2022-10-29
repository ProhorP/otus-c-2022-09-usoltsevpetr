#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define BUFFSIZE 4096
#define SIZE_WORD 1024

typedef struct entry {
	char		*key;
	unsigned int	value;
} entry;

entry	**hash_table;
int	size_hash_table = 10;

unsigned long long	hashcode(unsigned char *str);
entry*			new_entry(char *user_string);
void			init_hash_table();
void			error_exit(char *str);
void			rebase_hash_table(char *str);
void			destroy_hash_table();
void			insert_hash_table(char *str);
void			print_hash_table();

int
main(int argc, char *argv[])
{

	if (argc != 2){
		printf("Неверно введена команда\n"
			"Нужно ввести в таком формате:\n"
			 "\"./count_words file\"\n");
		return 1;
	}

	int		fd;

	if ((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Ошибка вызова open(чтение) для файла %s\n", argv[1]);
		return 1;
	}

	int		i,j=0,n;
	unsigned char	buf_file[BUFFSIZE];
	char		buf_word[SIZE_WORD];

	init_hash_table();

	while ((n = read(fd, buf_file, BUFFSIZE)) > 0){
		i = 0;
		while (i < n){

			if (buf_file[i] == ' '){
				
				/*слова считаем по пробелам
				 * могут быть знаки препинания, но не зная кодировки файла
				 * нельзя трактовать смысл символов, поэтому 
				 * делаю максимавльно простой вариант*/
				buf_word[j] = '\0';//конец строки
				insert_hash_table(buf_word);
				/*обнуляем слово в буфере*/
				j=0;
				buf_word[0] = '\0';

			} else if (buf_file[i] != '\n') {
		
				buf_word[j] = buf_file[i];
	
				if (++j == SIZE_WORD)
					error_exit("Найдено слишком длинное слово");

			}
			i++;
		}
	}
	
	if (n < 0){
		printf("Ошибка чтения\n");
		return 1;
	}

	if (close(fd) < 0){
		printf("Ошибка вызова close для файла %s\n", argv[1]);
		return 1;
	}

	if (strlen(buf_word)){
                buf_word[j] = '\0';//конец строки
		insert_hash_table(buf_word);
	}

	print_hash_table();

	destroy_hash_table();

	return 0;

}

void
print_hash_table(){

	for (int i=0; i < size_hash_table; i++){
		if (hash_table[i])
			printf("Word: \"%s\", Count: %u\n", hash_table[i]->key, hash_table[i]->value);
	}

}

/*это придумал сам, чтобы для ключа отдельно не делать malloc*/
entry*
new_entry(char *str) {

	entry *e = (entry*)malloc(sizeof(entry) + strlen(str) + 1);

	if (e == NULL)
		error_exit("Не удалось выделить память под элемент таблицы");

	e->key = (char *) e + sizeof(entry);
	e->value = 1;
      	strcpy(e->key, str);	
	
	return e;

}

/*функцию взял из интернета*/
unsigned long long
hashcode(unsigned char *str) {
    unsigned long long hash = 5381;
    int c;
 
    if (size_hash_table == 1)
	    error_exit("Размер таблицы должен быть более 1/n");

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) ^ c;
    }
    return hash;
}

void
init_hash_table() {

	hash_table = (entry**) calloc(sizeof(entry*), size_hash_table);
	
	if (hash_table == NULL)
		error_exit("Не удалось выделить память под таблицу");

}

void
insert_hash_table(char *str) {

	if (!strlen(str))
		return;

	int index = (int) (hashcode((unsigned char*)str) % size_hash_table);

	int 	i = index;
	bool 	success = false;
	int 	red_line = size_hash_table;

	while (i < red_line){
		
		if (hash_table[i]){
			if (strcmp(hash_table[i]->key, str) == 0){
                    		hash_table[i]->value++;
				success = true;
				break;
			}
		} else {
	
			hash_table[i] = new_entry(str);
			success = true;
			break;
		}

		/*уходим на поиск с начала таблицы
		до вычисленного по hash-функции индекса
		*/
		if (++i == size_hash_table){
			i = 0;
			red_line = index;
		}
	}

	if (!success)
		rebase_hash_table(str);
}


void
rebase_hash_table(char *str){

	/*увеличим размер hash-таблицы в 2 раза*/
	int new_size_hash_table = size_hash_table << 1;

	entry ** new_hash_table = (entry**) calloc(sizeof(entry*), new_size_hash_table);
		
	if (new_hash_table == NULL)
		error_exit("Не удалось выделить память под таблицу");

	int	i=0, j=0, index=0, red_line=0;

	while (i < size_hash_table){
	
		index = (int) (hashcode((unsigned char*)hash_table[i]->key) % new_size_hash_table);
		red_line = new_size_hash_table;
		j = index;

		while (j < red_line){
		
			if (!new_hash_table[j]){
				new_hash_table[j] = hash_table[i];
				break;
			}
	
			/*уходим на поиск с начала таблицы
        	        до вычисленного по hash-функции индекса
               		 */
                	if (++j == new_size_hash_table){
                	        j = 0;
                        	red_line = index;
                	}

		}
			
		i++;
	}

	/*запоминаем новый размер таблицы*/
	size_hash_table = new_size_hash_table;

	/*очищаем старую таблицу указателей*/
	free(hash_table);

	/*переназначаем таблицу*/
	hash_table = new_hash_table;

	/*делаем повторную вставку элемента.
	 * Сейчас таблица увеличилась и операция вставки будет успешной*/
	insert_hash_table(str);

}

void
destroy_hash_table(){

	for (int i = 0; i < size_hash_table; i++){
	
		if (hash_table[i])
			free(hash_table[i]);

	}

	free(hash_table);

}

void
error_exit(char *str){

	printf("%s\n", str);
	exit(1);

}
