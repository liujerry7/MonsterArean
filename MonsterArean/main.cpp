#include <iostream>
#include <string>
#include <vector>

#define ATTRIBUTE_MAX 100
#define ATTRIBUTE_MIN 0

using namespace std;

class Monster {
public:
	virtual string getName() = 0;

	virtual int getHealth() {
		return health;
	};

protected:
	int maxHealth;
	int health;
	int damage;
};

class Goblin : public Monster {
public:
	string getName() override {
		return "Goblin";
	}

private:
	int numAttacks;
};

class Troll : public Monster {
public:
	string getName() override {
		return "Troll";
	}

private:
	int healthRegen;
};

class Orc : public Monster {
public:
	string getName() override {
		return "Orc";
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
			teamRed.erase(teamRed.begin());
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
