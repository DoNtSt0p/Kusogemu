#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <Windows.h>
#include <iostream>

// // // // // // // // // // // // // // // // // // // // // // // // // // //
struct map_packet { int size; char arr[100][100]; };
sf::Packet& operator<<(sf::Packet &pack, const map_packet &a) {
	pack << a.size;
	for (int i = 0; i < 100; i++) pack << a.arr[i];
	return pack; }
sf::Packet& operator>>(sf::Packet &pack, map_packet &a) {
	pack >> a.size;
	for (int i = 0; i < 100; i++) pack >> a.arr[i];
	return pack; }
struct full_tank_packet {
	int m0, m1, m2, m3, gun_type;
	int a0, a1, a2, a3, s0, s1, s2, s3, s4, s5, s6, s7,
		team, health, speed, rotation_speed, repair_speed,
		engine_power, gun_reload, gun_rotate_speed, gun_spread; };
sf::Packet& operator<<(sf::Packet &pack, const full_tank_packet &a) {
	return pack << a.m0 << a.m1 << a.m2 << a.m3 << a.gun_type << a.a0 << a.a1
	<< a.a2 << a.a3 << a.s0 << a.s1 << a.s2 << a.s3 << a.s4 << a.s5 <<a.s6<<a.s7
	<< a.team << a.health << a.speed << a.rotation_speed << a.repair_speed
	<< a.engine_power << a.gun_reload << a.gun_spread; }
sf::Packet& operator>>(sf::Packet &pack, full_tank_packet &a) {
	return pack >> a.m0 >> a.m1 >> a.m2 >> a.m3 >> a.gun_type >> a.a0 >> a.a1
	>> a.a2 >> a.a3 >> a.s0 >> a.s1 >> a.s2 >> a.s3 >> a.s4 >> a.s5 >>a.s6>>a.s7
	>> a.team >> a.health >> a.speed >> a.rotation_speed >> a.repair_speed
	>> a.engine_power >> a.gun_reload >> a.gun_spread; }
struct control { bool left_forward, right_forward, left_back, right_back, shoot;
	int mouse_x, mouse_y; };
sf::Packet& operator<<(sf::Packet& pack, const control& a) {
	return pack << a.left_forward << a.right_forward << a.left_back
	<< a.right_back << a.mouse_x << a.mouse_y; }
sf::Packet& operator>>(sf::Packet& pack, control& a) {
	return pack >> a.left_forward >> a.right_forward >> a.left_back
	>> a.right_back >> a.mouse_x >> a.mouse_y; }
// // // // // // // // // // // // // // // // // // // // // // // // // // //

class map {
public:
	int size;
	char **arr;
	map(int sz) : size(sz) {
		arr = new char* [size];
		for (int i = 0; i < size; i++) {
			arr[i] = new char[size];
			for (int j = 0; j < size; j++)
				arr[i][j] = '0';
		}
	}
	map(map_packet p) : size(p.size) {
		arr = new char* [size];
		for (int i = 0; i < size; i++) {
			arr[i] = new char[size];
			for (int j = 0; j < size; j++)
				arr[i][j] = p.arr[i][j];
		}
	}
	~map() {
		for (int i = 0; i < size; i++)
			delete [] arr[i];
	}
};

class tank {
public:
	sf::Vector2f server_pos;
	float server_rot, server_rot_turret;
	sf::RectangleShape shape_body, shape_tread_l, shape_tread_r, shape_barrel;
	sf::CircleShape shape_turret;
	int
		m0, m1, m2, m3,
		gun_type;
	int
		a0, a1, a2, a3,
		s0, s1, s2, s3,
		s4, s5, s6, s7,
		team,
		health,
		speed, rotation_speed, repair_speed,
		engine_power,
		gun_reload, gun_rotate_speed, gun_spread;
	int server_reload;
	control server_move;
	sf::Vector2f server_speed;
	tank() :
		server_pos(sf::Vector2f(0, 0)),
		server_rot(0.f), server_rot_turret(0.f),
		m0(0), m1(0), m2(0), m3(0), gun_type(0),
		a0(10), a1(10), a2(10), a3(10),
		s0(10), s1(10), s2(10), s3(10),
		s4(10), s5(10), s6(10), s7(0),
		team(0), health(100), speed(10), rotation_speed(10), repair_speed(10),
		engine_power(10), gun_reload(1), gun_rotate_speed(10), gun_spread(10),
		server_reload(0), server_speed(sf::Vector2f(0, 0))
	{
		server_move.left_forward = false; server_move.right_forward = false;
		server_move.left_back = false; server_move.right_back = false;
		server_move.shoot = false;
	}
	tank(full_tank_packet p) :
		server_pos(sf::Vector2f(0, 0)),
		server_rot(0.f), server_rot_turret(0.f),
		m0(p.m0), m1(p.m1), m2(p.m2), m3(p.m3), gun_type(p.gun_type),
		a0(p.a0), a1(p.a1), a2(p.a2), a3(p.a3),
		s0(p.s0), s1(p.s1), s2(p.s2), s3(p.s3),
		s4(p.s4), s5(p.s5), s6(p.s6), s7(p.s7),
		team(p.team), health(p.health), speed(p.speed),
		rotation_speed(p.rotation_speed), repair_speed(p.repair_speed),
		engine_power(p.engine_power), gun_reload(p.gun_reload),
		gun_rotate_speed(p.gun_rotate_speed), gun_spread(p.gun_spread),
		server_reload(0), server_speed(sf::Vector2f(0, 0))
	{
		server_move.left_forward = false; server_move.right_forward = false;
		server_move.left_back = false; server_move.right_back = false;
		server_move.shoot = false;
	}
};

int main() {
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	sf::UdpSocket socket;
	int port = 5185;
	sf::Packet packet;
	while (socket.bind(5185) != sf::Socket::Done) {
		std::cout << "failed connect port " << port << "\n";
		port++;
	}
	std::cout << "Connected   ip: " << ip << "   port: " << port << "\n";

	sf::IpAddress  admin_ip,   users_ip[10],    sender_ip;
	unsigned short admin_port, users_ports[10], sender_port;
	int users_count = 0;

	int data;

	while (data != 12345) {
		socket.receive(packet, admin_ip, admin_port);
		packet >> data;
		std::cout << data << "\n";
		packet.clear();
	}
	std::cout << "Admin   ip: " << admin_ip << "   port: " << admin_port <<"\n";
	users_ip[0] = admin_ip;
	users_ports[0] = admin_port;
	users_count = 1;

	packet.clear();
	packet << 5185;
	socket.send(packet, admin_ip, admin_port);

	tank players[10];
	int players_id[10];
	int players_count = 0;

	sf::SocketSelector selector;
	selector.add(socket);
	socket.setBlocking(false);
	while (true) {
		selector.wait();
		packet.clear();
		socket.receive(packet, sender_ip, sender_port);
		packet >> data;
		
		if (data == 12345) {
			users_ip[users_count] = sender_ip;
			users_ports[users_count] = sender_port;
			users_count++;
			packet.clear();
			packet << 54321;
			socket.send(packet, sender_ip, sender_port);
		}

		else if (data == 5) {
			int id;
			packet >> id;
			players_id[players_count] = id;
			packet.clear();
			packet << 5;
			socket.send(packet, users_ip[id], users_ports[id]);
			socket.clear();
			selector.wait(sf::seconds(1));
			full_tank_packet inpt;
			socket >> inpt;
			players[id] = tank(inpt);
		}
	}

	system("pause");
	return 0;
}