#include<stdio.h>
#include"game.h"
#include<malloc.h>
#include<memory.h>
#include<stdlib.h>
#include "rich_client.h"
extern int sockfd;

void put_Block(st_game *,int);
void meet_Block(st_game *,int);

st_tool tool_tb[MAX_TOOL_NUM]={{'G',"Godwealth",0,500,Green,NeedU2realize,NeedU2realize},	//����
							   {'B',"Block",1,200,Red,put_Block,meet_Block},				//·��
							   {'C',"Control dice",2,200,Green,NeedU2realize,NeedU2realize},//ң������
							   {'\0',"",0,0,0,NULL,NULL},
							  };

st_land_info init_land_st={
	x:0,
	y:0,
	price:0,
	p_player:NULL,
	level:Level_0,
	color:None,
	owner_color:None,
	tool_n:0,
	symbol:0,
};

//��ʼ����Ϸ�ṹ�� ������ң���ͼ���ؿ���Ϣ
st_game * init_game(int player_num)
{
	st_game *p=malloc(sizeof(st_game));
	if(p==NULL) return NULL;
	set_game_pnum(p,player_num);
	set_game_id(p,0);
	
	int i;
	for(i=0;i<player_num;i++)
		if(((p->p_player_tab)[i]=init_player(i))==NULL) goto init_player_fail;

	if((p->p_map=init_map_info())==NULL) goto init_map_fail;
	init_shop(p->p_map);
	init_land(&(p->p_land));	
	clear_turnflag(p);
	return p;
	
init_map_fail:
init_player_fail:
	for(;i>=0;i--)
		deinit_player(&(p->p_player_tab)[i]);
			
	free(p);	
	return NULL;		
}

//������Ϸʱ������Ϸ�ṹ�� �ͷ���ؿռ�
void deinit_game(st_game **p)
{
	if(*p==NULL) return;
	int i=get_game_pnum(*p);
	for(;i>0;i--)
		deinit_player(&((*p)->p_player_tab)[i-1]);
	deinit_map_info(&((*p)->p_map));
	free(*p);
	*p=NULL;	
	
}



//��ӡ��ͼ
/*�д��Ż�
*aaaaaaa*
*a     a*
*a     a*
*aaaaaaa*
*/
void print_map_line(int num,int flag)
{
#ifdef LINUX_CONSOLE
	print_line(' ',num);
#else
	if(flag==1)
		printf("\n");
#endif

}


/*�����ж�land�е�symbol�Ƿ��з�������У���ӡland�е���ʱsymbol�������ӡ��ͼԪ��:
  ��ͼ����״
            aaaa
  		    b  d
  		    cccc
  ���ȴ�ӡa�У�Ȼ���ӡ�ڶ��е���height-1��(��b�д�d��)������ӡc��,ÿ��ǰ�����
  �ո��õ�ͼ���м���ʾ��land��һά���飬map�Ƕ�ά���飬�ɿ��ɽ�landΧ����map������
  �����ǵ�������һ���Ķ�Ӧ��ϵ��Ҳ�Ƿֳ��Ĳ��֡�
*/
void print_game_map(st_game *p)
{
	int i,j,k;
	st_land_info *pland=get_game_land(p,0);
	st_map_info *pmap=p->p_map;
	if(p==NULL) return;
	print_map_line(window_col()/2-WIDTH/2,0);
	for(i=j=0;j<WIDTH;j++)
		if(get_land_symbol(pland+j)!=0)	//���land������Ҫ��ʾ�ķ��� ����ʾ
			print_land(p,j);
		else
			print_color(get_map_e(pmap,i,j),get_land_ownclor(pland+j));//��ӡ��һ��a
	print_map_line(window_col()/2-WIDTH+WIDTH/2,1);
	//��ӡ�м��пո��a
	for(i=1,j=0;i<HEIGHT-1;i++)
	{
		k=WIDTH-2;
		print_map_line(window_col()/2-WIDTH/2,0);
		if(get_land_symbol(get_game_land(p,LAND_NUM-i))!=0)
			print_land(p,LAND_NUM-i);
		else
			print_color(get_map_e(pmap,i,0),get_land_ownclor(pland+LAND_NUM-i));//��ӡ��һ��
		while((k--)>0)
			printf(" ");
		if(get_land_symbol(get_game_land(p,WIDTH+i-1))!=0)
			print_land(p,WIDTH+i-1);
		else
			print_color(p->p_map->map[i][WIDTH-1],(p->p_land[WIDTH+i-1]).owner_color);//��ӡ���һ��
		print_map_line(window_col()/2-WIDTH+WIDTH/2,1);
	}

	print_map_line(window_col()/2-WIDTH/2,0);
	for(i=HEIGHT-1,j=0;j<WIDTH;j++)
		if(get_land_symbol(get_game_land(p,LAND_NUM-i-j))!=0)
			print_land(p,LAND_NUM-i-j);
		else	
			print_color(p->p_map->map[i][j],(p->p_land[LAND_NUM-i-j]).owner_color);//��ӡ���һ��a	
	print_map_line(window_col()/2-WIDTH+WIDTH/2,1);	

}



//��ʾ��ͼ�ϵ���� ������ص�ʱ����ʾ��ǰ���
//�л���Һ�ʹ��update_map(game,0),��ʾ��ǰ���
int update_map(st_game *p,int n)
{
	int i=0,tmp;
	int player_num=get_game_pnum(p);
	st_land_info *land;
	//���³���ǰ��������ҵĵ�ͼ��ʾ ��Ϊ����ƶ���������������ҷ���
	for(;i<player_num-1;i++)
	{
		st_player *player=get_playern(p,(get_p_id(p)+i+1)%player_num);
		tmp=get_player_step(player);
		land=get_game_land(p,tmp);
		set_land_symbol(land,get_player_symbol(player));
		set_land_color(land,get_player_color(player));
	}

	//���µ�ǰ��ҵ���ʾ
	int step=get_p_step(p);	//������ҵĲ���
	land=get_game_land(p,step);
	if(get_land_symbol(land)== get_p_symbol(p))	//�����ǰλ��ֻ���Լ� ��ָ�Ϊ��
	{
		set_land_symbol(land,0);	//���land�ϵķ���
		set_land_color(land,None);	//���land����ɫ
	}

	int new_step=(n+step>=0) ? (n+step)%LAND_NUM : (LAND_NUM+(n+step)%LAND_NUM);  //����ģ�� ��Щ������Ϊ�� gccΪ����
	int x=0,y=0;
	get_n_pos(new_step,&x,&y);		//����x=y=0������������
	//printf("x=%d,y=%d,pos=%d\n",x,y,new_step);
	set_p_pos(p,x,y);				//������ҵ����� 
	set_p_step(p,new_step);			//����������ԭ���߹��Ĳ��� ������land[]
	land=get_game_land(p,new_step);	//����ߵ���λ��
	set_land_symbol(land,get_p_symbol(p));	//������λ��land����ʾ�ķ���
	set_land_color(land,get_p_color(p));	//���õؿ���ɫ
	return 0;
}

int buy_land(st_game *game,st_land_info *land)
{
	//�������ؼ�ֵ
	int x=0,y=0;
	get_p_pos(game,&x,&y);
	//st_land_info *land=get_game_land(game,pos);
	int init_vlaue=get_map_value(game->p_map,x,y);
	init_vlaue*=100;
	if(get_land_price(land)!=0) init_vlaue=get_land_price(land);	//�������˳���ҵ�����
	//��� ����land map p->money
	int money=get_p_money(game);
	if( money < init_vlaue)
	{
		printf("Sorry,your money %d are not enough to buy land that worth %d\n",money,init_vlaue);
		return 1;
	}
	if(is_myturn(game))
		printf("land worth is $%d, Do you want to buy land? Y-N\n",init_vlaue);
	
	if(is_yes())
	{
		set_p_money(game,money-init_vlaue);	
		set_land_player(land,get_player(game));
		set_land_price(land,init_vlaue);
		set_land_ownclor(land,get_p_color(game));
		print_name(game);
		printf(" spend %d on land\n",init_vlaue);
	}
	return 0;
}

int pay_rent(st_game *game,st_land_info *pland)
{
	//����Ӧ�����
	//int step=get_p_step(game);
	//st_land_info *pland=get_game_land(game,step);
	st_player *player_pay=get_player(game);
	st_player *house_owner=get_land_player(pland);
	//p1->money-- p2->money++
	int rent=get_land_price(pland)/2;
	int player_money=get_player_money(player_pay);
	
	printf("%s pay $%d to %s for rent\n",get_player_name(player_pay),\
		   rent,get_player_name(house_owner));
	if(player_money < rent)
	{
		rent=player_money;
		printf("player %s bankrupt!\n",get_player_name(player_pay));
		player_quit(game,get_player_id(player_pay));
	}
	else
	{
		set_player_money(player_pay,player_money-rent);
	}
	
	set_player_money(house_owner,get_player_money(house_owner)+rent);
	return 0;
		
	//p1�Ʋ�? bankrupt(game,id);
}

int game_over(st_game *game)
{
	//ֻʣһ�ˣ���Ϸ����
	//int win_col=window_col();
	char name[MAX_NAME_LEN+10]={0};
	sprintf(name,"%s Win",get_p_name(game));
	print_bar(name,'=',get_p_color(game));
	//���������table��0λ��
	print_p_info(game,0);
	print_bar("Game Over!",'=',Red);
	deinit_game(&game);
	exit(0);
}


//�������st_player�е�id
void player_quit(st_game *game,int id)
{
	//ɾ��game�е�ָ�� ��̵�ָ��ǰ�� �����-- 
	int player_num=get_game_pnum(game);
	st_player *player;
	st_land_info *pland=get_game_land(game,0);
	int i,j,player_id;
	char msg[MAX_NAME_LEN+64];

	//��playertable�в������
	for(i=0;i<player_num;i++)
	{
		player=get_playern(game,i);
		player_id=get_player_id(player);
		if(player_id==id)
		{	
			sprintf(msg,"player %s quit!\n",get_player_name(player));
			print_color_str(msg,get_player_color(player));
			pland=get_game_land(game,get_player_step(player));
			set_land_symbol(pland,0);
			set_land_color(pland,None);
			//�����ع黹ϵͳ
			pland=get_game_land(game,0);
			for(j=0;j<LAND_NUM;j++)
				if(get_land_player(pland+j)!=NULL && get_player_id(get_land_player(pland+j))== player_id)
					set_land_player(pland+j,NULL);
			deinit_player(&player);
			break;
		}
	}

	//table�����һ����Ҳ����ƶ�ָ�룬�����idλ��
	if(i==(player_num-1))
		set_game_id(game,0);
	else
	{
		for(;i<player_num-1;i++)
			((game)->p_player_tab)[i]=((game)->p_player_tab)[i+1];		
	}
	set_game_pnum(game,--player_num);

	if(player_num == 1)
		game_over(game);

}

e_LandOwnerType whose_land(st_game *game,st_land_info *land)
{
	int x,y;
	st_player *player=get_player(game);
	get_player_pos(player,&x,&y);
	char e=get_map_e(game->p_map,x,y);
	if(e==SHOP) return Toolshop;
	
	player=get_land_player(land);
	if(player == NULL) return Na;
	if(get_player_id(player)==get_p_id(game)) 
		return Owner;
	else 
		return Other;
	
}
int upgrade_house(st_game *game,st_land_info *land)
{
	int p_money=get_p_money(game);
	int price=get_land_price(land);
	House_type level=get_land_level(land);
	if(p_money < price || level >= (MAX_LEVEL-1)) return 0;
	if(is_myturn(game))
		printf("Do you want to upgrade your house? Y-N\n");	
	if(is_yes())
	{
		set_land_level(land,level+1);
		set_land_price(land,price*2);
		set_p_money(game,p_money-price);
		printf("house level%d!\n",level+1);
	}
	return 0;
}
void print_game_info(st_player *player,int landnum)
{
	print_player_info(player);
	printf("    land=%-3d",landnum);
	printf("\n");
}

//��ӡplayer table[index]�������Ϣ  id��player table�е�����
void print_p_info(st_game *game,int index)
{
	st_land_info *land=get_game_land(game,0);
	st_player *player;
	int land_num[MAX_PLAYER_NUM];
	memset(land_num,0,sizeof(land_num));
	int i;
	//����ÿ����ҵ�������
	for(i=0;i<LAND_NUM;i++)
	{
		player=get_land_player(land+i);
		if(player!=NULL)
			land_num[get_player_id(player)]++;//���idΪ����
	}

	if(index==9)//print all player
		for(i=0;i<get_game_pnum(game);i++)
		{
			player=get_playern(game,i);
			print_game_info(player,land_num[get_player_id(player)]);
		}
	else
	{
		i=get_player_id(get_playern(game,index));
		print_game_info(get_playern(game,index),land_num[i]);
	}
	
	
}

int  is_yes()
{
	char buf[MAXLINE];
	if(sockfd!=-1)
	{
		client_run(sockfd,buf);
		if(buf[0]=='y' || buf[0]=='Y')
			return 1;
		else
			return 0;
	}
	else
	{
#if 1
		char c;
		scanf("%c",&c);
		while(getchar()!='\n');
		if(c=='y' || c=='Y')
			return 1;
		else
			return 0;
#endif
	}
}

int get_num()
{
	char buf[MAXLINE];
	if(sockfd!=-1)
	{
		client_run(sockfd,buf);
		char *str=buf;
		int ret=0;
		while(*str!='\0' && !is_digtial(*str)) str++;
		ret=str2int(str);
		if(str!=buf && *(str-1)=='-') 
			return -ret;
		return ret;
	}
	else
	{
	#if 1
		int num;
		scanf("%d",&num);
		while(getchar()!='\n');
		return num;
		#endif
	}

}


void buy_tool(st_game *game)
{
	st_player *player=get_player(game);
	int myturn=is_myturn(game);
	if(myturn)
	{
		printf("Input the tool's id and number you want to buy\n");
		printf("id=");
	}
	int id,num,money,price;
	id=get_num();
	if(myturn)
		printf("number=");
	num=get_num();
	if(id >= MAX_TOOL_NUM || id < 0 || num<=0)
		return;
	money=get_player_money(player);
	price=tool_tb[id].price;
		
	if(num*price > money)//Ǯ���� ��С��num��
		num=money/price;
	if(get_player_tlnum(player)+num > MAX_TOOL_NUM)
		num=MAX_TOOL_NUM-get_player_tlnum(player);
	money-=price*num;
	set_player_tlnum(player,get_player_tlnum(player)+num);
	set_player_money(player,money);
	int i;
	for(i=0;i<num;i++)
		add_player_tool(player,id);
	
	print_name(game);
	printf(" get %d %s,cost %d\n",num,tool_tb[id].name,price*num);
	if(myturn)
		printf("get more? Y-N\n");
}

void display_tool()
{
	int i;
	for(i=0;i<MAX_TOOL_NUM && tool_tb[i].symbol!='\0';i++)
		printf("Id:%d %c:tool name:%-15s price:%-4d\n",i,tool_tb[i].symbol,\
				tool_tb[i].name,tool_tb[i].price);
}
void shop(st_game *game)
{
	if(is_myturn(game))
	{
		display_tool();
		printf("Do you want to buy tools? Y-n\n");
	}
	while(1)
	{
		if(is_yes())
			buy_tool(game);
		else
		break;
	}
	return;	
}

char * get_tool_name(int id)
{
	return tool_tb[id].name;
}
//��ʾ���ӵ�еĵ��� ����
void display_mytool(st_player *player)
{
	
	int tool_numtable[MAX_TOOL_NUM]={0};
	int id=0;
	int tool_num=0;
	for(tool_num=0; tool_num < MAX_TOOL_NUM; tool_num++)
		if((id=get_player_tln(player,tool_num))!=MAX_TOOL_NUM)
			tool_numtable[id]++;

	for(id=0; id < MAX_TOOL_NUM ; id++)
		if(tool_numtable[id]!=0)
			printf("%d.%s:%d  ",id,get_tool_name(id),tool_numtable[id]);
	printf("\n");
	
}
void use_tool(st_game *game)
{
	st_player *player=get_player(game);
	if(get_player_tlnum(player)==0)	
	{
		printf("sorry, you have no tools\n");
		return;
	}
	display_mytool(player);
	int index;
	printf("pls enter the tool's id you want to use, 9 to return\n");
	int id=get_num();
	if(9 == id) return;
	for(index=0; index < MAX_TOOL_NUM ; index++)
	{
		if(get_player_tln(player,index)==id)
		{
			tool_tb[id].usetool(game,0);
			del_player_tool(player,id);
			break;
		}
	}	
			
}	
#define blockID 1
//������λ�õ�num����·��
void put_Block(st_game *game,int x)
{
	//int blockID=tool_tb[1].id;
retry:
	if(is_myturn(game))
		printf("pls enter the step(+-10) you want to block: ");
	int num=get_num();
	if(num>10 || num <-10) goto retry;
	
	//���õؿ���ʾ �ؿ�tool�ṹ ��meettoolʱ������ز���
	int step=get_p_step(game);
	int new_step=(num+step>=0) ? (num+step)%LAND_NUM : (LAND_NUM+(num+step)%LAND_NUM);

	st_land_info *land=get_game_land(game,new_step);
	
	int tool_num=get_land_tlnum(land);
	if(tool_num==MAX_TOOL_NUM) 
	{
		printf("no room to put tools\n");
		return;
	}
	set_land_symbol(land,tool_tb[blockID].symbol);
	set_land_color(land,tool_tb[blockID].color);
	add_land_tool(land,blockID);
	printf("player %s put block at %d\n",get_p_name(game),new_step);
}

//̽����ҵ�ǰn���Ƿ���·�� ��������·�Ϻ��߹��Ĳ���
int explore_block(st_game *game,int n)
{
	int current_step=get_p_step(game);//��ҵ�ǰ����
	st_land_info *land;
	int i;
	int onestep=n>=0?1:-1;
	int steps=0;
	int new_step=0;
	for(i=n>=0?n:-n ; i >=0 ; i--)
	{
		new_step=(steps+current_step>=0) ? (steps+current_step)%LAND_NUM : (LAND_NUM+(steps+current_step)%LAND_NUM);
		land=get_game_land(game,new_step);
		if(get_land_symbol(land) == tool_tb[blockID].symbol)
		{
			tool_tb[blockID].meettool(game,new_step);
			return steps;
		}
		steps+=onestep;
	}
	return n;
}

//�����ͼ�ϵ�symbol��'B' ������·��
void meet_Block(st_game *game,int n)
{
	printf("you meet a block!\n");
}

//��put_block meet_block��ʵ������
void NeedU2realize(st_game *game,int n)
{
	printf("this function need you to reailize!\n");
}

void next_player(st_game *game)
{
	int next=(get_game_id(game)+1)%get_game_pnum(game);
	set_game_id(game,next);
	char des_buf[MAXLINE]={'x','\n'};//���ַ�'x'��ʾֹͣ���λغ�����
	if(sockfd == -1) return;
	#if 1
	if(is_notice(sockfd)) //������֪ͨ 
	{
		printf("next player\n");
		Writen(sockfd, des_buf, 3);
		
	}
	download_cmd(sockfd,des_buf);
	clear_turnflag(game);
	#endif
	//client_run(sockfd,des_buf);
}


void print_land(st_game *p,int step)
{
	st_land_info *land=get_game_land(p,step);
	print_color(get_land_symbol(land),land->color);//��ӡ�ؿ���ʱԪ�ص���ɫ
}


//��ʼ���ؿ���Ϣ
void init_land(st_land_info (*p_land)[])
{

	int i;
	for(i=0;i<LAND_NUM;i++)
	{
		*(*p_land+i)=init_land_st;
	}

}

void init_shop(st_map_info *map)
{
	int x1,y1,x2,y2;
	x1=0;
	y1=WIDTH/2;
	x2=HEIGHT-1;
	y2=WIDTH/2;
	set_map_e(map,SHOP,x1,y1);
	set_map_e(map,SHOP,x2,y2);
}



/****************************��game�ṹ����land�����ķ�װ**************************/

//�ؿ�ṹ����غ���
void set_land_player(st_land_info *land,st_player *p_player)
{
	land->p_player=p_player;
}

//��ȡ���ص�������
st_player * get_land_player(st_land_info *land)
{
	return land->p_player;
}

//����������ɫ
void set_land_color(st_land_info *land,e_color color)
{
	land->color=color;
}

//��ȡ������ɫ
e_color get_land_color(st_land_info *land)
{
	return land->color;
}

//�������ؼ۸�
void set_land_price(st_land_info *land,int price)
{
	land->price=price;
}

int get_land_price(st_land_info *land)
{
	return land->price;
}

//�������صȼ�
void set_land_level(st_land_info *land,House_type level)
{
	land->level=level;
}

House_type get_land_level(st_land_info *land)
{
	return land->level;
}

//��ȡ��step������
st_land_info * get_game_land(st_game *p,int step)
{
	return p->p_land+step;
}

//����������ʱ��ʾ����
void set_land_symbol(st_land_info *land,char symbol)
{
	land->symbol=symbol;
}

char get_land_symbol(st_land_info *land)
{
	return land->symbol;
}

//�ؿ���ɫ��������һ��
e_color get_land_ownclor(st_land_info *land)
{
	return land->owner_color;
}

void set_land_ownclor(st_land_info *land,e_color color)
{
	land->owner_color=color;
}

//��ȡ�����ϵ��ߵ�����
int get_land_tlnum(st_land_info *land)
{
	return land->tool_n;
}
void set_land_tlnum(st_land_info *land,int num)
{
	land->tool_n=num;
}

//���������ϵĵ�index�ĵ��߱��id
void set_land_tln(st_land_info *land,int index,int id)
{
	land->tools[index]=id;
}
int get_land_tln(st_land_info *land,int index)
{
	return land->tools[index];
}

//�����������ӵ���
void add_land_tool(st_land_info *land,int id)
{
	int num=get_land_tlnum(land);	
	if(num == MAX_TOOL_NUM) printf("no room to put tools\n");

	set_land_tln(land,num,id);
	set_land_tlnum(land,num+1);
}


/****************************��game�ṹ����player�����ķ�װ**************************/

//��ȡ������� linux���ն���ʱ������ʾ����
char * get_p_name(st_game *p)
{
	return get_player_name((p->p_player_tab)[p->id]);
}

//��ȡ���id
int get_p_id(st_game *p)
{
	return get_player_id((p->p_player_tab)[p->id]);
}

//��ȡ��ҵ�ͼ�ϵ�λ��
void get_p_pos(st_game *p,int *x,int *y)
{
	get_player_pos((p->p_player_tab)[p->id],x,y);
}

//�������λ��
void set_p_pos(st_game *p,int x,int y)
{
	set_player_pos((p->p_player_tab)[p->id],x,y);	
}

//��ȡ��ҽ�Ǯ��
int get_p_money(st_game *p)
{
	return get_player_money((p->p_player_tab)[p->id]);	
}


//������ҽ�Ǯ
void set_p_money(st_game *p,int money)
{
	set_player_money((p->p_player_tab)[p->id],money);
}

//��ȡ��ҵ�ͼ�ϵı�־
char get_p_symbol(st_game *p)
{
	return get_player_symbol((p->p_player_tab)[p->id]);	
}

//��ȡ������������Ĳ���
int get_p_step(st_game *p)
{
	return get_player_step((p->p_player_tab)[p->id]);
}

void set_p_step(st_game *p,int step)
{
	set_player_step((p->p_player_tab)[p->id],step);	
}

//��ȡ�����ɫ
e_color get_p_color(st_game *p)
{
	return get_player_color((p->p_player_tab)[p->id]);	
}

//��ȡ��ǰ���ָ��
st_player *get_player(st_game *p)
{
	return (p->p_player_tab)[p->id];
}

//��ȡ��ұ��е�index����ҵ�ָ��
st_player *get_playern(st_game *p,int id)
{
	return (p->p_player_tab)[id];
}

//��ȡ��ǰp_player_tab[]�е�������
int get_game_id(st_game *p)
{
	return p->id;
}

//���õ�ǰ��ұ�������
void set_game_id(st_game *p,int id)
{
	p->id=id;
}

//��ȡ�������
int get_game_pnum(st_game *p)
{
	return p->player_num;
}

//�����������
void set_game_pnum(st_game *p,int num)
{
	p->player_num=num;
}

//�ж��Ƿ��ֵ��Լ�
int is_myturn(st_game *p)
{
	if(sockfd==-1) return 1;
	return p->my_turnflag;
}

//���turn��־
void clear_turnflag(st_game *p)
{
	set_game_turn(p,0);
}

//����turn��־
void set_game_turn(st_game *p,int turn)
{
	p->my_turnflag=turn;
}

//��ӡ��ǰ�������
void print_name(st_game *p)
{
	print_color_str(get_p_name(p),get_p_color(p));
}



