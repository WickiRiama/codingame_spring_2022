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
	return (a.near_base > b.near_base);
}

int	ft_dist(int xa, int ya, int xb, int yb)
{
	return (sqrt(pow(xa - xb, 2) + pow(ya - yb, 2)));
}

void	ft_barycentre(vector<t_entity> spiders, t_entity *hero, t_entity target)
{
	std::vector<t_entity>	casualties;
	int	i;

	if (ft_dist(hero->x, hero->y, target.x, target.y) > 1600)
	{
		hero->vx = target.x + (target.vx * ft_dist(hero->x, hero->y, target.x, target.y) / 800);
		hero->vy = target.y + (target.vy * ft_dist(hero->x, hero->y, target.x, target.y) / 800);
		return ;
	}
	for (i = 0; i < spiders.size(); ++i)
	{
		if (ft_dist(hero->x, hero->y, spiders.at(i).x, spiders.at(i).y) <= 1600 
			&& ft_dist(target.x, target.y, spiders.at(i).x, spiders.at(i).y) <= 1600
			&& target.id != spiders.at(i).id)
		{
			casualties.push_back(spiders.at(i));
			casualties.at(casualties.size() - 1).dist = ft_dist(hero->x, hero->y, spiders.at(i).x, spiders.at(i).y);
		}
	}
	sort(casualties.begin(), casualties.end(), sortdist);
	while (casualties.size() > 0)
	{
		hero->vx = target.x;
		hero->vy = target.y;
		for (i = 0; i < casualties.size(); ++i)
		{
			hero->vx += casualties.at(i).x;
			hero->vy += casualties.at(i).y;
		}
		hero->vx = hero->vx / (casualties.size() + 1);
		hero->vy = hero->vy / (casualties.size() + 1);
		if (ft_dist(hero->vx, hero->vy, hero->x, hero->y) <= 800 && ft_dist(hero->vx, hero->vy, target.x, target.y) <= 800)
			break;
		casualties.pop_back();		
	}
	if (casualties.size() == 0)
	{
		hero->vx = target.x;
		hero->vy = target.y;
	}
	return ;
}

void	ft_hero0(vector<t_entity> spiders, t_entity *hero, int my_mana, int base_x, int base_y, int use_control, int turn_count)
{
	int nb_spiders = spiders.size();
	int i;
	if (nb_spiders == 0)
		return;
	sort(spiders.begin(), spiders.end(), sortdist);
	sort(spiders.begin(), spiders.end(), sortthreat);
	i = 0;
	while (i < nb_spiders 
		&& (spiders.at(i).threat_for == 2))
			i++;
	if (i == nb_spiders)
		return;
	hero->dist = ft_dist(hero->x, hero->y, spiders.at(i).x, spiders.at(i).y);
	if (use_control == 1 && hero->shield_life < 1)
	{
		hero->spell = "SPELL SHIELD ";
		hero->spell = hero->spell.append(to_string(hero->id));
		hero->spell = hero->spell.append(" ");
		hero->vx = -1000;
		hero->vy = -1000;
	}
	else if (hero->dist <= 1280 && my_mana >= 10 && spiders.at(i).shield_life == 0 && spiders.at(i).health > 2 && spiders.at(i).dist < 3000)
	{
		hero->spell = "SPELL WIND ";
		hero->vx = abs(spiders.at(i).x + hero->x - base_x);
		hero->vy = abs(spiders.at(i).y + hero->y - base_y);
	}
	else
	{
		hero->spell = hero->spell.append("MOVE ");
		ft_barycentre(spiders, hero, spiders.at(i));
		// hero->vx = spiders.at(0).x;
		// hero->vy = spiders.at(0).y;
	}
	return ;
}

void	ft_hero1(vector<t_entity> spiders, t_entity *hero, int base_x, int base_y, int my_mana, int turn_count)
{
	int	nb_spiders = spiders.size();
	int i;
	int target;
	if (nb_spiders == 0)
		return ;
	sort(spiders.begin(), spiders.end(), sortdist);
	target = 0;
	if (turn_count > 50)
	{
		sort(spiders.begin(), spiders.end(), sortthreat);
		i = 0;
		while (i < nb_spiders && (spiders.at(i).threat_for == 2))
			i++;
		if (i == nb_spiders)
			return ;
		target = i;
		if (spiders.at(target).shield_life == 0)
		{
			i++;
			if (i != nb_spiders)
				target = i;
		}
		hero->dist = ft_dist(hero->x, hero->y, spiders.at(target).x, spiders.at(target).y);
		if (hero->dist <= 1280 && my_mana >= 20 && spiders.at(target).shield_life == 0 && spiders.at(target).health > 2 && spiders.at(target).dist < 3000)
		{
			hero->spell = "SPELL WIND ";
			hero->vx = abs(spiders.at(target).x + hero->x - base_x);
			hero->vy = abs(spiders.at(target).y + hero->y - base_y);
		}
	}
	hero->spell = "MOVE ";
	ft_barycentre(spiders, hero, spiders.at(target));
	// hero->vx = spiders.at(0).x;
	// hero->vy = spiders.at(0).y;
	return ;
}

void	ft_hero2(vector<t_entity> spiders, t_entity *hero, int my_mana, int other_base_x, int other_base_y, int turn_count)
{
	int	nb_spiders = spiders.size();
	int	i;
	if (nb_spiders == 0)
		return ;
	sort(spiders.begin(), spiders.end(), sortdist);
	if (my_mana >= 30)
	{
		i = 0;
		while (i < nb_spiders 
			&& (spiders.at(i).health < (spiders.at(i).dist) / 400 * 2
				|| spiders.at(i).dist >= 400 * 12 
				|| spiders.at(i).shield_life != 0
				|| ft_dist(hero->x, hero->y, spiders.at(i).x, spiders.at(i).y) > 2200))
			i++;
		if (i < nb_spiders)
		{
			hero->spell = "SPELL SHIELD ";
			hero->spell = hero->spell.append(to_string(spiders.at(i).id));
			hero->spell = hero->spell.append(" ");
			hero->vx = -1000;
			hero->vy = -1000;
			return ;
		}
		i = 0;
		while (i < nb_spiders 
			&& (ft_dist(hero->x, hero->y, spiders.at(i).x, spiders.at(i).y) > 1280 
				|| spiders.at(i).dist > 8000))
			i++;
		if (i < nb_spiders)
		{
			cerr << "here" << endl;
			hero->spell = "SPELL WIND ";
			hero->vx = hero->x - spiders.at(i).x + other_base_x;
			hero->vy = hero->y - spiders.at(i).y + other_base_y;
			/*
			if (turn_count % 2 == 0)
			{
				hero->vx = abs(2000 - other_base_x);
				hero->vy = other_base_y;
			}
			else
			{
				hero->vx = other_base_x;
				hero->vy = abs(2000 - other_base_y);
			}*/
			return ;
		}
	}
	i = 0;
	while (i < nb_spiders && (spiders.at(i).threat_for == 2 || spiders.at(i).shield_life != 0))
		i++;
	if (i == nb_spiders)
		return ;/*
	hero->dist = ft_dist(hero->x, hero->y, spiders.at(i).x, spiders.at(i).y);
	if (my_mana > 50 && hero->dist < 2200)
	{
		hero->spell = "SPELL CONTROL ";
		hero->spell = hero->spell.append(to_string(spiders.at(i).id));
		hero->spell = hero->spell.append(" ");
		if (turn_count % 2 == 0)
		{
			hero->vx = abs(4000 - other_base_x);
			hero->vy = other_base_y;
		}
		else
		{
			hero->vx = other_base_x;
			hero->vy = abs(4000 - other_base_y);
		}
		return ;
	}*/
	else
	{
		hero->spell = "MOVE ";
		ft_barycentre(spiders, hero, spiders.at(i));
		// hero->vx = spiders.at(i).x;
		// hero->vy = spiders.at(i).y;
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
	float mid_angle = 0.0;
	int	use_control = 0;
	int	turn_count = 0;
	int	mod_patrol = 1;

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
					temp.dist = ft_dist(heroes.at(1).x, heroes.at(1).y, temp.x, temp.y);
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
		ft_hero0(spiders, &heroes.at(0), my_mana, base_x, base_y, use_control, turn_count);
		cerr << "hero0 OK" << endl;
		if (turn_count < 50)
			ft_hero1(mid_spiders, &heroes.at(1), base_x, base_y, my_mana, turn_count);
		else
			ft_hero1(spiders, &heroes.at(1), base_x, base_y, my_mana, turn_count);
		cerr << "hero1 OK" << endl;
		// ft_hero2(far_spiders, &heroes.at(1), my_mana, other_base_x, other_base_y, turn_count);
		ft_hero2(far_spiders, &heroes.at(2), my_mana, other_base_x, other_base_y, turn_count);
		cerr << "hero2 OK" << endl;
		sort(heroes.begin(), heroes.end(), sortid);
		cerr << (mid_angle + (turn_count % 10) * M_PI / 20) << endl;
		for (int i = 0; i < heroes_per_player; i++)
		{
			if (heroes.at(i).vx == -1)
			{
				if (i == 0)
					cout << "MOVE " << floor(abs(cos(mid_angle) * 5000 - base_x)) << " " << floor(abs(sin(mid_angle) * 5000 - base_y)) << endl;
				else if (i == 1 && turn_count > 50)
					cout << "MOVE " << floor(abs(cos(mid_angle) * 7000 - base_x)) << " " << floor(abs(sin(mid_angle) * 7000 - base_y)) << endl;
				else if (i == 1)
					cout << "MOVE " << floor(abs(8815 - base_x)) << " " << floor(abs(0 - base_y)) << endl;
				else
					cout << "MOVE " << floor(abs(cos(mid_angle) * 4500 - other_base_x)) << " " << floor(abs(sin(mid_angle) * 4500 - other_base_y)) << endl;
			}
			else if (heroes.at(i).vx != -1000 || heroes.at(i).vy != -1000)
				cout << heroes.at(i).spell << heroes.at(i).vx << " " << heroes.at(i).vy << endl;
			else
				cout << heroes.at(i).spell << endl;
		}
		turn_count++;
		mid_angle += M_PI /10 * mod_patrol;
		if (mid_angle >= (M_PI / 2) - (M_PI / 20) )
			mod_patrol = -1;
		if (mid_angle <= M_PI / 20)
			mod_patrol = 1;
	}
}
