/*  linux�ն���C���Ա����ϰ ������Ϸ �ɲο�rich_jpeg.jpg�е����ݽṹ 
 *  ��srcĿ¼��ִ��make, ִ��./rich ������Ϸ  ��������з�ʽ:�������նˣ����ն�1
 *  �µ�srcĿ¼��ִ��./rich -s ,���ն�2�µ�srcĿ¼��ִ��./rich -c,�ն�1��������,Ȼ��
 *  �ն�2���С�
 *
 *
 *  V0.1 ���һλ��ҵ����ߣ���land����symbol������ʱ����ʾ���� ��ӡ��ͼʱ�����ж�
 *       ������ʱ��ʾ��Ԫ�ء�2012.11.22
 *  V0.2 �޸�update_map���������4���л���ʾ �ڵ�ͼ����ʾ��ҷ�����ɫ������ÿ��ˢ
 *       ��land�е���ʾ���ţ������ܻ���ȷ�� 2012.11.22
 *  V0.3 ��Ӳ�ѯ�����Ϣ����player_info, ���game��id��player_num��غ������޸�    
 *  		update_map�е����������bug, ���buy_land������2012.11.23
 *  V0.4 ��ӵ�һ����˳�����player_quit(),������ӡ�����Ϣʱ��Ҫ��idΪ�������id��
 *  		�޸���deinit_game()��i�ķ�Χbug��
 *  V0.5 ��land��������������ߵ���ɫowner_color,�޸�init_land(),print_game_map()
 *		 ��������˳�������ʾ���ŵ�bug,�޸�print_game_map()��land��ʾbug���������
 *		 ���ļ�rich.c���޸�print_game_map()��ʹ�����ն˵��м��ӡ��ͼ��2012.11.25
 *  V0.6 ������util��д��print_bar������welcome(). ɾ��main�ж�����롣
 *  V0.7 ���roll����
 *  V0.8 ��дcmd_go�е�if-elseΪswitch����ʱû�뵽���õķ�ʽ������̬������ṹ�
 *  		���upgrade_house��������main����������ûɶ�ô������print_p_info �ɲ鿴
 *       ���ӵ�е������������޸��˳�ʱ����ʾ
 *  V0.9 ���tool��ع��ܣ�Ϊ������tool�ֶΣ�������ߣ��޸�print_p_info��bug����д
 *		 init_land();  2012.11.30
 *  V1.0 ��ӵ���ʹ����ؽṹ����cmd��ִ������
 *  V1.1 ���·��Block��ʹ�ú�����Block�Ĳ���2012.12.04
 *  V1.2 �޸�print_color��������tuil.h����ӵ���ʹ�õĿպ�������ֹ�δ���
 *  V1.3 �޸� window_col()�����л�ȡ��׼������ն˿�ȣ��Է�ֹ�����ض�����ܵõ���׼
 *       ����Ŀ�ȵ�bug�������õ�input.txt�ļ��У�������ֻ��һ���������Ҫ�����ո�
 *       ������������ȷ�����Է�ʽ ./rich.out < input.txt  �ض����������� ͬ����
 *       window_col()����ģ���#undef LINUX_CONSOLE��������ض���ʹ��diff�Բ��Խ����
 *       �ϡ�bug:Block����������ҵĵؿ���ʹ�á�
 *  V1.4 ��дsix_rand()������      
 *  V1.5 ���������غ�������/netĿ¼�£�./a.out�Ƿ��������������У�Ȼ��ֱ�����������
 *		 �ն���ִ��/srcĿ¼�µ�./rich.out -n��ʵ����λ��ҵĻ��� ,ʵ�ֿͻ�����Ҫ�޸ĵ�
 *		 ������:get_cmd_line();get_num();next_player(�����ַ�x);init();is_yes();
 *		 ���ȫ�ֱ���sockfd, game�ṹ�������turn��־λ�����Ƿ��ֵ��Լ���2012.12.14
 *	V1.6 ��дmakefile����ͬʱmake����ͷ�������д�����ǲ���׼�������������ķ�ʽ:��init
 *		 ������ʹ��fork���źš�ParseCmdLine����ȷ�������������ʽ:
 *		 	-server ������ -clinet �ͻ��� Ĭ�ϱ�������  
 *		 ��init��#define PLAYER_NUM 2�������������ͬʱ�޸�rich_server.c�е�wait_client(2);
 *		 ����ʱ�ն�����ҲҪ��Ӧ����		2012.12.21
 *	V1.7 �޸�4��ʱ�˳���bug����rich_server.c��Ϊwrite�������pipe�źŲ�����write_err
 *		 ,��ֹ��write��������ֱ����ֹ��
 *  V1.8 ���һЩע��
 * 	V1.9 ��fork�е�tell_parentŲ��execl������Ľ����У�ȥ����ʱ������ѡ������������ܡ�
 *  V2.0 �޸����߸�������ԭ��SʱԽ�磬��block����ʹ���е�����bug��2013.1.24
 * 		 open_gg@qq.com
 */

#include"console.h"


int main(int argc, char *argv[])
{

	if(init(ParseCmdLine(argc,argv))<0) return -1;

	print_bar("Welcome to Richman!",' ',Green);
	cmd_help(0,NULL);
	while(1)
	{
		playing();
	}

}



