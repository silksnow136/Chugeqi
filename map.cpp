#include <iostream>
#include "map.h"
#include "Game.h"
#include"SceneManager.h"
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
	setColor(2);
	cout << "森森森森森森森森森森森森森森森森森森森森森森森森森森森森森森" << "\n";
	cout << "森森森森森森森森森森森森森森森森森森森森森森森森森森森森森森" << "\n";
	cout << "森森森森森                                        森森森森森" << "\n";
	cout << "森森森森森     ";
	setColor(4);
	cout << "垓   下";
	setColor(2);
	cout<<"                            森森森森森" << "\n";

	cout << "森森森森森        ";
	setColor(4);
	cout << "|";
	setColor(2);
	cout<<"                               森森森森森" << "\n";
	setColor(3);
	cout << "水水水水水水水水  ";
	setColor(4);
	cout << "|";
	setColor(3);
	cout<<" 水水水水水水水水水水水水水水水水水水水水" << "\n";
	cout << "水水水水水     ";
	setColor(4);
	cout << "淮   河";
	setColor(3);
	cout<<"                            水水水水水" << "\n";
	cout << "水水水水水水水水  ";
	setColor(4);
	cout << "|";
	setColor(3);
	cout<<" 水水水水水水水水水水水水水水水水水水水水" << "\n";
	setColor(2);
	cout << "森森森森森        ";
	setColor(4);
	cout << "|";
	setColor(2);
	cout<<"                               森森森森森" << "\n";
	cout << "森森森森森     ";
	setColor(4);
	cout << "东   城";
	setColor(2);
	cout<<"                            森森森森森" << "\n";
	cout << "森森森森森        ";
	setColor(4);
	cout << "|";
	setColor(2);
	cout<<"                               森森森森森" << "\n";
	setColor(3);
	cout << "水水水水水水水水  ";
	setColor(4);
	cout << "|";
	setColor(3);
	cout<<" 水水水水水水水水水水水水水水水水水水水水" << "\n";
	cout << "水水水水水     ";
	setColor(4);
	cout << "乌   江";
	setColor(3);
	cout<<"                            水水水水水" << "\n";
	cout << "水水水水水                                        水水水水水" << "\n";
	cout << "水水水水水水水水水水水水水水水水水水水水水水水水水水水水水水" << "\n";
	cout << "水水水水水水水水水水水水水水水水水水水水水水水水水水水水水水" << "\n";
	setColor(14);
}
//场景显示
void map_Manager(int scene_id) {
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
		break;

	case 3:
		break;

	case 4:
		break;

	default:
		break;
	}
}
