#ifndef SAPPER_H_
#define SAPPER_H_

#define SAP_CELL_SIZE 20
#define SAP_HEIGHT_MENU 40
#include <stdio.h>
#include <stdlib.h>

enum
{ SAP_EASY, SAP_MIDDLE, SAP_HARD };

struct sap_cell
{
  bool check;
  bool bomb;
  bool open;
  int count_bomb;
};

typedef struct sap_cell sap_cell;

static sap_cell *sap_ptr_cells = NULL;

bool sap_geme_play = false;
static int sap_op = SAP_EASY;
static int sap_cols = 0;
static int sap_rows = 0;
static int sap_check_count = 0;
static int sap_bomb_count = 0;
static int sap_cell_close = 0;

float sap_x = 0, sap_y = 0;
ALLEGRO_BITMAP *sap_bitmap_default = NULL;
ALLEGRO_BITMAP *sap_bitmap_check = NULL;
ALLEGRO_BITMAP *sap_bitmap_bomp = NULL;
ALLEGRO_BITMAP *sap_bitmap_cells[9] = { 0 };

void sap_click(int button){

if (sap_geme_play == false)
return;

int i = sap_x % SAP_CELL_SIZE;
int y = (sap_y - SAP_HEIGHT_MENU) % SAP_CELL_SIZE;

if (button == 0){
//LEFT
/*открытие ячеек*/

} else if (button == 1) {
//RIGHT
/*установка/снятие флажков*/

}

        al_draw_bitmap (sap_bitmap, i * SAP_CELL_SIZE,
                        SAP_HEIGHT_MENU + j * SAP_CELL_SIZE, 0);
 

}

void
sap_add_count_bomb (int i, int j)
{

  if (i < 0 || j < 0 || i >= sap_cols || j >= sap_rows)
    return;

  (sap_ptr_cells + i * sap_cols + j)->count_bomb++;

}

void
sap_init (void)
{
  if (sap_ptr_cells)
    free (sap_ptr_cells);

  if (sap_op == SAP_EASY)
    {
      sap_cols = 3;
      sap_rows = 3;
    }
  else if (sap_op == SAP_MIDDLE)
    {
      sap_cols = 7;
      sap_rows = 7;
    }
  else if (sap_op == SAP_HARD)
    {
      sap_cols = 12;
      sap_rows = 12;
    }

  sap_ptr_cells =
    (sap_cell *) calloc (sap_cols * sap_rows, sizeof (sap_cell));

sap_geme_play = true;
  sap_bomb_count = 0;
  sap_check_count = 0;
  int i = 0, j = 0, l = 0, m = 0;
  bool r;
  for (i = 0; i < sap_rows; i++)
    for (j = 0; j < sap_cols; j++)
      {
	(sap_ptr_cells + i * sap_cols + j)->open = true;
	r = random () % 10 < 2;
	if (r)
	  {
	    (sap_ptr_cells + i * sap_cols + j)->bomb = true;
	    sap_bomb_count++;
	    for (l = i - 1; l < i + 2; l++)
	      for (m = j - 1; m < j + 2; m++)
		sap_add_count_bomb (l, m);

	  }

      }

  sap_cell_close = sap_cols * sap_rows - sap_bomb_count;

  printf ("%s\n", "init sapper");
}


void
sap_render (void)
{

  if (!sap_ptr_cells)
    return;

  int i = 0, j = 0;
  struct sap_cell *cell_ptr = NULL;
  ALLEGRO_BITMAP *sap_bitmap = NULL;

  for (i = 0; i < sap_rows; i++)
    for (j = 0; j < sap_cols; j++)
      {
	cell_ptr = sap_ptr_cells + i * sap_cols + j;

	if (cell_ptr->open)
	  {
	    if (cell_ptr->bomb)
	      sap_bitmap = sap_bitmap_bomp;
	    else
	      sap_bitmap = sap_bitmap_cells[cell_ptr->count_bomb];
	  }
	else if (cell_ptr->check)
	  sap_bitmap = sap_bitmap_check;
	else
	  sap_bitmap = sap_bitmap_default;

	al_draw_bitmap (sap_bitmap, i * SAP_CELL_SIZE,
			SAP_HEIGHT_MENU + j * SAP_CELL_SIZE, 0);
      }
}

int
sap_load_bitmap (void)
{

  int ret = 1;

  if (!(sap_bitmap_default = al_load_bitmap ("picture/default.bmp")))
    ret = 0;
  if (!(sap_bitmap_check = al_load_bitmap ("picture/check.bmp")))
    ret = 0;
  if (!(sap_bitmap_bomp = al_load_bitmap ("picture/bomb.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[0] = al_load_bitmap ("picture/0.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[1] = al_load_bitmap ("picture/1.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[2] = al_load_bitmap ("picture/2.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[3] = al_load_bitmap ("picture/3.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[4] = al_load_bitmap ("picture/4.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[5] = al_load_bitmap ("picture/5.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[6] = al_load_bitmap ("picture/6.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[7] = al_load_bitmap ("picture/7.bmp")))
    ret = 0;
  if (!(sap_bitmap_cells[8] = al_load_bitmap ("picture/8.bmp")))
    ret = 0;

  return ret;
}


void
sap_destroy (void)
{

  if (sap_ptr_cells)
    free (sap_ptr_cells);

  al_destroy_bitmap (sap_bitmap_default);
  al_destroy_bitmap (sap_bitmap_check);
  al_destroy_bitmap (sap_bitmap_bomp);


  for (int i = 0; i < 9; i++)
    al_destroy_bitmap (sap_bitmap_cells[i]);
}

#endif /* SAPPER_H_ */