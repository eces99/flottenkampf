#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <sstream>

// Basisklasse Schiff
class Schiff {
protected:
    int huellenwert;
    int groessenwert;
    int schadenswert;
    int x, y; // Position in der 2D-Welt
    int erfahrungspunkte;
    int level;
    bool evasion; // For Jaeger: To evade the next attack
    int bonusSchaden; // For Zerstoerer: To increase damage for the next attack


    //koncstruktor für basisklasse schiff
public:
    Schiff(int huelle, int groesse, int schaden, int startX, int startY)
        : huellenwert(huelle), groessenwert(groesse), schadenswert(schaden), x(startX), y(startY), erfahrungspunkte(0), level(1), evasion(false), bonusSchaden(0) {}

// virtual destructor, allowing for proper cleanup of derived class objects when deleted through a base class pointer.
    virtual ~Schiff() {}

    virtual std::string getSpezial() const = 0;     //returns the special ability of the ship as string to display
    virtual std::string getTyp() const = 0;   //returns the type of the ship

    //getter functions to access the private and protected member variables
    int getHuellenwert() const { return huellenwert; }
    int getGroessenwert() const { return groessenwert; }
    int getSchadenswert() const { return schadenswert + bonusSchaden; }
    int getErfahrungspunkte() const { return erfahrungspunkte; }
    int getLevel() const { return level; }

    //funktion verringert den Hüllenwert um den Schadenswert
    void erleideSchaden(int schaden) {
        if (evasion) {  //nur in stufe 3, wenn evasion true dann keine schaden aus hüllen abgezogen
            std::cout << getTyp() << " hat dem Angriff ausgewichen!\n";
            evasion = false;
        } else {
            huellenwert -= schaden; //gleich wie andere stufen
        }
    }

    //checks ob hüllenwert kleiner gleich 0 bzw. sit der schiffe zerstört ist
    bool istZerstoert() const { return huellenwert <= 0; }

    //aktualisiert die neue koordinaten von shiffe
    void bewegen(int newX, int newY) {
        x = newX;
        y = newY;
    }

    //berechnet hypotenus un je nachdem die schaden unterschiedlich effektiv
    double distanzZu(const Schiff& anderes) const {
        return std::sqrt(std::pow(x - anderes.x, 2) + std::pow(y - anderes.y + 10 , 2));
    }

    //getter funktionen für die koordinaten
    int getX() const { return x; }
    int getY() const { return y; }

    //pure virtual funktionen
    virtual int trefferProbeModifikation() const { return 0; }  //returns a modification value for the hit test
    virtual void spezialEffekt(int wuerfelergebnis, Schiff& ziel) {}    //applies a special effect based on a dice roll and the target ship

    //function adds experience points and checks if the ship should level up
    void addErfahrung(int xp) {
        erfahrungspunkte += xp;
        if (erfahrungspunkte >= 100 * level) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        erfahrungspunkte = 0;
        std::cout << getTyp() << " hat Level " << level << " erreicht und neue Fähigkeiten freigeschaltet!\n";
    }

    virtual void faehigkeit1(Schiff& ziel) {}
    virtual void faehigkeit2() {}
};

// Abgeleitete Klassen
class Jaeger : public Schiff {
public:
    Jaeger(int startX, int startY) : Schiff(75, 4, 30, startX, startY) {}

    std::string getSpezial() const override { return "Kritische Treffer"; }
    std::string getTyp() const override { return "Jaeger"; }

    void spezialEffekt(int wuerfelergebnis, Schiff& ziel) override {
        if (wuerfelergebnis >= 9) {
            ziel.erleideSchaden(getSchadenswert()*2);
            std::cout << "Kritischer Treffer! Doppelschaden verursacht.\n";
        }
    }

    //quick attack that deals half damage without roll the dice
    void faehigkeit1(Schiff& ziel) override {
        std::cout << "Jaeger verwendet Schneller Angriff!\n";
        ziel.erleideSchaden(getSchadenswert() / 2);
    }

    void faehigkeit2() override {
        std::cout << "Jaeger verwendet Ausweichen!\n";
        evasion = true; // Enable evasion for the next attack
    }
};

class Zerstoerer : public Schiff {
public:
    Zerstoerer(int startX, int startY) : Schiff(150, 6, 60, startX, startY) {}

    std::string getSpezial() const override { return "Zielsuchend"; }
    std::string getTyp() const override { return "Zerstoerer"; }

    //Zielsuchend: Trefferproben sind um 2 erleichtert
    int trefferProbeModifikation() const override { return +2; }

    void faehigkeit1(Schiff& ziel) override {
        std::cout << "Zerstoerer verwendet Raketensalve!\n";
        ziel.erleideSchaden(getSchadenswert() / 2);
        ziel.erleideSchaden(getSchadenswert() / 2);
    }

     void faehigkeit2() override {
        std::cout << "Zerstoerer verwendet Schadensverstärkung!\n";
        bonusSchaden += 20; // Increase damage for the next attack
    }
};

class Kreuzer : public Schiff {
public:
    Kreuzer(int startX, int startY) : Schiff(250, 8, 50, startX, startY) {}

    std::string getSpezial() const override { return "Bombardement"; }
    std::string getTyp() const override { return "Kreuzer"; }

    void spezialEffekt(int wuerfelergebnis, Schiff& ziel) override {
        while (wuerfelergebnis >= ziel.getGroessenwert()) {
            ziel.erleideSchaden(getSchadenswert());
            std::cout << "Bombardement! Ein weiterer Angriff wird ausgeführt.\n";
            if (ziel.istZerstoert()) break;
            wuerfelergebnis = rand() % 10 + 1;
        }
    }

    void faehigkeit1(Schiff& ziel) override {
        std::cout << "Kreuzer verwendet Schwere Kanonen!\n";
        ziel.erleideSchaden(getSchadenswert() * 1.5);
    }

    void faehigkeit2() override {
        std::cout << "Kreuzer verwendet Reparaturdrohnen!\n";
        huellenwert += 50; // Repair some hull damage
        if (huellenwert > 250) { // Ensure hull value doesn't exceed the maximum
            huellenwert = 250;
        }
    }
};

// Flotte
class Flotte {
private:
    std::vector<Schiff*> schiffe;
    std::string name;
    static const int gridSize = 10;

// constructor for the Flotte class, initializing the fleet with a given name
public:
    Flotte(const std::string& n) : name(n) {}

    ~Flotte() {
        for (auto schiff : schiffe) {
            delete schiff;
        }
    }

    void hinzufuegen(Schiff* schiff) {
        if (schiffe.size() < 9) {
            schiffe.push_back(schiff);
        } else {
            throw std::runtime_error("Maximale Anzahl von Schiffen erreicht!");
        }
    }

    bool istVernichtet() const {
        return schiffe.empty();
    }

    void entferneZerstoerteSchiffe() {
        schiffe.erase(
            std::remove_if(schiffe.begin(), schiffe.end(),
                           [](Schiff* schiff) { return schiff->istZerstoert(); }),
            schiffe.end());
    }

    Schiff* waehleSchiff() {
        if (schiffe.empty()) {
            throw std::runtime_error("Keine Schiffe in der Flotte!");
        }
        return schiffe[rand() % schiffe.size()];
    }

    //in main rundenweise beide flotten explizit aufgerufen
    void angriffAuf(Flotte& gegner) {
        Schiff* angreifer = waehleSchiff();
        Schiff* ziel = gegner.waehleSchiff();

        std::cout << name << " Schiff " << angreifer->getTyp() << " greift " << gegner.name << " Schiff " << ziel->getTyp() << " an.\n";

        double distanz = angreifer->distanzZu(*ziel);
        int wuerfelergebnis = rand() % 10 + 1;
        int modifikation = angreifer->trefferProbeModifikation();
        if (wuerfelergebnis + modifikation >= ziel->getGroessenwert()) {
            // Adjust damage based on distance
            int schaden = static_cast<int>(angreifer->getSchadenswert() * std::max(0.1, 1.0 - distanz / 10));   //even at the maximum distance, the damage will be at least 10% of the base damage
            ziel->erleideSchaden(schaden);
            std::cout << "Angriff erfolgreich! Schaden: " << schaden << " bei Distanz: " << distanz << "\n";
            angreifer->spezialEffekt(wuerfelergebnis, *ziel);

            if (ziel->istZerstoert()) {
                std::cout << "Das Ziel wurde zerstört!\n";
                angreifer->addErfahrung(50); // Gain XP for destroying a ship
                gegner.entferneZerstoerteSchiffe();
            } else {
                angreifer->addErfahrung(10); // Gain XP for successful hit
            }
        } else {
            std::cout << "Angriff verfehlt!\n";
        }
    }

    void bewegenSchiff(int index, int newX, int newY) {
        if (index < 0 || index >= schiffe.size()) {
            throw std::out_of_range("Invalid index: " + std::to_string(index));
        }
        if (newX < 0 || newX >= gridSize || newY < 0 || newY >= gridSize) {
            throw std::invalid_argument("Out of bounds: (" + std::to_string(newX) + ", " + std::to_string(newY) + ")");
        }
        schiffe[index]->bewegen(newX, newY);
    }


    void zeigeFlotte() const {
        std::cout << "Flotte " << name << ":\n";
        for (const auto& schiff : schiffe) {
            std::cout << "Schiff: " << schiff->getTyp()
                      << ", Hülle: " << schiff->getHuellenwert()
                      << ", Position: (" << schiff->getX() << ", " << schiff->getY() << ")"
                      << ", Level: " << schiff->getLevel()
                      << ", Erfahrungspunkte: " << schiff->getErfahrungspunkte() << "\n";
        }
    }

    void zeigeSpielwelt() const {
        std::vector<std::vector<char>> spielwelt(gridSize, std::vector<char>(gridSize, '.'));

        for (const auto& schiff : schiffe) {
            int x = schiff->getX();
            int y = schiff->getY();
            spielwelt[y][x] = 'S';
        }

        for (const auto& row : spielwelt) {
            for (const auto& cell : row) {
                std::cout << cell << ' ';
            }
            std::cout << '\n';
        }
    }

    int getAnzahlSchiffe() const {
        return schiffe.size();
    }

    Schiff* getSchiff(int index) const {
        if (index >= 0 && index < schiffe.size()) {
            return schiffe[index];
        }
        return nullptr;
    }
};

// Eingabevalidierung
int validierteEingabe(int min, int max) {
    int eingabe;
    std::string input;

    while (true) {
        try {
            std::cout << "Bitte geben Sie eine Zahl zwischen " << min << " und " << max << " ein: ";
            std::getline(std::cin, input);

            // Use stringstream to parse the input as an integer
            std::stringstream ss(input);
            if (ss >> eingabe && !(ss >> input) && eingabe >= min && eingabe <= max) {
                return eingabe; // Valid input, return the value
            } else {
                throw std::runtime_error("Ungültige Eingabe.");
            }
        } catch (const std::runtime_error& e) {
            std::cout << e.what() << " Bitte versuchen Sie es erneut.\n";
        }
    }
}

// Hauptfunktion
int main() {
    srand(time(0));

    Flotte flotte1("Flotte 1"), flotte2("Flotte 2");

    std::cout << "Flotte 1 zusammenstellen:\n";
    std::cout << "Wählen Sie die Anzahl der Schiffe (1 bis 9): ";
    int anzahlSchiffe1 = validierteEingabe(1, 9);
    for (int i = 0; i < anzahlSchiffe1; ++i) {
        std::cout << "Wählen Sie ein Schiff (1: Jaeger, 2: Zerstoerer, 3: Kreuzer): ";
        int wahl = validierteEingabe(1, 3);
        int startX = rand() % 10;
        int startY = rand() % 10;
        switch (wahl) {
        case 1:
            flotte1.hinzufuegen(new Jaeger(startX, startY));
            break;
        case 2:
            flotte1.hinzufuegen(new Zerstoerer(startX, startY));
            break;
        case 3:
            flotte1.hinzufuegen(new Kreuzer(startX, startY));
            break;
        }
    }

    std::cout << "Flotte 2 zusammenstellen:\n";
    std::cout << "Wählen Sie die Anzahl der Schiffe (1 bis 9): ";
    int anzahlSchiffe2 = validierteEingabe(1, 9);
    for (int i = 0; i < anzahlSchiffe2; ++i) {
        std::cout << "Wählen Sie ein Schiff (1: Jaeger, 2: Zerstoerer, 3: Kreuzer): ";
        int wahl = validierteEingabe(1, 3);
        int startX = rand() % 10;
        int startY = rand() % 10;
        switch (wahl) {
        case 1:
            flotte2.hinzufuegen(new Jaeger(startX, startY));
            break;
        case 2:
            flotte2.hinzufuegen(new Zerstoerer(startX, startY));
            break;
        case 3:
            flotte2.hinzufuegen(new Kreuzer(startX, startY));
            break;
        }
    }

    std::cout << "Kampf beginnt!\n";
    while (!flotte1.istVernichtet() && !flotte2.istVernichtet()) {
        std::cout << "Flotte 1 bewegt sich:\n";
        for (int i = 0; i < flotte1.getAnzahlSchiffe(); ++i) {
            int newX = (flotte1.getSchiff(i)->getX() + 1) % 10;
            int newY = (flotte1.getSchiff(i)->getY() + 1) % 10;
            flotte1.bewegenSchiff(i, newX, newY);
        }
        flotte1.zeigeSpielwelt();
        flotte1.zeigeFlotte();

        std::cout << "Flotte 2 bewegt sich:\n";
        for (int i = 0; i < flotte2.getAnzahlSchiffe(); ++i) {
            int newX = (flotte2.getSchiff(i)->getX() - 1 + 10) % 10;
            int newY = (flotte2.getSchiff(i)->getY() - 1 + 10) % 10;
            flotte2.bewegenSchiff(i, newX, newY);
        }
        flotte2.zeigeSpielwelt();
        flotte2.zeigeFlotte();

        std::cout << "Flotte 1 greift Flotte 2 an:\n";
        flotte1.angriffAuf(flotte2);
        flotte1.zeigeSpielwelt();
        flotte2.zeigeSpielwelt();
        flotte1.zeigeFlotte(); // Add this line to show the state of Flotte 1 after the attack
        flotte2.zeigeFlotte();

        if (!flotte2.istVernichtet()) {
            std::cout << "Flotte 2 greift Flotte 1 an:\n";
            flotte2.angriffAuf(flotte1);
            flotte1.zeigeSpielwelt();
            flotte2.zeigeSpielwelt();
            flotte1.zeigeFlotte(); // Add this line to show the state of Flotte 1 after the attack
            flotte2.zeigeFlotte();
        }
    }

    if (flotte1.istVernichtet()) {
        std::cout << "Flotte 2 hat gewonnen!\n";
    } else {
        std::cout << "Flotte 1 hat gewonnen!\n";
    }

    return 0;
}
