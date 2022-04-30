#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

using namespace std;

// To debug: cerr << "Debug messages..." << endl;

typedef struct s_entity
{
	int id; // Unique identifier
	int type; // 0=monster, 1=your hero, 2=opponent hero
	int x; // Position of this entity
	int y;
	int shield_life; // Ignore for this league; Count down until shield spell fades
	int is_controlled; // Ignore for this league; Equals 1 when this entity is under a control spell
	int health; // Remaining health of this monster
	int vx; // Trajectory of this monster
	int vy;
	int near_base; // 0=monster with no target yet, 1=monster targeting a base
	int threat_for; // Given this monster's trajectory, is it a threat to 1=your base, 2=your opponent's base, 0=neither
	int	dist; //distance to base for monster;
	string spell;
}	t_entity;

bool sortthreat(t_entity a, t_entity b)
{
	return (a.threat_for > b.threat_for);
}

bool sortdist(t_entity a, t_entity b)
{
	return (a.dist < b.dist);
}

bool sortdest(t_entity a, t_entity b)
{
	return (a.vx < b.vx);
}

bool sortid(t_entity a, t_entity b)
{
	return (a.id < b.id);
}

bool sortnear(t_entity a, t_entity b)
{
	return (a.near_base < b.near_base);
}

int	ft_dist(int xa, int ya, int xb, int yb)
{
	return (sqrt(pow(xa - xb, 2) + pow(ya - yb, 2)));
}

void	ft_hero0(vector<t_entity> spiders, t_entity *hero, int my_mana, int other_base_x, int other_base_y, int use_control)
{
	int nb_spiders = spiders.size();
	if (nb_spiders == 0)
		return;
	sort(spiders.begin(), spiders.end(), sortdist);
	hero->dist = ft_dist(hero->x, hero->y, spiders.at(0).x, spiders.at(0).y);
	if (use_control == 1 && hero->shield_life < 1)
	{
		hero->spell = "SPELL SHIELD ";
		hero->spell = hero->spell.append(to_string(hero->id));
		hero->spell = hero->spell.append(" ");
		hero->vx = -2;
		hero->vy = -2;
	}
	else if (hero->dist <= 1280 && my_mana > 10 && spiders.at(0).shield_life == 0 && spiders.at(0).health > 1 && spiders.at(0).dist < 5000)
	{
		cerr << other_base_x << endl;
		hero->spell = "SPELL WIND ";
		hero->vx = other_base_x;
		hero->vy = other_base_y;
	}
	else
	{
		hero->spell = hero->spell.append("MOVE ");
		hero->vx = spiders.at(0).x;
		hero->vy = spiders.at(0).y;
	}
	return ;
}

void	ft_hero1(vector<t_entity> spiders, t_entity *hero, vector<t_entity> others, int other_base_x, int other_base_y)
{
	int	nb_spiders = spiders.size();
	int	nb_others = others.size();

	if (nb_others != 0)
	{
		sort(others.begin(), others.end(), sortdist);
		if (others.at(0).dist < 8000)
		{
			if(ft_dist(hero->x, hero->y, others.at(0).x, others.at(0).y) < 880)
			{
				hero->spell = "SPELL WIND ";
				hero->vx = other_base_x;
				hero->vy = other_base_y;
			}
			else
			{
				hero->spell = "MOVE ";
				hero->vx = others.at(0).x;
				hero->vy = others.at(0).y;
			}
			return ;
		}
	}
	if (nb_spiders == 0)
		return ;
	sort(spiders.begin(), spiders.end(), sortdist);
	hero->spell = "MOVE ";
	hero->vx = spiders.at(0).x;
	hero->vy = spiders.at(0).y;
	return ;
}

void	ft_hero2(vector<t_entity> spiders, t_entity *hero, int my_mana, int other_base_x, int other_base_y, int turn_count)
{
	int	nb_spiders = spiders.size();
	if (nb_spiders == 0)
		return ;
	sort(spiders.begin(), spiders.end(), sortdist);
	hero->dist = ft_dist(hero->x, hero->y, spiders.at(0).x, spiders.at(0).y);
	if (my_mana > 50 && spiders.at(0).health >= 14 && spiders.at(0).dist < 400 * 12)
	{
		hero->spell = "SPELL SHIELD ";
		hero->spell = hero->spell.append(to_string(spiders.at(0).id));
		hero->spell = hero->spell.append(" ");
		hero->vx = -2;
		hero->vy = -2;
		return ;
	}
	if (my_mana > 50 && hero->dist < 1280 && spiders.at(0).dist < 7000)
	{
		hero->spell = "SPELL WIND ";
		hero->vx = other_base_x;
		hero->vy = other_base_y;
		return ;
	}
	int i = 0;
	while (i < nb_spiders && spiders.at(i).threat_for == 2)
		i++;
	if (i == nb_spiders)
		return ;
	hero->dist = ft_dist(hero->x, hero->y, spiders.at(i).x, spiders.at(i).y);
	if (my_mana > 50 && hero->dist < 2200)
	{
		hero->spell = "SPELL CONTROL ";
		hero->spell = hero->spell.append(to_string(spiders.at(i).id));
		hero->spell = hero->spell.append(" ");
		if (turn_count % 2 == 0)
		{
			hero->vx = other_base_x;
			hero->vy = other_base_y;
		}
		else
		{
			hero->vx = other_base_x;
			hero->vy = other_base_y;
		}
		return ;
	}
	else
	{
		hero->spell = "MOVE ";
		hero->vx = spiders.at(i).x;
		hero->vy = spiders.at(i).y;
		return ;
	}
}

int main()
{
	int base_x; // The corner of the map representing your base
	int base_y;
	cin >> base_x >> base_y; cin.ignore();
	int other_base_x = 17630 - base_x;
	int other_base_y = 9000 - base_y;
	int heroes_per_player; // Always 3
	cin >> heroes_per_player; cin.ignore();
	float mid_angle = atanf(17630.0 / 9000.0);
	int	use_control = 0;
	int	turn_count = 0;

	// game loop
	while (1) {
		int	my_mana;
		for (int i = 0; i < 2; i++)
		{
			int health; // Each player's base health
			int mana; // Ignore in the first league; Spend ten mana to cast a spell
			cin >> health >> mana; cin.ignore();
			if (i == 0)
				my_mana = mana;
		}
		int entity_count; // Amount of heros and monsters you can see
		cin >> entity_count; cin.ignore();
		std::vector<t_entity> entities;
		std::vector<t_entity> heroes;
		std::vector<t_entity> others;
		std::vector<t_entity> spiders;
		std::vector<t_entity> mid_spiders;
		std::vector<t_entity> far_spiders;
		vector<t_entity>::iterator iter;
		for (int i = 0; i < entity_count; i++)
		{
			t_entity	temp;
			cin >> temp.id >> temp.type >> temp.x >> temp.y >> temp.shield_life >> temp.is_controlled >> temp.health >> temp.vx >> temp.vy >> temp.near_base >> temp.threat_for; cin.ignore();
			entities.push_back(temp);
			if (temp.type == 0)
			{
				temp.dist = ft_dist(temp.x, temp.y, base_x, base_y);
				spiders.push_back(temp);
				if (temp.dist > 5000 && temp.dist < 12000)
				{
					temp.dist = ft_dist(heroes.at(1).x, heroes.at(1).y, other_base_x, other_base_y);
					mid_spiders.push_back(temp);
				}
				else if (temp.dist >= 12000)
				{
					temp.dist = ft_dist(temp.x, temp.y, other_base_x, other_base_y);
					far_spiders.push_back(temp);
				}
			}
			else if (temp.type == 1)
			{
				temp.vx = -1;
				temp.vy = -1;
				if (temp.is_controlled == 1)
					use_control = 1;
				heroes.push_back(temp);
			}
			else
			{
				temp.dist = ft_dist(temp.x, temp.y, base_x, base_y);
				others.push_back(temp);
			}
		}
		ft_hero0(spiders, &heroes.at(0), my_mana, other_base_x, other_base_y, use_control);
		ft_hero1(mid_spiders, &heroes.at(1), others, other_base_x, other_base_y);
		ft_hero2(far_spiders, &heroes.at(2), my_mana, other_base_x, other_base_y, turn_count);
		sort(heroes.begin(), heroes.end(), sortid);
		for (int i = 0; i < heroes_per_player; i++)
		{
			if (heroes.at(i).vx == -1)
			{
				if (i == 0)
					cout << "MOVE " << floor(abs(2000 - base_x)) << " " << floor(abs(2000 - base_y)) << endl;
				else if (i == 1)
					cout << "MOVE " << floor(abs(sin(mid_angle) * 7000 - base_x)) << " " << floor(abs(cos(mid_angle) * 7000 - base_y)) << endl;
				else
					cout << "MOVE " << floor(abs(sin(mid_angle) * 14000 - base_x)) << " " << floor(abs(cos(mid_angle) * 14000 - base_y)) << endl;
			}
			else if (heroes.at(i).vx >= 0)
				cout << heroes.at(i).spell << heroes.at(i).vx << " " << heroes.at(i).vy << endl;
			else
				cout << heroes.at(i).spell << endl;
		}
		turn_count++;
	}
}
