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
	int	dist_mybase; //distance to base for monster;
	int dist_obase;
	int	dist_hero[3];
	string spell;
}	t_entity;

bool sortthreat(t_entity a, t_entity b)
{
	return (a.threat_for > b.threat_for);
}

bool sortdist_mybase(t_entity a, t_entity b)
{
	return (a.dist_mybase < b.dist_mybase);
}

bool sortdist_obase(t_entity a, t_entity b)
{
	return (a.dist_obase < b.dist_obase);
}

bool sortdist_h0(t_entity a, t_entity b)
{
	return (a.dist_hero[0] < b.dist_hero[0]);
}

bool sortdist_h1(t_entity a, t_entity b)
{
	return (a.dist_hero[1] < b.dist_hero[1]);
}

bool sortdist_h2(t_entity a, t_entity b)
{
	return (a.dist_hero[2] < b.dist_hero[2]);
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

	cerr << "id :" << hero->id << " dist :" << target.dist_hero[hero->id % 3] << " target :" << target.id << " dist :" << ft_dist(target.x, target.y, hero->x, hero->y) << endl;
	if (target.dist_hero[hero->id % 3] > 1600)
	{
		hero->vx = target.x + (target.vx * target.dist_hero[hero->id % 3] / 800);
		hero->vy = target.y + (target.vy * target.dist_hero[hero->id % 3] / 800);
		return ;
	}
	for (i = 0; i < spiders.size(); ++i)
	{
		if (spiders.at(i).dist_hero[hero->id % 3] <= 1600
			&& ft_dist(target.x, target.y, spiders.at(i).x, spiders.at(i).y) <= 1600
			&& target.id != spiders.at(i).id)
			casualties.push_back(spiders.at(i));
	}
	if (hero->id %3 == 0)
		sort(casualties.begin(), casualties.end(), sortdist_h0);
	else if (hero->id %3 == 1)
		sort(casualties.begin(), casualties.end(), sortdist_h1);
	else
		sort(casualties.begin(), casualties.end(), sortdist_h2);
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
	sort(spiders.begin(), spiders.end(), sortdist_mybase);
	sort(spiders.begin(), spiders.end(), sortthreat);
	i = 0;
	while (i < nb_spiders
		&& (spiders.at(i).threat_for == 2))
			i++;
	if (i == nb_spiders)
		return;
	if (use_control == 1 && hero->shield_life < 1)
	{
		hero->spell = "SPELL SHIELD ";
		hero->spell = hero->spell.append(to_string(hero->id));
		hero->spell = hero->spell.append(" ");
		hero->vx = -1000;
		hero->vy = -1000;
	}
	else if (spiders.at(i).dist_hero[hero->id % 3] <= 1280
		&& my_mana >= 10
		&& spiders.at(i).shield_life == 0
		&& spiders.at(i).health > 2
		&& spiders.at(i).dist_mybase < 4000)
	{
		hero->spell = "SPELL WIND ";
		hero->vx = spiders.at(i).x + hero->x + base_x;
		hero->vy = spiders.at(i).y + hero->y + base_y;
	}
	else
	{
		hero->spell = hero->spell.append("MOVE ");
		ft_barycentre(spiders, hero, spiders.at(i));
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
	sort(spiders.begin(), spiders.end(), sortdist_h1);
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
		if (spiders.at(target).dist_hero[1] <= 1280
			&& my_mana >= 20
			&& spiders.at(target).shield_life == 0
			&& spiders.at(target).health > 2
			&& spiders.at(target).dist_mybase < 4000)
		{
			hero->spell = "SPELL WIND ";
			hero->vx = abs(spiders.at(target).x + hero->x - base_x);
			hero->vy = abs(spiders.at(target).y + hero->y - base_y);
			return ;
		}
	}
	hero->spell = "MOVE ";
	ft_barycentre(spiders, hero, spiders.at(target));
	return ;
}

int	ft_hero2(vector<t_entity> spiders, vector<t_entity> others, t_entity *hero, int my_mana, int base_x, int base_y, int other_base_x, int other_base_y, int old_target, int turn_count)
{
	int	i;
	int	nb_spiders = spiders.size();

	sort(others.begin(), others.end(), sortdist_obase);
	sort(spiders.begin(), spiders.end(), sortdist_obase);
	if (old_target >= 0)
	{
		i = 0;
		while (i < nb_spiders && spiders.at(i).id != old_target)
			i++;
		if (i != nb_spiders
			&& others.at(0).dist_obase <= 5000
			&& spiders.at(i).dist_hero[2] <= 2200
			&& spiders.at(i).shield_life == 0)
		{
			hero->spell = "SPELL SHIELD ";
			hero->spell = hero->spell.append(to_string(spiders.at(i).id));
			hero->spell = hero->spell.append(" ");
			hero->vx = -1000;
			hero->vy = -1000;
			return (-1);
		}
		if (i != nb_spiders && spiders.at(i).dist_hero[2] <= 1280
			&& spiders.at(i).shield_life == 0)
		{
			hero->spell = "SPELL WIND ";
			hero->vx = -spiders.at(i).x + hero->x + other_base_x;
			hero->vy = -spiders.at(i).y + hero->y + other_base_y;
			return (-1);
		}
	}
	if (my_mana < 60)
	{
		sort(spiders.begin(), spiders.end(), sortdist_h2);
		i = 0;
		while (i < nb_spiders
			&& (spiders.at(i).near_base == 1 || spiders.at(i).shield_life > 0))
			i++;
		if (i == nb_spiders
			|| ft_dist(spiders.at(i).x, spiders.at(i).y, abs(12815 - base_x), abs(0 -base_y)) > 5 * 800)
		{
			if (i != nb_spiders)
				cerr << "dist bord " << ft_dist(spiders.at(i).x, spiders.at(i).y, 12815, 0) << " id :" << spiders.at(i).id << endl;
			hero->spell = "MOVE ";
			hero->vx = abs(12815 - base_x);
			hero->vy = abs(0 - base_y);
			return (-1);
		}
		hero->spell = "MOVE ";
		ft_barycentre(spiders, hero, spiders.at(i));
		return (-1);
	}
	if (others.size() != 0 && others.at(0).dist_obase <= 5000)
	{
		i = 0;
		while (i < nb_spiders
			&& (spiders.at(i).health < spiders.at(i).dist_obase * 12 / 400
				|| spiders.at(i).shield_life > 0))
			i++;
		if (i == nb_spiders)
			return (-1);
		if (spiders.at(i).dist_obase <= 5000)
		{
			if (spiders.at(i).dist_hero[2] <= 2200)
			{
				hero->spell = "SPELL SHIELD ";
				hero->spell = hero->spell.append(to_string(spiders.at(i).id));
				hero->spell = hero->spell.append(" ");
				hero->vx = -1000;
				hero->vy = -1000;
				return (-1);
			}
			hero->spell = "MOVE ";
			ft_barycentre(spiders, hero, spiders.at(i));
			return (-1);
		}
		if (spiders.at(i).dist_hero[2] <= 1280)
		{
			hero->spell = "SPELL WIND ";
			hero->vx = -spiders.at(i).x + hero->x + other_base_x;
			hero->vy = -spiders.at(i).y + hero->y + other_base_y;
			return (spiders.at(i).id);
		}
		if (spiders.at(i).dist_obase < 7000)
		{
			hero->spell = "MOVE ";
			ft_barycentre(spiders, hero, spiders.at(i));
			return (-1);
		}
	}
	i = 0;
	while (i < nb_spiders && spiders.at(i).shield_life > 0)
		i++;
	if (i == nb_spiders)
		return (-1);
	if (spiders.at(i).dist_hero[2] <= 1280)
	{
		if (spiders.at(i).dist_obase <= 5000)
		{
			hero->spell = "SPELL WIND ";
			hero->vx = -spiders.at(i).x + hero->x + other_base_x;
			hero->vy = -spiders.at(i).y + hero->y + other_base_y;
			return (-1);
		}
		if (spiders.at(i).dist_obase <= 7000)
		{
			hero->spell = "SPELL WIND ";
			hero->vx = -spiders.at(i).x + hero->x + other_base_x;
			hero->vy = -spiders.at(i).y + hero->y + other_base_y;
			return (spiders.at(i).id);
		}
	}
	if (nb_spiders != 0 && spiders.at(0).dist_obase < 7000)
	{
		hero->spell = "MOVE ";
		ft_barycentre(spiders, hero, spiders.at(0));
		return (-1);
	}
	return (-1);
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
	int	old_target = -1;

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
			temp.dist_mybase = ft_dist(temp.x, temp.y, base_x, base_y);
			temp.dist_obase = ft_dist(temp.x, temp.y, other_base_x, other_base_y);
			if (temp.type == 0)
			{
				temp.dist_hero[0] = ft_dist(temp.x, temp.y, heroes.at(0).x, heroes.at(0).y);
				temp.dist_hero[1] = ft_dist(temp.x, temp.y, heroes.at(1).x, heroes.at(1).y);
				temp.dist_hero[2] = ft_dist(temp.x, temp.y, heroes.at(2).x, heroes.at(2).y);
				spiders.push_back(temp);
				if (temp.dist_mybase > 5000 && temp.dist_mybase < 12000)
					mid_spiders.push_back(temp);
				else if (temp.dist_mybase >= 12000)
					far_spiders.push_back(temp);
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
				temp.dist_hero[0] = ft_dist(temp.x, temp.y, heroes.at(0).x, heroes.at(0).y);
				temp.dist_hero[1] = ft_dist(temp.x, temp.y, heroes.at(1).x, heroes.at(1).y);
				temp.dist_hero[2] = ft_dist(temp.x, temp.y, heroes.at(2).x, heroes.at(2).y);
				others.push_back(temp);
			}
		}
		ft_hero0(spiders, &heroes.at(0), my_mana, base_x, base_y, use_control, turn_count);
		if (turn_count < 50)
			ft_hero1(mid_spiders, &heroes.at(1), base_x, base_y, my_mana, turn_count);
		else
			ft_hero1(spiders, &heroes.at(1), base_x, base_y, my_mana, turn_count);
		old_target = ft_hero2(spiders, others, &heroes.at(2), my_mana, base_x, base_y, other_base_x, other_base_y, old_target, turn_count);
		sort(heroes.begin(), heroes.end(), sortid);
		for (int i = 0; i < heroes_per_player; i++)
		{
			if (heroes.at(i).vx == -1)
			{
				if (i == 0)
					cout << "MOVE " << floor(abs(cos(mid_angle) * 5000 - base_x)) << " " << floor(abs(sin(mid_angle) * 5000 - base_y)) << " H0" << endl;
				else if (i == 1 && turn_count > 50)
					cout << "MOVE " << floor(abs(cos(mid_angle) * 7000 - base_x)) << " " << floor(abs(sin(mid_angle) * 7000 - base_y)) << " H1" << endl;
				else if (i == 1)
					cout << "MOVE " << floor(abs(8815 - base_x)) << " " << floor(abs(0 - base_y)) << " H1" << endl;
				else
					cout << "MOVE " << floor(abs(cos(mid_angle) * 4500 - other_base_x)) << " " << floor(abs(sin(mid_angle) * 4500 - other_base_y)) << " H2" << endl;
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
