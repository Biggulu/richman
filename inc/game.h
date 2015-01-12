#ifndef GAME_H
#define GAME_H
#include"map.h"
#include"player.h"
#include"util.h"
#if 0	//����С��ͼ
#ifdef WIDTH
#undef WIDTH
#define WIDTH WIDTH_S
#endif
#ifdef HEIGHT
#undef HEIGHT
#define HEIGHT HEIGHT_S
#endif
#endif 
#define LAND_NUM (WIDTH*2+(HEIGHT-2)*2)
typedef struct land{
	int x,y;	//pos
	int price;					//���صļ۸�
	st_player *p_player;		//���ص�������
	e_color owner_color;		//���������ߵ���ɫ
	House_type level;			//���صĵȼ�	
	e_color color;				//���ص���ʱ��ɫ ��վ������������ɫһ��
	int tool_n;					//�����ϵ��ߵ�����
	int tools[MAX_TOOL_NUM];	//�����ϵĵ���
	char symbol;				//������ʱ��ʾ�ķ���
	
}st_land_info;

typedef struct game{
	st_player *p_player_tab[MAX_PLAYER_NUM];	//���table ������ָ�뻹�ǽṹ����أ�
	st_map_info *p_map;							//��ͼָ��
	st_land_info p_land[LAND_NUM];				//�ؿ�ṹ��
	int player_num;								//����� ==1�����ʤ�� ��Ϸ����
	int id;										//p_player_tab�е����� ָ��ǰ���
	int my_turnflag;							//������еı�־ 1:my turn 0:others turn
}st_game;

#ifndef MAX_TOOL_NUM
#define MAX_TOOL_NUM 10
#endif
#define SHOP 'H'
typedef struct tool{
	char symbol;	//��־
	char *name;		//����
	int id;			//id
	int price;		//�۸�
	e_color color;	//��ɫ
	void (*usetool)(st_game *,int);	//ʹ�õ���
	void (*meettool)(st_game *,int); //���������������ߵ�Ч�� ��������·��
}st_tool;


/*ʹ�þ�̬��ȡ�������if-else?*/
enum land_owner_type{Na=0,Owner,Other,Toolshop};
typedef enum land_owner_type e_LandOwnerType;

#define MAX_LAND_FUN 50
//land function table struct
struct land_ftbl_s{
	e_LandOwnerType owner_type;
	int (*f)(st_game *,int);
};
typedef struct land_ftbl_s	land_ftbl_t;

//��game�ṹ����ֱ�ӻ�ȡ��ǰ��ҵ���Ϣ
int  get_p_money(st_game *p);
int  get_p_id(st_game *p);
int  get_p_step(st_game *p);
void get_p_pos(st_game *p,int *x,int *y);
void set_p_pos(st_game *p,int x,int y);
void set_p_money(st_game *p,int money);
char get_p_symbol(st_game *p);
char *get_p_name(st_game *p);
e_color get_p_color(st_game *p);
void set_p_step(st_game *p,int step);

//��ӡ��ͼ��غ���
int  update_map(st_game *p,int n);
void print_game_map(st_game *p);
void print_name(st_game *p);
void print_land(st_game *p,int );
void print_p_info(st_game *game,int index);

//��ʼ��
void init_land(st_land_info (*p_land)[]);
void init_shop(st_map_info *map);
void deinit_game(st_game **p);
st_game * init_game(int player_num);


//land�ṹ����غ���
int  get_land_price(st_land_info *land);
int  get_land_tlnum(st_land_info *land);
void set_land_player(st_land_info *land,st_player *p_player);
void set_land_color(st_land_info *land,e_color color);
void set_land_price(st_land_info *land,int price);
void set_land_level(st_land_info *land,House_type level);
void set_land_symbol(st_land_info *land,char symbol);
void set_land_ownclor(st_land_info *land,e_color color);
char get_land_symbol(st_land_info *land);
e_color get_land_color(st_land_info *land);
e_color get_land_ownclor(st_land_info *land);
st_player *get_land_player(st_land_info *land);
House_type get_land_level(st_land_info *land);
e_LandOwnerType whose_land(st_game *game,st_land_info *land);
void add_land_tool(st_land_info *land,int id);

//game�ṹ����غ���
int  get_game_pnum(st_game *p);
int  get_game_id(st_game *p);
void set_game_id(st_game *p,int id);
void set_game_pnum(st_game *p,int num);
void next_player(st_game *game);
st_player *get_player(st_game *p);
st_player *get_playern(st_game *p,int id);
st_land_info *get_game_land(st_game *p,int );

//cmd��غ���
int  buy_land(st_game *game,st_land_info *land);
int  upgrade_house(st_game *game,st_land_info *land);
int  is_yes();
int  pay_rent(st_game *game,st_land_info *pland);
void player_quit(st_game *game,int id);
void shop(st_game *game);
void use_tool(st_game *game);
int  explore_block(st_game *game,int n);
void meet_Block(st_game *game,int x);
void NeedU2realize(st_game *game,int n);
int get_num();
int is_myturn(st_game *p);
void clear_turnflag(st_game *p);
void set_game_turn(st_game *p,int turn);




#endif
