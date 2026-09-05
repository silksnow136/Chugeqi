#include <iostream>
#include "map.h"
#include "Game.h"
#include"SceneManager.h"
#include "core/console.h"
void map(){
	cout << "\n";
	//旧版地图
	/*
	cout <<"   垓   下   " << "\n"
		<< "      |" << "\n"
		<< "   淮   河"<<"\n"
		<< "      |"<<"\n"
		<< "   东   城" << "\n"
		<< "      |" << "\n"
		<< "   乌   江"<<"\n";
	*/
	console::setColor(2);
	cout << "森森森森森森森森森森森森森森森森森森森森森森森森森森森森森森" << "\n";
	cout << "森森森森森森森森森森森森森森森森森森森森森森森森森森森森森森" << "\n";
	cout << "森森森森森                                        森森森森森" << "\n";
	cout << "森森森森森     ";
	console::setColor(4);
	cout << "垓   下";
	console::setColor(2);
	cout<<"                            森森森森森" << "\n";

	cout << "森森森森森        ";
	console::setColor(4);
	cout << "|";
	console::setColor(2);
	cout<<"                               森森森森森" << "\n";
	console::setColor(3);
	cout << "水水水水水水水水  ";
	console::setColor(4);
	cout << "|";
	console::setColor(3);
	cout<<" 水水水水水水水水水水水水水水水水水水水水" << "\n";
	cout << "水水水水水     ";
	console::setColor(4);
	cout << "淮   河";
	console::setColor(3);
	cout<<"                            水水水水水" << "\n";
	cout << "水水水水水水水水  ";
	console::setColor(4);
	cout << "|";
	console::setColor(3);
	cout<<" 水水水水水水水水水水水水水水水水水水水水" << "\n";
	console::setColor(2);
	cout << "森森森森森        ";
	console::setColor(4);
	cout << "|";
	console::setColor(2);
	cout<<"                               森森森森森" << "\n";
	cout << "森森森森森     ";
	console::setColor(4);
	cout << "东   城";
	console::setColor(2);
	cout<<"                            森森森森森" << "\n";
	cout << "森森森森森        ";
	console::setColor(4);
	cout << "|";
	console::setColor(2);
	cout<<"                               森森森森森" << "\n";
	console::setColor(3);
	cout << "水水水水水水水水  ";
	console::setColor(4);
	cout << "|";
	console::setColor(3);
	cout<<" 水水水水水水水水水水水水水水水水水水水水" << "\n";
	cout << "水水水水水     ";
	console::setColor(4);
	cout << "乌   江";
	console::setColor(3);
	cout<<"                            水水水水水" << "\n";
	cout << "水水水水水                                        水水水水水" << "\n";
	cout << "水水水水水水水水水水水水水水水水水水水水水水水水水水水水水水" << "\n";
	cout << "水水水水水水水水水水水水水水水水水水水水水水水水水水水水水水" << "\n";
	console::setColor(14);
}
//场景显示
void map_Manager(int scene_id, int branch_id) {
	switch(scene_id){
	case 1:
		cout << "垓下-军营"<<"\n";
		cout << "-------------------------------------------------" << "\n"
			<< "|                                               |" << "\n"
			<< "|             项羽            虞姬              |" << "\n"
			<< "|                                               |" << "\n"
			<< "|                    小卒a                      |" << "\n"
			<< "|                                               |" << "\n"
			<< "|                                               |" << "\n"
			<< "----------------------门门------------------------" << "\n"<<"\n";
		break;

	case 2:
		if (branch_id == 1) {
			cout << "左边" << "\n";
			cout << "-------------------------------------------------" << "\n"
				<< "|                                               |" << "\n"
				<< "|                     项羽                      |" << "\n"
				<< "|                                               |" << "\n"
				<< "|         王翦                    汉军          |" << "\n"
				<< "|                                               |" << "\n"
				<< "|                                               |" << "\n"
				<< "--------------------------------------------------" << "\n" << "\n";
		}
		else {
			cout << "右边-沼泽" << "\n";
			cout << "-------------------------------------------------" << "\n"
				<< "|                                               |" << "\n"
				<< "|          项羽                副将             |" << "\n"
				<< "|                                               |" << "\n"
				<< "|                                               |" << "\n"
				<< "|                                               |" << "\n"
				<< "|                                               |" << "\n"
				<< "--------------------------------------------------" << "\n" << "\n";
		}
		break;

	case 3:
		cout << "东城" << "\n";
		cout << "-------------------------------------------------" << "\n"
			<< "|                                               |" << "\n"
			<< "|           赤泉侯          秦时月              |" << "\n"
			<< "|                                               |" << "\n"
			<< "|        钟离昧       项羽     二十八骑         |" << "\n"
			<< "|                                               |" << "\n"
			<< "|                                               |" << "\n"
			<< "--------------------------------------------------" << "\n" << "\n";
		break;

	case 4:
		cout << "乌江" << "\n";
		cout << "-------------------------------------------------" << "\n"
			<< "|                                               |" << "\n"
			<< "|             韩信          汉军                |" << "\n"
			<< "|                                               |" << "\n"
			<< "|        钟离昧      项羽      二十八骑         |" << "\n"
			<< "|                                               |" << "\n"
			<< "|                  乌江亭长                     |" << "\n"
			<< "--------------------------------------------------" << "\n" << "\n";
		break;

	default:
		break;
	}
}
