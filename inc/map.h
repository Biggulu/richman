#ifndef MAP_H
#define MAP_H

#define WIDTH 8
#define HEIGHT 5 //map��valueҲҪ�����޸�
#define WIDTH_S 7
#define HEIGHT_S 4 //map��valueҲҪ�����޸�
typedef struct {
	char map[HEIGHT][WIDTH];	//��ͼ��Ϣ
	int (*value)[WIDTH];		//�ؿ��ʼ��ֵ��
	int x,y;					//��ǰ�α�λ��cur
}st_map_info;

typedef enum House{Level_0=0,Level_1,Level_2,Level_3}House_type;
#define MAX_LEVEL 4
st_map_info * init_map_info();
void deinit_map_info(st_map_info **p);
void print_map(st_map_info *p);
void get_n_pos(int n,int *x,int *y);
void set_map_cur(st_map_info *p,int x,int y);
void get_map_cru(st_map_info *p,int *x,int *y);
void set_map_e(st_map_info *p,char e,int x,int y);
char get_map_e(st_map_info *p,int x,int y);
int  get_map_value(st_map_info *p,int x,int y);
#endif