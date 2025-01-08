#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <sstream>
#include <string>

using namespace std;

enum Direction { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };
const std::string PRESET_SCENARIO = //"4 4 20 1 1 0 0 1 100 0 3 0 100\n";
"3 3 5\n 2 1\n 1 2 1 1\n 1 1 0 2\n 0 2 1 2\n";    


class Animal {
protected:
    int posX, posY;
    Direction direction;
    int turnPeriod;
    int age;
    int moveSteps;
    int turnCounter;

public:
    Animal(int x, int y, Direction d, int k, int steps) :
        posX(x), posY(y), direction(d), turnPeriod(k), age(0), 
        moveSteps(steps), turnCounter(0) {}

    virtual ~Animal() = default;

    void move(int fieldWidth, int fieldHeight) {
        for(int i = 0; i < moveSteps; ++i) {
            switch(direction) {
                case UP: posY = (posY - 1 + fieldHeight) % fieldHeight; break;
                case RIGHT: posX = (posX + 1) % fieldWidth; break;
                case DOWN: posY = (posY + 1) % fieldHeight; break;
                case LEFT: posX = (posX - 1 + fieldWidth) % fieldWidth; break;
            }
        }
        
        turnCounter++;
        if(turnCounter >= turnPeriod) {
            direction = Direction((direction + 1) % 4);
            turnCounter = 0;
        }
    }

    virtual bool isHungry() const = 0;
    virtual void eat() = 0;
    virtual bool canReproduce() const = 0;
    virtual Animal* reproduce() const = 0;
    virtual bool isDead() const = 0;
    virtual bool isPredator() const = 0;

    void increaseAge() { age++; }

    int getX() const { return posX; }
    int getY() const { return posY; }
    Direction getDirection() const { return direction; }
    int getTurnPeriod() const { return turnPeriod; }
};

class Prey : public Animal {
private:
    static const int MAX_AGE = 10;
    bool hasReproduced1, hasReproduced2;

public:
    Prey(int x, int y, Direction d, int k) : 
        Animal(x, y, d, k, 1), hasReproduced1(false), hasReproduced2(false) {}

    bool isHungry() const override { return false; }
    void eat() override {}
    
    bool canReproduce() const override {
		return (age == 5 && !hasReproduced1) || (age == 10 && !hasReproduced2);
    }

    Animal* reproduce() const override {
        return new Prey(posX, posY, direction, turnPeriod);
    }

    bool isDead() const override { return age >= MAX_AGE; }
    bool isPredator() const override { return false; }
};

class Predator : public Animal {
private:
    static const int MAX_AGE = 20;
    int preyEaten;

public:
    Predator(int x, int y, Direction d, int k) : 
        Animal(x, y, d, k, 2), preyEaten(0) {}

    bool isHungry() const override { return true; }

    void eat() override { 
        preyEaten++; 
    }
	
	void reduceHunger(int amount) {
		preyEaten = std::max(0, preyEaten - amount);
	}

    bool canReproduce() const override {
        return preyEaten >= 2;
    }

    Animal* reproduce() const override {
        return new Predator(posX, posY, direction, turnPeriod);
    }

    bool isDead() const override { return age >= MAX_AGE; }
    bool isPredator() const override { return true; }
};

class GameSimulation {
private:
    int width, height;
    std::vector<std::unique_ptr<Animal>> animals;

    void moveAnimals() {
        for(auto& animal : animals) {
            animal->move(width, height);
        }
    }

   void processPredation() {
    std::vector<size_t> preyToRemove;
    
    // First pass: mark victims
    for(size_t predIdx = 0; predIdx < animals.size(); predIdx++) {
        if(!animals[predIdx]->isPredator()) continue;
        
        for(size_t preyIdx = 0; preyIdx < animals.size(); preyIdx++) {
            if(animals[preyIdx]->isPredator()) continue;
            
            if(animals[predIdx]->getX() == animals[preyIdx]->getX() && 
               animals[predIdx]->getY() == animals[preyIdx]->getY()) {
                animals[predIdx]->eat();
                preyToRemove.push_back(preyIdx);
            }
        }
    }
    
    // Second pass: remove victims (in reverse order to maintain indices)
    std::sort(preyToRemove.begin(), preyToRemove.end(), std::greater<size_t>());
    for(size_t idx : preyToRemove) {
        animals.erase(animals.begin() + idx);
    }
}

    void aging() {
        for(auto& animal : animals) {
            animal->increaseAge();
        }
    }

    void reproduction() {
        std::vector<std::unique_ptr<Animal>> newAnimals;
        for(const auto& animal : animals) {
            if(animal->canReproduce()) {
                newAnimals.emplace_back(animal->reproduce());
            }
        }
        for(auto& newAnimal : newAnimals) {
            animals.push_back(std::move(newAnimal));
        }
    }

    void extinction() {
        animals.erase(
            std::remove_if(animals.begin(), animals.end(),
                [](const auto& animal) { return animal->isDead(); }),
            animals.end()
        );
    }

public:
    GameSimulation(int w, int h) : width(w), height(h) {}

    void addAnimal(Animal* animal) {
        animals.emplace_back(animal);
    }

    void simulateStep() {
		cout<<"m\n";
        moveAnimals();
		cout<<"pp\n";
        processPredation();
		cout<<"a\n";
        aging();
		cout<<"r\n";
        reproduction();
		cout<<"e\n";
        extinction();
    }
	
	void printField() const {
std::vector<std::vector<int>> field(height, std::vector<int>(width, 0));

// Count animals
for(const auto& animal : animals) {
int x = animal->getX();
int y = animal->getY();


field[y][x] += (animal->isPredator() ? -1 : 1);
}

// Print field
for(const auto& row : field) {
for(int cell : row) {
if(cell == 0) {
std::cout << "* ";
} else {
std::cout << (cell > 0 ? "+" : "\0\b") << cell;
}
}
std::cout << '\n';
}
}
	
};


int main() {
    std::cout << "Choose mode:\n";
    std::cout << "1. Manual input\n";
    std::cout << "2. Preset scenario\n";
    int choice;
    std::cin >> choice;

    std::istringstream presetInput(PRESET_SCENARIO);
    std::istream& input = (choice == 2) ? presetInput : std::cin;

    int N, M, T;
    input >> N >> M >> T;

    int R, W;
    input >> R >> W;

    GameSimulation game(N, M);

    for(int i = 0; i < R; i++) {
        int x, y, d, k;
        input >> x >> y >> d >> k;
        game.addAnimal(new Prey(x, y, static_cast<Direction>(d), k));
    }

    for(int i = 0; i < W; i++) {
        int x, y, d, k;
        input >> x >> y >> d >> k;
        game.addAnimal(new Predator(x, y, static_cast<Direction>(d), k));
    }
        std::cout << "\nStep " << 0 << ":\n";
        game.printField();

    for(int i = 0; i < T; i++) {
        std::cout << "\nStep " << i + 1 << ":\n";
        game.simulateStep();
        game.printField();
    }

    return 0;
}
