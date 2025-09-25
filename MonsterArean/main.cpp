#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#define ATTRIBUTE_MAX 100
#define ATTRIBUTE_MIN 0

using namespace std;

class Monster {
public:
	Monster() : maxHealth(ATTRIBUTE_MAX), health(maxHealth), damage(ATTRIBUTE_MIN) {}

	virtual string getName() = 0;

	virtual int getHealth() {
		return health;
	};

	virtual void playTurn(Monster& opp) = 0;

	virtual void hit(Monster& opp) {
		cout << getName() << " attacks " << opp.getName() << " for " << damage << " damage" << endl;
		opp.hurt(*this, damage);
	}

	virtual void hurt(Monster &opp, int amount) {
		health -= amount;

		if (health <= 0) {
			cout << getName() << " has died" << endl;
		}
	};

	bool isDead() {
		return health <= 0;
	}

protected:
	int maxHealth;
	int health;
	int damage;
};

class Goblin : public Monster {
public:
	Goblin() : Monster() {
		damage = 10;
		numAttacks = 4;
	}

	string getName() override {
		return "Goblin";
	}

	void playTurn(Monster& opp) override {
		for (int i = 0; i < numAttacks; i++) {
			hit(opp);
		}
	}

private:
	int numAttacks;
};

class Troll : public Monster {
public:
	Troll() : Monster() {
		healthRegen = 10;
		damage = 30;
	}

	string getName() override {
		return "Troll";
	}

	void playTurn(Monster& opp) override {
		hit(opp);
		heal();
	}

	void heal() {
		int healthHealed = maxHealth > healthRegen + health ? healthRegen : maxHealth - health;
		cout << getName() << " regenerates " << healthHealed << " health" << endl;
		health += healthHealed;
	}

private:
	int healthRegen;
};

class Orc : public Monster {
public:
	Orc() : Monster() {
		damage = 5;
		blockDamage = 10;
		reflectDamage = 5;
	}

	string getName() override {
		return "Orc";
	}

	void playTurn(Monster& opp) override {
		hit(opp);
	}

	void hurt(Monster &opp, int amount) override {
		health -= amount  - blockDamage;
		opp.hurt(*this, reflectDamage);
	}

private:
	int blockDamage;
	int reflectDamage;
};

class Game {
public:
	void battle(vector<unique_ptr<Monster>>& teamRed, vector<unique_ptr<Monster>>& teamBlue) {
		cout << "Battle Start" << endl;
		printComposition(teamRed, teamBlue);

		int turnIdx = 1;
		while (!teamRed.empty() && !teamBlue.empty()) {
			cout << "Turn " << turnIdx << endl;
			printComposition(teamRed, teamBlue);

			teamRed[0]->playTurn(*teamBlue[0]);

			if (teamBlue[0]->isDead()) {
				teamBlue.erase(teamBlue.begin());
				break;
			}

			teamBlue[0]->playTurn(*teamRed[0]);

			if (teamRed[0]->isDead()) {
				teamRed.erase(teamRed.begin());
				break;
			}

			turnIdx++;
		}

		string winner = teamRed.empty() ? "Blue" : "Red";
		cout << "Battle Over. " << winner << " team wins!" << endl;
		printComposition(teamRed, teamBlue);
	}

	void printComposition(vector<unique_ptr<Monster>>& teamRed, vector<unique_ptr<Monster>>& teamBlue) {
		string teamRedName = teamRed.empty() ? "" : teamRed[0]->getName() + to_string(teamRed[0]->getHealth());
		string teamBlueName = teamBlue.empty() ? "" : teamBlue[0]->getName() + to_string(teamBlue[0]->getHealth());

		cout << "[ Red | " << teamRedName;
		cout << " ] ... [ " << teamBlueName;
		cout << " | Blue ]" << endl;
	}
};

int main() {
	Game game;

	vector<unique_ptr<Monster>> teamRed;
	vector<unique_ptr<Monster>> teamBlue;
	teamRed.push_back(unique_ptr<Monster>(new Goblin()));
	teamBlue.push_back(unique_ptr<Monster>(new Troll()));
	game.battle(teamRed, teamBlue);

	return 0;
}
