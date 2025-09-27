#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

template<typename... Args>
class Signal {
public:
    using Slot = function<void(Args...)>;
	void connect(Slot slot) { slots.push_back(move(slot)); };
    void emit(Args... args) { for (Slot& s : slots) s(args...); }

private:
    vector<Slot> slots;
};


class Monster {
public:
    Signal<Monster&, Monster&, int, int, int> onHurt;
    Signal<Monster&, int> onHeal;

	Monster(string newName, int newMaxHealth, int newPower) 
		: name(newName), 
		  maxHealth(newMaxHealth), 
		  health(newMaxHealth), 
		  power(newPower)
	{}

	virtual ~Monster() {};

	const string& getName() const { return name; }
	int getHealth() const { return health; }
	int getPower() const { return power; }
	bool isDead() const { return health <= 0; }

	virtual void act(Monster& target) = 0;

	virtual void hit(Monster& target) { target.hurt(*this); }

	virtual void hurt(Monster &source) {
		int damage = source.getPower();
		inflict(damage);
		onHurt.emit(source, *this, damage, damage, 0);
	}

	void inflict(int damage) { 
		health -= damage;  
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

	void act(Monster& target) override {
		for (int i = 0; i < numAttacks; i++) hit(target);
	}

private:
	int numAttacks;
};

class Troll : public Monster {
public:
	Troll() : Monster("Troll", 80, 15), regen(5) {}

	void act(Monster& target) override {
		hit(target);
		heal();
	}

	void heal() {
		int gain = min(regen, maxHealth - getHealth());
		health += gain;
		onHeal.emit(*this, gain);
	}

private:
	int regen;
};

class Orc : public Monster {
public:
	Orc() : Monster("Orc", 63, 9), blockDamage(6), reflectDamage(3) {}

	void act(Monster& target) override { hit(target); }

	void hurt(Monster& source) override {
		int damage = source.getPower();
		int trueDamage = damage - blockDamage;
		health -= trueDamage;
		source.inflict(reflectDamage);
		onHurt.emit(
			source,
			*this, 
			damage,
			trueDamage,
			reflectDamage
		);
	}

private:
	int blockDamage;
	int reflectDamage;
};

class Team {
public:
	Team(string newName) : name(newName) {}

	void add(unique_ptr<Monster> m) { lineup.push_back(move(m)); }
	bool empty() const { return lineup.empty(); }
	void popFront() { lineup.erase(lineup.begin()); }
	Monster& front() { return *lineup.front(); }
	const string getName() const { return name; }

	// string getLabel() {
	// 	if (empty()) return "";
	// 	return front().getName() + "(" + to_string(front().getHealth()) + ")";
	// }

    void forEach(const function<void(Monster&)>& apply) const {
        for (auto& monster: lineup) apply(*monster);
    }

	friend ostream& operator<<(ostream& os, Team& team) {
		if (team.empty()) return os;
		os << team.front().getName() << "(" 
		   << to_string(team.front().getHealth()) << ")";
		return os;
	}

private:
	string name;
    vector<unique_ptr<Monster>> lineup;
};

class TextRenderer {
public:
	void observe(Team& teamRed, Team& teamBlue) {
		teamRed.forEach([&](Monster& monster) {
			monster.onHurt.connect([&](auto&&... args) {
				renderAttack(teamBlue, teamRed, args...);
			});
		});

		teamBlue.forEach([&](Monster& monster) {
			monster.onHurt.connect([&](auto&&... args) {
				renderAttack(teamRed, teamBlue, args...);
			});
		});

		teamRed.forEach([&](Monster& monster) {
			monster.onHeal.connect([&](Monster& source, int healthGain) {
				renderHeal(teamRed, monster, healthGain);
			});
		});

		teamBlue.forEach([&](Monster& monster) {
			monster.onHeal.connect([&](Monster& source, int healthGain) {
				renderHeal(teamBlue, monster, healthGain);
			});
		});
	}

	void renderStart() const {
        cout << "Battle Start!" << endl << endl;
    }

    void renderTurnBanner(int turnIdx) const {
        cout << "-----------------------------------------------------------"
			 << "------------------------------------------------------------"
			 << endl;
		cout << "Turn " << turnIdx << endl << endl;
    }

    void renderTeams(Team& teamRed, Team& teamBlue) const {
        cout << "[ Red | " << teamRed
			 << " ] ... [ " << teamBlue
			 << " | Blue ]" << endl << endl;
    }

	void renderDeath(Team& team, Monster& monster) const {
		cout << team.getName() << " " 
			 << monster.getName() << " has died" 
			 << endl;
	}

	void renderAttack(
		Team& teamSource, 
		Team& teamTarget, 
		Monster& monsterSource, 
		Monster& monsterTarget, 
		int damage, 
		int trueDamage, 
		int reflectDamage
	) const {
			cout << teamSource.getName() << " "
				 << monsterSource.getName() << " attacks "
				 << teamTarget.getName() << " "
				 << monsterTarget.getName() << " for "
				 << damage << " damage";

			if (trueDamage != damage) {
				cout << ", dealing " << trueDamage << " damage";
			}

			if (reflectDamage > 0) {
				cout << ", and receiving " << reflectDamage 
					 << " reflected damage";
			}

			cout << endl;
	}

	void renderHeal(Team& team, Monster& monster, int healthGain) const {
		cout << team.getName() << " " 
			 << monster.getName() << " regenerates " 
			 << healthGain << " health\n";
	}

	void renderWinner(Team& team) {
        cout << "Battle over. " << team.getName() 
			 << " team wins!" << endl << endl;
	}
};

class Game {
public:
	void battle(Team& teamRed, Team& teamBlue) {
        TextRenderer renderer;
		
		renderer.observe(teamRed, teamBlue);
        renderer.renderStart();
        renderer.renderTeams(teamRed, teamBlue);

		int turnIdx = 1;
		while (!teamRed.empty() && !teamBlue.empty()) {
			renderer.renderTurnBanner(turnIdx);
			renderer.renderTeams(teamRed, teamBlue);

			teamRed.front().act(teamBlue.front());
			teamBlue.front().act(teamRed.front());

			if (teamRed.front().isDead()) {
				renderer.renderDeath(teamRed, teamRed.front());
				teamRed.popFront();
			}

			if (teamBlue.front().isDead()) {
				renderer.renderDeath(teamBlue, teamBlue.front());
				teamBlue.popFront();
			}

			turnIdx++;
		}

		renderer.renderWinner(teamRed.empty() ? teamBlue : teamRed);
        renderer.renderTeams(teamRed, teamBlue);
	}
};

int main() {
	Team red("Red"), blue("Blue");
	red.add(make_unique<Goblin>());
    blue.add(make_unique<Troll>());
    Game().battle(red, blue);

	return 0;
}
