#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Monster {
public:
	Monster(string newName, int newMaxHealth, int newPower) 
		: name(newName), 
		  maxHealth(newMaxHealth), 
		  health(newMaxHealth), 
		  power(newPower)
	{}

	virtual ~Monster() {};

	const string& getName() const { return name; }
	int getHealth() const { return health; }
	bool isDead() const { return health <= 0; }

	virtual void act(Monster& opp) = 0;

	virtual void hit(Monster& opp) {
		int reflectDamage = opp.hurt(power);
		health = max(0, health - reflectDamage);
	}

	virtual int hurt(int damage) {
		int trueDamage = clamp(damage, 0, health);
		health -= trueDamage;
		return 0;
	}
	
protected:
	string name;
	int maxHealth;
	int health;
	int power;
};

class Goblin : public Monster {
public:
	Goblin() : Monster("Goblin", 50, 10), numAttacks(4) {}

	void act(Monster& opp) override {
		for (int i = 0; i < numAttacks && !opp.isDead(); i++) {
			hit(opp);
		}
	}

private:
	int numAttacks;
};

class Troll : public Monster {
public:
	Troll() : Monster("Troll", 80, 15), healthRegen(5) {}

	void act(Monster& opp) override {
		hit(opp);
		heal();
	}

	void heal() {
		int healthMissing = maxHealth - health;
		int healthGain = min(healthMissing, healthRegen);
		health += healthGain;
	}

private:
	int healthRegen;
};

class Orc : public Monster {
public:
	Orc() : Monster("Orc", 63, 9), blockDamage(6), reflectDamage(3) {}

	void act(Monster& opp) override {
		hit(opp);
	}

	int hurt(int damage) override {
		int reducedDamage = max(0, damage - blockDamage);
		int trueDamage = clamp(reducedDamage, 0, health);
		health -= trueDamage;
		return reflectDamage;
	}

private:
	int blockDamage;
	int reflectDamage;
};

class Game {
public:
    void setup(vector<unique_ptr<Monster>> newTeamRed,
               vector<unique_ptr<Monster>> newTeamBlue) {
        teamRed = move(newTeamRed);
		teamBlue = move(newTeamBlue);
    }

	void battle() {
		int turnIdx = 1;
		while (!teamRed.empty() && !teamBlue.empty()) {
			teamRed.front()->act(*teamBlue.front());
			teamBlue.front()->act(*teamRed.front());

			if (teamRed.front()->isDead()) teamRed.erase(teamRed.begin());
			if (teamBlue.front()->isDead()) teamBlue.erase(teamBlue.begin());

			turnIdx++;
		}
	}
private:
	vector<unique_ptr<Monster>> teamRed;
	vector<unique_ptr<Monster>> teamBlue;
};

int main() {
	Game game;
	vector<unique_ptr<Monster>> teamRed;
	vector<unique_ptr<Monster>> teamBlue;

	teamRed.push_back(make_unique<Goblin>());
	teamBlue.push_back(make_unique<Troll>());
	game.setup(move(teamRed), move(teamBlue));
    game.battle();

	return 0;
}
