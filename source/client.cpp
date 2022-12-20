#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <Windows.h>

using namespace sf;

// // // // // // // // // // // // // // // // // // // // // // // // // // //
struct map_packet { int size; char arr[100][100]; };
Packet& operator<<(Packet &pack, const map_packet &a) {
	pack << a.size;
	for (int i = 0; i < 100; i++) pack << a.arr[i];
	return pack; }
Packet& operator>>(Packet &pack, map_packet &a) {
	pack >> a.size;
	for (int i = 0; i < 100; i++) pack >> a.arr[i];
	return pack; }
struct full_tank_packet {
	int m0, m1, m2, m3, gun_type;
	int a0, a1, a2, a3, s0, s1, s2, s3, s4, s5, s6, s7,
		team, health, speed, rotation_speed, repair_speed,
		engine_power, gun_reload, gun_rotate_speed, gun_spread; };
Packet& operator<<(Packet &pack, const full_tank_packet &a) {
	return pack << a.m0 << a.m1 << a.m2 << a.m3 << a.gun_type << a.a0 << a.a1
	<< a.a2 << a.a3 << a.s0 << a.s1 << a.s2 << a.s3 << a.s4 << a.s5 <<a.s6<<a.s7
	<< a.team << a.health << a.speed << a.rotation_speed << a.repair_speed
	<< a.engine_power << a.gun_reload << a.gun_spread; }
Packet& operator>>(Packet &pack, full_tank_packet &a) {
	return pack >> a.m0 >> a.m1 >> a.m2 >> a.m3 >> a.gun_type >> a.a0 >> a.a1
	>> a.a2 >> a.a3 >> a.s0 >> a.s1 >> a.s2 >> a.s3 >> a.s4 >> a.s5 >>a.s6>>a.s7
	>> a.team >> a.health >> a.speed >> a.rotation_speed >> a.repair_speed
	>> a.engine_power >> a.gun_reload >> a.gun_spread; }
struct control { bool left_forward, right_forward, left_back, right_back, shoot;
	int mouse_x, mouse_y; };
Packet& operator<<(Packet& pack, const control& a) {
	return pack << a.left_forward << a.right_forward << a.left_back
	<< a.right_back << a.mouse_x << a.mouse_y; }
Packet& operator>>(Packet& pack, control& a) {
	return pack >> a.left_forward >> a.right_forward >> a.left_back
	>> a.right_back >> a.mouse_x >> a.mouse_y; }
// // // // // // // // // // // // // // // // // // // // // // // // // // //

float _size = 1.f;

class map {
public:
	int size;
	char arr[100][100];
	map(int sz) : size(sz) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++)
				arr[i][j] = '0';
		}
	}
	map(map_packet p) : size(p.size) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++)
				arr[i][j] = p.arr[i][j];
		}
	}
	~map() {
		for (int i = 0; i < 100; i++)
			delete [] arr[i];
	}
};

class tank {
public:
	Vector2f _pos;
	float _rot, _rot_turret;
	RectangleShape shape_body, shape_tread_l, shape_tread_r, shape_barrel;
	CircleShape shape_turret;
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
	
	tank() :
		_pos(Vector2f(0, 0)),
		_rot(0.f), _rot_turret(0.f),
		m0(0), m1(0), m2(0), m3(0), gun_type(0),
		a0(10), a1(10), a2(10), a3(10),
		s0(10), s1(10), s2(10), s3(10),
		s4(10), s5(10), s6(10), s7(0),
		team(0), health(100), speed(10), rotation_speed(10), repair_speed(10),
		engine_power(10), gun_reload(1), gun_rotate_speed(10), gun_spread(10)
	{
		update_form();
	}
	tank(full_tank_packet p) :
		_pos(Vector2f(0, 0)),
		_rot(0.f), _rot_turret(0.f),
		m0(p.m0), m1(p.m1), m2(p.m2), m3(p.m3), gun_type(p.gun_type),
		a0(p.a0), a1(p.a1), a2(p.a2), a3(p.a3),
		s0(p.s0), s1(p.s1), s2(p.s2), s3(p.s3),
		s4(p.s4), s5(p.s5), s6(p.s6), s7(p.s7),
		team(p.team), health(p.health), speed(p.speed),
		rotation_speed(p.rotation_speed), repair_speed(p.repair_speed),
		engine_power(p.engine_power), gun_reload(p.gun_reload),
		gun_rotate_speed(p.gun_rotate_speed), gun_spread(p.gun_spread)
	{
		update_form();
	}
	void update_pos() {
		shape_body   .setPosition(_pos); shape_body   .setRotation(_rot);
		shape_tread_l.setPosition(_pos); shape_tread_r.setPosition(_pos);
		shape_turret .setPosition(_pos); shape_barrel .setPosition(_pos);
		shape_tread_l.setRotation(_rot); shape_tread_r.setRotation(_rot);
		shape_turret .setRotation(_rot + _rot_turret);
		shape_barrel .setRotation(_rot + _rot_turret);
	}
	void update_form() {
		shape_body   .setSize  (Vector2f(8 * s1 * _size, 14 * s0 * _size));
		shape_body   .setOrigin(Vector2f(shape_body.getSize().x / 2.f,
			shape_body.getSize().y / 2.f));
		shape_tread_l.setSize  (Vector2f(2 * s2 * _size, 15 * s0 * _size));
		shape_tread_l.setOrigin(Vector2f(shape_tread_l.getSize().x
			+ shape_body.getSize().x / 2.f, shape_tread_l.getSize().y / 2.f));
		shape_tread_r.setSize  (Vector2f(2 * s2 * _size, 15 * s0 * _size));
		shape_tread_r.setOrigin(Vector2f( - shape_body.getSize().x / 2.f,
			shape_tread_r.getSize().y / 2.f));
		shape_turret .setRadius(s4 * 6 / 2.f);
		shape_turret .setOrigin(Vector2f(shape_turret.getRadius(),
			shape_turret.getRadius() + s7 * _size));
		shape_barrel .setSize  (Vector2f(s6 * 0.75f * _size, s5 * 5 * _size));
		shape_barrel .setOrigin(Vector2f(shape_barrel.getSize().x / 2.f,
			shape_barrel.getSize().y + shape_turret.getRadius()
			- shape_barrel.getSize().x));

		shape_tread_l.setFillColor(Color(223,223,223));
		shape_tread_r.setFillColor(Color(223,223,223));
		shape_turret.setFillColor(Color(191,191,191));
		shape_barrel.setFillColor(Color(207,207,207));
		update_pos();
	}
	void draw(RenderWindow* W) {
		W -> draw(shape_body);
		W -> draw(shape_tread_l);
		W -> draw(shape_tread_r);
		W -> draw(shape_barrel);
		W -> draw(shape_turret);
	}
};

int server_port = 5185;
IpAddress server_ip;
void request(UdpSocket* s) {
	Packet req;
	req << 12345;
	s->send(req, server_ip, server_port);
	SocketSelector sel;
	sel.add(*s);
	if (sel.wait(seconds(5))) {
		IpAddress sip;
		unsigned short spt;
		int data;
		s->receive(req, sip, spt);
		req >> data;
		if (data == 5185 || data == 54321) {
			// ?
		}
	}
}

int main() {
	::ShowWindow(::GetConsoleWindow(), SW_MINIMIZE);

	IpAddress ip = IpAddress::getLocalAddress();
	server_ip = ip;
	UdpSocket socket;
	int port;
	for (port = 5000; port < 5185; port++) {
		if (socket.bind(port) == Socket::Done) {
			break;
		}
	}
	SocketSelector selector;

	tank t;
	t._pos = Vector2f(250, 250);
	t.update_form();
	RenderWindow w;
	w.create(VideoMode(500, 500), "test");
	w.setFramerateLimit(60);
	w.requestFocus();
	while (w.isOpen()) {
		w.clear(Color(10,10,10));
		t.draw(&w);
		t._rot += 0.5f;
		t._rot_turret += 0.5f;
		t.update_pos();
		w.display();
		Event e;
		while (w.pollEvent(e)) {
			if (e.type == Event::Closed)
				w.close();
			if (e.type == Event::KeyPressed) {
				if (e.key.code == Keyboard::Key::Tab)
					request(&socket);
			}
		}
	}
	return 0;
}