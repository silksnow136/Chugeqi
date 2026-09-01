#include<iostream>
#include"SceneManager.h"
#include"Game.h"
#include <conio.h> // 控制台无回显按键读取
#include"map.h"
#include"StroyManager.h"

SceneManager::SceneManager(Game& game): game(game)
{
	current_scene_id = 0;//剧情初步存档,显示当前场景id
	current_state = SceneState::ORIGIN_SCENE;
	current_character = 0;

	autoPlay = false;//判断是否自动播放剧情
	unique_map_print = false;
}

bool SceneManager::current_Auto() {
	return autoPlay;
}

void SceneManager::changeAuto() {
	autoPlay = !autoPlay;
}

void SceneManager::deleteWords(string tip) {
	// 以下是让提示【按任意键继续对话】消失
	for (int i = 0; i < tip.length(); i++) {
		cout << '\b'; // \b是退格符，循环提示长度的次数，使光标到达提示之前
	}
	// 用空格覆盖所有残留字符，再退回到行首
	cout << string(tip.length(), ' ');
	for (int i = 0; i < tip.length(); i++) {
		cout << '\b';
	}
}
void SceneManager::nextLine() {
	if (autoPlay) {
		string tip = "  【按ESC手动对话】";
		cout << tip;
		Sleep(1500);//1500ms后播放下一条剧情
		deleteWords(tip);
		//_kbhit()非阻塞检测按键输入
		if (_kbhit()) {
			int key;
			key = _getch(); // 读取键盘按键，但不是显示键盘输入
			if (key == 27) {
				changeAuto();
			}
		}
	}
	else {
		string tip = "  【按ESC自动对话，按其他键继续对话】";
		cout << tip;
		int key;
		key=_getch(); // 读取键盘按键，但不是显示键盘输入
		if (key == 27) {
			changeAuto();
		}
		deleteWords(tip);
	}
	
}

int SceneManager::showScene_id() {
	return current_scene_id;
}

void SceneManager::changeScene(int scene_id)
{
	// 离开旧场景
	current_scene_id = scene_id;
	// 进入新场景时重置状态
	current_state = SceneState::ORIGIN_SCENE;
	// 重置对话人物
	current_character = 0;
	// 允许新场景重新打印地图
	unique_map_print = false;
}

//场景功能显示管理
void SceneManager::showSceneManager(int scene_id, int branch_id) {
	switch (scene_id) {
	case 1:
		map_Manager(scene_id, branch_id);
		setColor(14);
		sceneManager(game, branch_id);
		break;

	case  2:
		map_Manager(scene_id, branch_id);
		setColor(14);
		sceneManager(game, branch_id);
		break;

	case 3:
		map_Manager(scene_id, branch_id);
		setColor(14);
		sceneManager(game, branch_id);
		break;

	case 4:
		map_Manager(scene_id, branch_id);
		setColor(14);
		sceneManager(game, branch_id);
		break;

	default:
		break;
	}
}


void choiceList_01() {
	cout << "1. 对话\n";
	cout << "2. 药房\n";
	cout << "3. 锻造\n";
	cout << "\n输入w继续游戏\n";
}
//场景功能管理,1对话系统+命令系统；2药店系统；3锻造系统
void SceneManager::sceneManager(Game& game1, int branch_id) {
	string sceneCommand;
	bool choice=true;
	int key1;//吃掉对话结束后的输入，方便返回上级交谈系统
	string tip1 = "输入任意按键返回";
	while (choice) {
		switch (current_state) {

		//大世界场景
		case SceneState::ORIGIN_SCENE:

			//清屏
			system("cls");
			map_Manager(showScene_id(), branch_id);

			switch (showScene_id()) {
			case 1:
				backGround_01();
				break;
			case 2:
				backGround_02();
				break;
			case 3:
				backGround_03();
				break;
			case 4:
				backGround_04();
				break;
			}
			
			choiceList_01();

			cout << "\n> ";
			cin >> sceneCommand;
			if (sceneCommand == "1" || sceneCommand == "2" || sceneCommand == "3") {
				int num = stoi(sceneCommand);//将string转化为int
				switch (num) {
				case 1:
					//进入对话
					current_state = SceneState::TALK;
					break;

				case 2:
					//进入药店
					current_state = SceneState::PHARMACY;
					//药店未制作！！！！！！！！！！！！！！！！！！！！！
					//不要在这里添加，这里只负责进入药店系统
					break;

				case 3:
					// 进入锻造
					current_state = SceneState::FORGE;
					//锻造未制作！！！！！！！！！！！！！！！！
					//不要在这里添加，这里只负责进入锻造系统
					break;
				}
			}
			else {
				if (sceneCommand == "south" || sceneCommand == "w" || sceneCommand == "W"
					|| sceneCommand == "north" || sceneCommand == "n" || sceneCommand == "N"
					|| sceneCommand == "quit" || sceneCommand == "start") {
					choice = false;

				}

				//清屏
				system("cls");

				game1.gameCommand(sceneCommand);
				if (choice) {
					cout << tip1;
					key1 = _getch();
					deleteWords(tip1);
				}
			}
			break;

		// 选择对话人物
		case SceneState::TALK:
			
			//清屏
			system("cls");
			map_Manager(showScene_id(), branch_id);

			switch (showScene_id()) {
				//第一幕
			case 1:
				talkScene_01();

				cout << "\n> ";
				cin >> sceneCommand;
				if (sceneCommand == "1" || sceneCommand == "2" || sceneCommand == "3") {
					int num = stoi(sceneCommand);//将string转化为int
					switch (num) {
					case 1:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与小卒a对话
						current_character = 1;
						talk_character_contnt_01(current_character);

						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);

						break;

					case 2:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与虞姬对话
						current_character = 2;
						talk_character_contnt_01(current_character);
	
						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);

						break;

					case 3:
						//返回
						current_state = SceneState::ORIGIN_SCENE;
						break;
					}
				}
				else {

					if (sceneCommand == "south" || sceneCommand == "w" || sceneCommand == "W"
						|| sceneCommand == "north" || sceneCommand == "n" || sceneCommand == "N"
						|| sceneCommand == "quit" || sceneCommand == "start") {
						choice = false;

					}

					//清屏
					system("cls");

					game1.gameCommand(sceneCommand);
					if (choice) {
						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);
					}
				}
				break;

				//第二幕
			case 2:
				//走左边遇王翦
				if (branch_id == 1) {
					talkScene_021();

					cout << "\n> ";
					cin >> sceneCommand;
					if (sceneCommand == "1" || sceneCommand == "2" || sceneCommand == "3") {
						int num = stoi(sceneCommand);//将string转化为int
						switch (num) {
						case 1:

							//清屏
							system("cls");
							map_Manager(showScene_id(), branch_id);

							//与王翦对话
							current_character = 1;
							talk_character_contnt_021(current_character);

							cout << tip1;
							key1 = _getch();
							deleteWords(tip1);

							break;

						case 2:

							//清屏
							system("cls");
							map_Manager(showScene_id(), branch_id);

							//与汉军对话
							current_character = 2;
							talk_character_contnt_021(current_character);

							cout << tip1;
							key1 = _getch();
							deleteWords(tip1);

							break;

						case 3:
							//返回
							current_state = SceneState::ORIGIN_SCENE;
							break;
						}
					}
					else {

						if (sceneCommand == "south" || sceneCommand == "w" || sceneCommand == "W"
							|| sceneCommand == "north" || sceneCommand == "n" || sceneCommand == "N"
							|| sceneCommand == "quit" || sceneCommand == "start") {
							choice = false;

						}

						//清屏
						system("cls");

						game1.gameCommand(sceneCommand);
						if (choice) {
							cout << tip1;
							key1 = _getch();
							deleteWords(tip1);
						}
					}
				}
				//走右边进沼泽
				else {
					talkScene_022();

					cout << "\n> ";
					cin >> sceneCommand;
					if (sceneCommand == "1" || sceneCommand == "2") {
						int num = stoi(sceneCommand);//将string转化为int
						switch (num) {
						case 1:

							//清屏
							system("cls");
							map_Manager(showScene_id(), branch_id);

							//与副将对话
							current_character = 1;
							talk_character_contnt_022(current_character);

							cout << tip1;
							key1 = _getch();
							deleteWords(tip1);

							break;

						case 2:
							//返回
							current_state = SceneState::ORIGIN_SCENE;
							break;
						}
					}
					else {

						if (sceneCommand == "south" || sceneCommand == "w" || sceneCommand == "W"
							|| sceneCommand == "north" || sceneCommand == "n" || sceneCommand == "N"
							|| sceneCommand == "quit" || sceneCommand == "start") {
							choice = false;

						}

						//清屏
						system("cls");

						game1.gameCommand(sceneCommand);
						if (choice) {
							cout << tip1;
							key1 = _getch();
							deleteWords(tip1);
						}
					}
				}
				break;

				//第三幕
			case 3:
				talkScene_03();

				cout << "\n> ";
				cin >> sceneCommand;
				if (sceneCommand == "1" || sceneCommand == "2" || sceneCommand == "3"|| sceneCommand == "4"|| sceneCommand == "5") {
					int num = stoi(sceneCommand);//将string转化为int
					switch (num) {
					case 1:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与赤泉侯对话
						current_character = 1;
						talk_character_contnt_03(current_character);

						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);

						break;

					case 2:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与秦时月对话
						current_character = 2;
						talk_character_contnt_03(current_character);

						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);

						break;
					case 3:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与钟离昧对话
						current_character = 3;
						talk_character_contnt_03(current_character);

						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);

						break;
					case 4:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与二十八骑对话
						current_character = 4;
						talk_character_contnt_03(current_character);

						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);

						break;

					case 5:
						//返回
						current_state = SceneState::ORIGIN_SCENE;
						break;
					}
				}
				else {

					if (sceneCommand == "south" || sceneCommand == "w" || sceneCommand == "W"
						|| sceneCommand == "north" || sceneCommand == "n" || sceneCommand == "N"
						|| sceneCommand == "quit" || sceneCommand == "start") {
						choice = false;

					}

					//清屏
					system("cls");

					game1.gameCommand(sceneCommand);
					if (choice) {
						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);
					}
				}
				break;

				//第四幕!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//第四幕对话暂时没有地方插入，第四幕项羽死了不知道如何合理的进行对话
			case 4:
				talkScene_04();

				cout << "\n> ";
				cin >> sceneCommand;
				if (sceneCommand == "1" || sceneCommand == "2" || sceneCommand == "3" || sceneCommand == "4" || sceneCommand == "5") {
					int num = stoi(sceneCommand);//将string转化为int
					switch (num) {
					case 1:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与韩信对话
						current_character = 1;
						talk_character_contnt_04(current_character);

						cout << "输入任意按键继续";
						
						key1 = _getch();

						break;

					case 2:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与汉军对话
						current_character = 2;
						talk_character_contnt_04(current_character);

						cout << "输入任意按键继续";
						
						key1 = _getch();

						break;
					case 3:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与钟离昧对话
						current_character = 3;
						talk_character_contnt_04(current_character);

						cout << "输入任意按键继续";
						
						key1 = _getch();

						break;
					case 4:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与二十八骑对话
						current_character = 4;
						talk_character_contnt_04(current_character);

						cout << "输入任意按键继续";
						
						key1 = _getch();

						break;
					case 5:

						//清屏
						system("cls");
						map_Manager(showScene_id(), branch_id);

						//与乌江亭长对话
						current_character = 5;
						talk_character_contnt_04(current_character);

						cout << "输入任意按键继续";
						
						key1 = _getch();

						break;

					case 6:
						//返回
						current_state = SceneState::ORIGIN_SCENE;
						break;
					}
				}
				else {

					if (sceneCommand == "south" || sceneCommand == "w" || sceneCommand == "W"
						|| sceneCommand == "north" || sceneCommand == "n" || sceneCommand == "N"
						|| sceneCommand == "quit" || sceneCommand == "start") {
						choice = false;

					}

					//清屏
					system("cls");

					game1.gameCommand(sceneCommand);
					if (choice) {
						cout << tip1;
						key1 = _getch();
						deleteWords(tip1);
					}
				}
				break;
			}
			
			break;

			//药房
		case SceneState::PHARMACY:

			//清屏
			system("cls");
			map_Manager(showScene_id(), branch_id);

			void phar();
			cout << "输入任意数字返回。\n";
			cin >> sceneCommand;
			//别忘记加入if/else输入指令
			current_state = SceneState::ORIGIN_SCENE;

			break;

			//锻造
		case SceneState::FORGE:

			//清屏
			system("cls");
			map_Manager(showScene_id(), branch_id);

			void forge();
			cout << "输入任意数字返回。\n";
			cin >> sceneCommand;
			//别忘记加入if/else输入指令
			current_state = SceneState::ORIGIN_SCENE;

			break;
		}
	}
}

//SceneManager::SceneState函数的返回值类型，后面是类的成员函数
//读取当前场景状态
SceneManager::SceneState SceneManager::getSceneState() const
{
	return current_state;
}
//进入新场景状态，便于从主世界进入对话
void SceneManager::setSceneState(SceneState state)
{
	current_state = state;
}
//读取对话人物
int SceneManager::getCurrentCharacter() const
{
	return current_character;
}
//更改对话人物
void SceneManager::setCurrentCharacter(int character_id)
{
	current_character = character_id;
}

void SceneManager::ShowBackground(int scene_id = 0) {
	int key1;
	int branch_id = 0;
	string command = "quit";//控制游戏结束后结束循环退出游戏
	switch (scene_id) {
	case 1:

		//清屏
		system("cls");

		setColor(4);
		cout << "第一幕：四面楚歌，垓下之围" << "\n";
		Sleep(1000);
		setColor(12);
		cout << "\n";
		cout << "公元前202年12月（上旬），垓下（今安徽灵璧东南）" << "\n";
		Sleep(1000);
		setColor(14);
		cout << "项羽和虞姬被困，兵少食尽，人心惶惶。汉军围困重重，夜间楚歌四起。"
			<< "帐中，霸王与虞姬对饮，虞姬舞剑，霸王悲怆，"
			<< "诗曰“力拔山兮气盖世，时不利兮骓不逝，骓不逝兮可奈何，虞兮虞兮奈若何。”" << "\n";
		Sleep(1000);
		//弹出对话框暂时没有制作思路！！！！！！
		cout << "\n小卒a来报" << "\n";
		nextLine();
		setColor(10);
		cout << "小卒a:“大王，我们已经被汉军重重围困，楚歌四起，军内的粮草最多维持三日”" << "\n";
		nextLine();
		setColor(14);
		cout << "\n" << "项羽与虞姬交谈" << "\n";
		nextLine();
		setColor(10);
		cout << "虞姬：“大王，酒温好了”" << "\n";
		nextLine();
		cout << "项羽：“你听见了吗”" << "\n";
		nextLine();
		cout << "虞姬：“听见了”" << "\n";
		nextLine();
		cout << "项羽：“帐外楚歌声声，刘邦这斯，已经把我楚地子民全部收服了吗”" << "\n";
		nextLine();
		cout << "虞姬：“大王，楚地子民永远忠心于你”" << "\n";
		nextLine();
		cout << "虞姬：“大王，你累吗”" << "\n";
		nextLine();
		cout << "项羽：“吾少年起兵，南征北走，未曾一惧，何来累之一说”" << "\n";
		nextLine();
		cout << "项羽：“虞姬，你可有悔”" << "\n";
		nextLine();
		cout <<"虞姬：“妾随大王生死无悔”" << "\n";
		nextLine();
		cout << "\n";
		setColor(14);
		cout << "长夜微凉，锦绣未央。虞姬的身姿映衬在烛火之下，翩翩起舞。"
			<< "宝剑的光泽和着凄冷的月，一切似乎定格在这一刻......" << "\n"<<"\n";
		Sleep(2000);
		cout << "输入任意按键继续";

		key1 = _getch();
		
		//清屏
		system("cls");

		cout << "已存档" << "\n";
		current_scene_id = scene_id;
		cout << "输入w或south继续剧情" << "\n";

		showSceneManager(current_scene_id);
		break;

	case 2:

		//清屏
		system("cls");

		setColor(4);
		cout << "第二幕：突围南逃，淮河之阻" << "\n";
		Sleep(1000);
		setColor(12);
		cout << "\n";
		cout << "公元前202年12月（中旬），自垓下经淮河、阴陵（安徽滁州）至东城" << "\n";
		Sleep(1000);
		setColor(14);
		cout << "霸王率麾下八百壮士，突围南逃，至破晓，渡淮河，余骑百人，汉军觉察。"
			<<"灌婴以五千骑追之。至阴陵，迷失道。遇一田夫。" << "\n";
		Sleep(1000);
		cout << "\n" << "与田夫交谈" << "\n";
		nextLine();
		//弹出对话框暂时没有制作思路！！！！！！
		setColor(10);
		cout << "项羽（勒马，拱手）：“老丈，此处往乌江，该走哪条道”" << "\n";
		nextLine();
		setColor(14);
		cout << "田夫：（不说话）：向左一指。"<<"\n";
		cout << "\n";
		{
			char choice;
			bool choice_test = true;
			do {
				setColor(11);
				cout << "请选择：A.走右边   B.走左边";
				cout << "\n" << ">";
				cin >> choice;
				switch (choice) {
				case 'A':
				case 'a':
					setColor(14);
					cout << "遇到大将王翦" << "\n";
					nextLine();
					setColor(10);
					//弹出对话框暂时没有制作思路！！！！！！
					cout << "王翦：“项将军何处去”" << "\n";
					nextLine();
					cout << "项羽：“兵败而已，你也敢来取笑吾？”" << "\n";
					nextLine();
					setColor(14);
					cout << "王翦默默让手下形成合围之势" << "\n";
					nextLine();
					setColor(10);
					//弹出对话框暂时没有制作思路！！！！！！
					cout << "项羽：“天命而已，吾不信天命，汝可敢来阵前一战”" << "\n";
					nextLine();
					cout << "王翦：“何惧，那便战”" << "\n";
					//-->此处进入两人回合制打斗！！！！！！！！！！！
					setColor(14);
					cout << "项羽突围，向南袭去" << "\n" << "\n";
					choice_test = false;
					branch_id = 1;
					break;

				case 'B':
				case 'b':
					setColor(14);
					cout << "项羽一行军队陷入沼泽，被困半天" << "\n";
					nextLine();
					//弹出对话框暂时没有制作思路！！！！！！
					setColor(10);
					cout << "项羽：“哈哈哈哈哈哈哈，天意如此吗，吾征战一生，杀敌无数，陷阵夺旗，攻城斩将。今日竟为一沮洳所困。”" << "\n";
					nextLine();
					cout << "副将：“将军不必气馁，我们越过此地继续南下，不远处就是乌江亭，待我们折回江东，卷土重来。”" << "\n";
					nextLine();
					setColor(14);
					cout << "项羽沉默" << "\n" << "\n";
					choice_test = false;
					branch_id = 2;
					break;

				default:
					setColor(11);
					cout << "未知分支，请重新选择！！！" << "\n" << "\n";
					setColor(14);
				}
			} while (choice_test);
		}
		Sleep(2000);
		cout << "输入任意按键继续";

		key1 = _getch();

		//清屏
		system("cls");

		cout << "已存档" << "\n";
		current_scene_id = scene_id;
		cout << "输入w或south继续剧情" << "\n";

		showSceneManager(current_scene_id, branch_id);
		break;

	case 3:

		//清屏
		system("cls");

		setColor(4);
		cout << "第三幕：东城快战，以一敌千" << "\n";
		Sleep(1000);
		setColor(12);
		cout << "\n";
		cout << "公元前202年12月（下旬），东城（今安徽定远东南）" << "\n";
		Sleep(1000);
		setColor(14);
		cout << "霸王至东城，余二十八骑，勉诸将曰：“吾起兵至今八岁有余，"
			<<"身经百战，战无不胜，攻无不克，遂霸天下。"
			<<"今天公亡我，固死矣，且看吾溃围，斩将，刈旗。”" << "\n";
		//弹出对话框暂时没有制作思路！！！！！！
		nextLine();
		setColor(10);
		cout << "项羽：“诸君，今日我项羽必败无疑，可惜我江东八千子弟，所向披靡，亡了秦的暴政，今日要折在刘邦这个狗贼手上。”" << "\n";
		nextLine();
		cout << "副将钟离昧：“大王，吾等誓死追随。”" << "\n";
		nextLine();
		cout << "身后众人：“誓死追随，无怨无悔。”" << "\n";
		nextLine();
		setColor(14);
		cout << "项羽（大笑一声）"<<"\n";
		//-->此处进入回合制打斗！！！！！！！！！！！
		setColor(14);
		cout << "项羽将此小将斩于马下，连杀数人，势如破竹" << "\n";
		nextLine();
		setColor(10);
		cout << "副将钟离昧：“将军，赤泉候杨喜率众驰援”" << "\n";
		nextLine();
		cout << "项羽（横枪立马，大喝一声）：“吾不杀无名之人，滚”" << "\n";
		nextLine();
		setColor(14);
		cout << "赤泉候退。" << "\n"<<"\n";
		Sleep(2000);
		cout << "输入任意按键继续";

		key1 = _getch();

		//清屏
		system("cls");

		cout << "已存档" << "\n";
		current_scene_id = scene_id;
		cout << "输入w或south继续剧情" << "\n";

		showSceneManager(current_scene_id);
		break;

	case 4:

		//清屏
		system("cls");

		setColor(4);
		cout << "第四幕：乌江自刎，天地同悲" << "\n";
		Sleep(1000);
		setColor(12);
		cout << "\n";
		cout << "公元前202年12月（月底），乌江（今安徽和县东北乌江浦）" << "\n";
		Sleep(1000);
		setColor(14);
		cout << "霸王欲渡乌江，数合间连斩数敌人，退至江边。"
			<<"乌江亭长檥船待。忆起江东父老，心中怅然，停步江边，仰天长啸。"<< "\n";
		//弹出对话框暂时没有制作思路！！！！！！
		nextLine();
		setColor(10);
		cout << "项羽：“吾今日，真的要葬身于此么”" << "\n";
		nextLine();
		cout << "副将钟离昧：“将军速走，过了乌江就是吾等之乡，何惧不能东山再起”" << "\n";
		nextLine();
		cout << "乌江亭长：“霸王，速走”" << "\n";
		nextLine();
		cout << "项羽：“吾一生未尝一败，今日之势，有死而已，况我江东八千子弟，今日只剩二十八人。吾何惧死，唯独无言面对江东父老。”" << "\n";
		nextLine();
		cout << "虞姬：“大王，你心意已决吗”" << "\n";
		nextLine();
		cout << "项羽：“吾心已死，汝与乌骓，吾之所系。”" << "\n";
		nextLine();
		setColor(14);
		cout << "旁白：虞姬看着项羽，一如当年年少时在江边浣衣初见之时。" << "\n";
		nextLine();
		setColor(10);
		cout << "虞姬：“大王，臣妾为你温酒。待你归家”" << "\n";
		setColor(14);
		cout<<"言罢自刎。" << "\n";
		nextLine();
		setColor(10);
		cout << "项羽（抱着虞姬，长啸一声）：“虞姬已死，吾今日绝不苟活。诸君可愿与吾再杀一场”" << "\n";
		nextLine();
		cout << "众将士：“杀！”" << "\n";
		nextLine();
		//-->此处触发打斗，项羽与对方大将王翦、杨喜、吕胜、杨武轮番打斗。最终战败
		//此处多轮战斗
		setColor(10);
		cout << "项羽：“刘邦，这天下，归你了”" << "\n";
		nextLine();
		setColor(14);
		cout << "（项羽隔着千军万马与韩信相视而立）" << "\n";
		nextLine();
		setColor(10);
		cout << "项羽：“吾去也。”" << "\n";
		setColor(14);
		cout<<"遂自刎而亡。" << "\n";
		nextLine();
		setColor(14);
		cout << "旁白：" << "\n";
		Sleep(1500);
		cout << "公元前202年，一场大雪涤净了天地的色彩。乌江之畔，一人持枪立马，傲立天地之间，鬼神不侵。" << "\n";
		Sleep(1500);
		cout<< "他是项羽，他去见他的虞姬了。一代霸王落幕，退出了楚汉相争的舞台。" << "\n";
		Sleep(1500);
		cout<<"东城荒野，无数的雪花自天边翩翩而落，天地也在为他的死而悲叹，送了他最后一程。" << "\n";
		Sleep(1500);
		cout << "马蹄声淹没了天地，烟尘吞噬了最后的视野，后来，那面残旗被风高高扬起，又被马蹄踩进土里，再也不见。" << "\n";
		Sleep(1500);
		cout << "往事越千年，魏武挥鞭，东临碣石有遗篇，萧瑟秋风今又是，换了人间。" << "\n";
		nextLine();
		cout << "游戏结束，感谢您的游玩" << "\n"
			<< "请输入任意键退出游戏" << "\n";
		
		key1 = _getch();
		game.gameCommand(command);

		break;

	default:
		break;

	}
}
