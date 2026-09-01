#include<iostream>
#include"SceneManager.h"
#include"Game.h"
#include <conio.h> // 控制台无回显按键读取
#include"map.h"
#include "TalkManager.h"
#include "PharManager.h"
#include "ForgeManager.h"
#include "BackGround.h"

SceneManager::SceneManager(Game& game): game(game)
{
	current_scene_id = 0;//剧情初步存档,显示当前场景id
	current_state = SceneState::ORIGIN_SCENE;
	current_character = 0;

	autoPlay = false;//判断是否自动播放剧情
	unique_map_print = false;

	talkManager.setSceneManager(this);
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

void SceneManager::refreshScene(int branch_id) {
	//清屏
	system("cls");
	map_Manager(showScene_id(), branch_id);
}

//判断并执行命令
bool SceneManager::handleCommand(Game& game1, const string& sceneCommand)
{	
	bool ch = true;
	// 判断是否为需要退出当前场景的指令
	if (sceneCommand == "south" || sceneCommand == "w" ||  sceneCommand == "W" ||  
		sceneCommand == "north" || sceneCommand == "n" || sceneCommand == "N" ||
		sceneCommand == "quit" || sceneCommand == "start")
	{	
		// 结束当前 SceneManager
		ch = false;
	}

	// 清屏
	system("cls");
	game1.gameCommand(sceneCommand);
	// 执行完普通指令后，等待玩家按键
	
	if (sceneCommand != "start") {
		cout << "输入任意按键返回";
		_getch();
		deleteWords("输入任意按键返回");
	}
	
	return ch;
}


// 显示当前场景背景
void SceneManager::showCurrentBackground() {
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
}
//场景功能管理,1对话系统+命令系统；2药店系统；3锻造系统
void SceneManager::sceneManager(Game& game1, int branch_id) {
	string sceneCommand;
	scene = true;
	while (scene) {
		switch (current_state) {

		//大世界场景
		case SceneState::ORIGIN_SCENE:

			refreshScene(branch_id);

			showCurrentBackground();
			
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
				scene = handleCommand(game1, sceneCommand);
			}
			break;

		// 选择对话人物
		case SceneState::TALK:
			
			refreshScene(branch_id);

			switch (showScene_id()) {
				//第一幕
			case 1:
				talkManager.talkScene01(game1, branch_id);
				break;

				//第二幕
			case 2:
				talkManager.talkScene02(game1, branch_id);
				break;

				//第三幕
			case 3:
				talkManager.talkScene03(game1, branch_id);
				break;

				//第四幕!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//第四幕对话暂时没有地方插入，第四幕项羽死了不知道如何合理的进行对话
			case 4:
				
				break;
			}
			
			break;

			//药房
		case SceneState::PHARMACY:

			refreshScene(branch_id);

			void phar();
			cout << "输入任意数字返回。\n";
			cin >> sceneCommand;
			//别忘记加入if/else输入指令
			current_state = SceneState::ORIGIN_SCENE;

			break;

			//锻造
		case SceneState::FORGE:

			refreshScene(branch_id);

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
//进入新场景状态，便于从主世界进入对话,便于切换场景
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

//输出剧情对话,逐字打印，Sleep1控制逐字打印速度
void SceneManager::printWords(string tips,int color,int sleep,int sleep1=100) {
	setColor(color);
	for (size_t i = 0; i < tips.length();)
	{
		unsigned char c = tips[i];
		// ASCII 字符
		if (c < 128)
		{
			cout << tips[i];
			i++;
		}
		// UTF-8 中文字符，通常占 3 个字节
		else
		{
			cout << tips.substr(i, 3);
			i += 3;
		}
		cout << flush;
		Sleep(sleep1);
	}

	cout << "\n";
	Sleep(sleep);
}
void SceneManager::ShowBackground(int scene_id = 0) {
	int key1;
	int branch_id = 0;
	string command = "quit";//控制游戏结束后结束循环退出游戏
	switch (scene_id) {
	case 1:

		//清屏
		system("cls");
		printWords("第一幕：四面楚歌，垓下之围", 4, 1500);
		cout << "\n";
		printWords("公元前202年12月（上旬），垓下（今安徽灵璧东南）",12,1000);
		printWords("项羽和虞姬被困，兵少食尽，人心惶惶。汉军围困重重，夜间楚歌四起。帐中，霸王与虞姬对饮，虞姬舞剑，霸王悲怆，诗曰“力拔山兮气盖世，时不利兮骓不逝，骓不逝兮可奈何，虞兮虞兮奈若何。”", 14, 1000);
		//弹出对话框暂时没有制作思路！！！！！！
		printWords("\n小卒a来报", 14, 0);
		nextLine();
		printWords("小卒a:“大王，我们已经被汉军重重围困，楚歌四起，军内的粮草最多维持三日”", 10, 0);
		nextLine();
		printWords("\n项羽与虞姬交谈", 14, 0);
		nextLine();
		printWords("虞姬：“大王，酒温好了”", 10, 0);
		nextLine();
		printWords("项羽：“你听见了吗”", 10, 0);
		nextLine();
		printWords("虞姬：“听见了”", 10, 0);
		nextLine();
		printWords("项羽：“帐外楚歌声声，刘邦这斯，已经把我楚地子民全部收服了吗”", 10, 0);
		nextLine();
		printWords("虞姬：“大王，楚地子民永远忠心于你”", 10, 0);
		nextLine();
		printWords("虞姬：“大王，你累吗”", 10, 0);
		nextLine();
		printWords("项羽：“吾少年起兵，南征北走，未曾一惧，何来累之一说”", 10, 0);
		nextLine();
		printWords("项羽：“虞姬，你可有悔”",10,0);
		nextLine();
		printWords("虞姬：“妾随大王生死无悔”", 10, 0);
		nextLine();
		printWords("\n长夜微凉，锦绣未央。虞姬的身姿映衬在烛火之下，翩翩起舞。宝剑的光泽和着凄冷的月，一切似乎定格在这一刻......\n", 14, 2000);
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
		printWords("第二幕：突围南逃，淮河之阻",4,1500);
		printWords("\n公元前202年12月（中旬），自垓下经淮河、阴陵（安徽滁州）至东城", 12, 1000);
		printWords("霸王率麾下八百壮士，突围南逃，至破晓，渡淮河，余骑百人，汉军觉察。灌婴以五千骑追之。至阴陵，迷失道。遇一田夫。", 14, 1000);
		printWords("\n与田夫交谈", 14, 0);
		nextLine();
		//弹出对话框暂时没有制作思路！！！！！！
		printWords("项羽（勒马，拱手）：“老丈，此处往乌江，该走哪条道”", 10, 0);
		nextLine();
		printWords("田夫：（不说话）：向左一指。\n", 14, 0);
		{
			char choice;
			bool choice_test = true;
			do {
				printWords("请选择：A.走右边   B.走左边", 11, 0);
				cout << "\n" << ">";
				cin >> choice;
				switch (choice) {
				case 'A':
				case 'a':
					printWords("遇到大将王翦", 14, 0);
					nextLine();
					printWords("王翦：“项将军何处去”", 10, 0);
					//弹出对话框暂时没有制作思路！！！！！！
					nextLine();
					printWords("项羽：“兵败而已，你也敢来取笑吾？”", 10, 0);
					nextLine();
					printWords("王翦默默让手下形成合围之势", 14, 0);
					nextLine();
					setColor(10);
					//弹出对话框暂时没有制作思路！！！！！！
					printWords("项羽：“天命而已，吾不信天命，汝可敢来阵前一战”", 10, 0);
					nextLine();
					printWords("王翦：“何惧，那便战”", 10, 0);
					//-->此处进入两人回合制打斗！！！！！！！！！！！
					printWords("项羽突围，向南袭去\n", 14, 0);
					choice_test = false;
					branch_id = 1;
					break;

				case 'B':
				case 'b':
					printWords("项羽一行军队陷入沼泽，被困半天", 14, 0);
					nextLine();
					//弹出对话框暂时没有制作思路！！！！！！
					printWords("项羽：“哈哈哈哈哈哈哈，天意如此吗，吾征战一生，杀敌无数，陷阵夺旗，攻城斩将。今日竟为一沮洳所困。”", 10, 0);
					nextLine();
					printWords("副将：“将军不必气馁，我们越过此地继续南下，不远处就是乌江亭，待我们折回江东，卷土重来。”", 10, 0);
					nextLine();
					printWords("项羽沉默\n", 14, 0);
					choice_test = false;
					branch_id = 2;
					break;

				default:
					printWords("未知分支，请重新选择！！！\n", 11, 0);
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
		printWords("第三幕：东城快战，以一敌千", 4, 1500);
		printWords("\n公元前202年12月（下旬），东城（今安徽定远东南）", 12, 1000);
		printWords("霸王至东城，余二十八骑，勉诸将曰：“吾起兵至今八岁有余，身经百战，战无不胜，攻无不克，遂霸天下。今天公亡我，固死矣，且看吾溃围，斩将，刈旗。”", 14, 0);
		//弹出对话框暂时没有制作思路！！！！！！
		nextLine();
		printWords("项羽：“诸君，今日我项羽必败无疑，可惜我江东八千子弟，所向披靡，亡了秦的暴政，今日要折在刘邦这个狗贼手上。”", 10, 0);
		nextLine();
		printWords("副将钟离昧：“大王，吾等誓死追随。”",10,0);
		nextLine();
		printWords("身后众人：“誓死追随，无怨无悔。”", 10, 0);
		nextLine();
		printWords("项羽（大笑一声）", 14, 0);
		//-->此处进入回合制打斗！！！！！！！！！！！
		printWords("项羽将此小将斩于马下，连杀数人，势如破竹", 14, 0);
		nextLine();
		printWords("副将钟离昧：“将军，赤泉候杨喜率众驰援”", 10, 0);
		nextLine();
		printWords("项羽（横枪立马，大喝一声）：“吾不杀无名之人，滚”", 10, 0);
		nextLine();
		printWords("赤泉候退。\n", 14, 2000);
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
		printWords("第四幕：乌江自刎，天地同悲", 4, 1500);
		printWords("\n公元前202年12月（月底），乌江（今安徽和县东北乌江浦）", 12, 1000);
		printWords("霸王欲渡乌江，数合间连斩数敌人，退至江边。乌江亭长檥船待。忆起江东父老，心中怅然，停步江边，仰天长啸。", 14, 0);
		//弹出对话框暂时没有制作思路！！！！！！
		nextLine();
		printWords("项羽：“吾今日，真的要葬身于此么”", 10, 0);
		nextLine();
		printWords("副将钟离昧：“将军速走，过了乌江就是吾等之乡，何惧不能东山再起”", 10, 0);
		nextLine();
		printWords("乌江亭长：“霸王，速走”", 10, 0);
		nextLine();
		printWords("项羽：“吾一生未尝一败，今日之势，有死而已，况我江东八千子弟，今日只剩二十八人。吾何惧死，唯独无言面对江东父老。”", 10, 0);
		nextLine();
		printWords("虞姬：“大王，你心意已决吗”", 10, 0);
		nextLine();
		printWords("项羽：“吾心已死，汝与乌骓，吾之所系。”", 10, 0);
		nextLine();
		printWords("旁白：虞姬看着项羽，一如当年年少时在江边浣衣初见之时。", 14, 0);
		nextLine();
		printWords("虞姬：“大王，臣妾为你温酒。待你归家”", 10, 0);
		nextLine();
		printWords("言罢自刎。", 14, 0);
		nextLine();
		printWords("项羽（抱着虞姬，长啸一声）：“虞姬已死，吾今日绝不苟活。诸君可愿与吾再杀一场”", 10, 0);
		nextLine();
		printWords("众将士：“杀！”", 10, 0);
		nextLine();
		//-->此处触发打斗，项羽与对方大将王翦、杨喜、吕胜、杨武轮番打斗。最终战败
		//此处多轮战斗
		printWords("项羽：“刘邦，这天下，归你了”", 10, 0);
		nextLine();
		printWords("（项羽隔着千军万马与韩信相视而立）", 14, 0);
		nextLine();
		printWords("项羽：“吾去也。”", 10, 0);
		nextLine();
		printWords("遂自刎而亡。", 14, 0);
		nextLine();
		printWords("旁白：", 14,1500);
		printWords("公元前202年，一场大雪涤净了天地的色彩。乌江之畔，一人持枪立马，傲立天地之间，鬼神不侵。", 14, 1500);
		printWords("他是项羽，他去见他的虞姬了。一代霸王落幕，退出了楚汉相争的舞台。", 14, 1500);
		printWords("东城荒野，无数的雪花自天边翩翩而落，天地也在为他的死而悲叹，送了他最后一程。", 14, 1500);
		printWords("马蹄声淹没了天地，烟尘吞噬了最后的视野，后来，那面残旗被风高高扬起，又被马蹄踩进土里，再也不见。", 14, 1500);
		printWords("往事越千年，魏武挥鞭，东临碣石有遗篇，萧瑟秋风今又是，换了人间。", 14, 0);
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
