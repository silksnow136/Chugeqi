#pragma once
//人物对话显示，日后可以添加剧情分支管理
//负责管理玩家在角色场景中主动触发的对话
//药房锻造工呢管理
using namespace std;

void talkManager(int scene_id = 0);

void backGround_01();//垓下大帐
void backGround_02();//淮河
void backGround_03();//东城
void backGround_04();//乌江

void talkScene_01();//虞姬,小卒a
void talkScene_020();//农夫
void talkScene_021();//王翦，楚军
void talkScene_022();//副将，楚军
void talkScene_03();//钟离昧
void talkScene_04();//钟离昧，乌江亭长，众将士，韩信

//当前角色的对话
void talk_character_contnt_01(int current_character);
void talk_character_contnt_020(int current_character);
void talk_character_contnt_021(int current_character);
void talk_character_contnt_022(int current_character);
void talk_character_contnt_03(int current_character);
void talk_character_contnt_04(int current_character);

//药房
void phar();

//锻造
void forge();