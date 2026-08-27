#include<iostream>
#include"SceneManager.h"
#include"Game.h"
#include <conio.h> // 控制台无回显按键读取

bool autoPlay = false;//判断是否自动播放剧情

bool current_Auto() {
	return autoPlay;
}

void chageAuto() {
	autoPlay = !autoPlay;
}

void nextLine() {
	if (autoPlay) {
		Sleep(1500);//1500ms后播放下一条剧情
	}
	else {
		string tip = "  【按任意键继续对话】";
		cout << tip;
		_getch(); // 读取键盘按键，但不是显示键盘输入

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
	
}

void ShowBackground(int scene_id = 0) {
	switch (scene_id) {
	case 1:
		setColor(4);
		cout << "第一幕：四面楚歌，垓下之围" << "\n";
		Sleep(1000);
		setColor(12);
		cout << "\n";
		cout << "公元前202年12月（上旬），垓下（今安徽灵璧东南）" << "\n";
		setColor(14);
		cout << "项羽和虞姬被困，兵少食尽，人心惶惶。汉军围困重重，夜间楚歌四起。"
			<< "帐中，霸王与虞姬对饮，虞姬舞剑，霸王悲怆，"
			<< "诗曰“力拔山兮气盖世，时不利兮骓不逝，骓不逝兮可奈何，虞兮虞兮奈若何。”" << "\n";
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
		cout << "输入w继续剧情" << "\n";
		break;

	case 2:
		setColor(4);
		cout << "第二幕：突围南逃，淮河之阻" << "\n";
		Sleep(1000);
		setColor(12);
		cout << "\n";
		cout << "公元前202年12月（中旬），自垓下经淮河、阴陵（安徽滁州）至东城" << "\n";
		setColor(14);
		cout << "霸王率麾下八百壮士，突围南逃，至破晓，渡淮河，余骑百人，汉军觉察。"
			<<"灌婴以五千骑追之。至阴陵，迷失道。遇一田夫。" << "\n";
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
					break;

				default:
					setColor(11);
					cout << "未知分支，请重新选择！！！" << "\n" << "\n";
					setColor(14);
				}
			} while (choice_test);
		}
		cout << "输入w继续剧情" << "\n";
		break;

	case 3:
		setColor(4);
		cout << "第三幕：东城快战，以一敌千" << "\n";
		Sleep(1000);
		setColor(12);
		cout << "\n";
		cout << "公元前202年12月（下旬），东城（今安徽定远东南）" << "\n";
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
		cout << "输入w继续剧情" << "\n";
		break;

	case 4:
		setColor(4);
		cout << "第四幕：乌江自刎，天地同悲" << "\n";
		Sleep(1000);
		setColor(12);
		cout << "\n";
		cout << "公元前202年12月（月底），乌江（今安徽和县东北乌江浦）" << "\n";
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
			<< "请输入quit退出游戏" << "\n";
		break;

	default:
		break;

	}
}