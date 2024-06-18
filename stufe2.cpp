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

public:
    Schiff(int huelle, int groesse, int schaden, int startX, int startY)
        : huellenwert(huelle), groessenwert(groesse), schadenswert(schaden), x(startX), y(startY) {}

    virtual ~Schiff() {}

    virtual std::string getSpezial() const = 0;
    virtual std::string getTyp() const = 0;

    int getHuellenwert() const { return huellenwert; }
    int getGroessenwert() const { return groessenwert; }
    int getSchadenswert() const { return schadenswert; }

    void erleideSchaden(int schaden) { huellenwert -= schaden; }

    bool istZerstoert() const { return huellenwert <= 0; }

    void bewegen(int newX, int newY) {
        x = newX;
        y = newY;
    }

//calculates the hypotenus between 2 ships to attack each other
    double distanzZu(const Schiff& anderes) const {
        return std::sqrt(std::pow(x - anderes.x, 2) + std::pow(y - anderes.y + 10, 2));
    }

    int getX() const { return x; }
    int getY() const { return y; }

    virtual int trefferProbeModifikation() const { return 0; }
    virtual void spezialEffekt(int wuerfelergebnis, Schiff& ziel) const {}
};

// Abgeleitete Klassen
class Jaeger : public Schiff {
public:
    Jaeger(int startX, int startY) : Schiff(75, 4, 30, startX, startY) {}

    std::string getSpezial() const override { return "Kritische Treffer"; }
    std::string getTyp() const override { return "Jaeger"; }

    void spezialEffekt(int wuerfelergebnis, Schiff& ziel) const override {
        if (wuerfelergebnis >= 9) {
            ziel.erleideSchaden(getSchadenswert()*2);
            std::cout << "Kritischer Treffer! Doppelschaden verursacht.\n";
        }
    }
};

class Zerstoerer : public Schiff {
public:
    Zerstoerer(int startX, int startY) : Schiff(150, 6, 60, startX, startY) {}

    std::string getSpezial() const override { return "Zielsuchend"; }
    std::string getTyp() const override { return "Zerstoerer"; }

    int trefferProbeModifikation() const override { return +2; }
};

class Kreuzer : public Schiff {
public:
    Kreuzer(int startX, int startY) : Schiff(250, 8, 50, startX, startY) {}

    std::string getSpezial() const override { return "Bombardement"; }
    std::string getTyp() const override { return "Kreuzer"; }

    void spezialEffekt(int wuerfelergebnis, Schiff& ziel) const override {
        while (wuerfelergebnis >= ziel.getGroessenwert()) {
            ziel.erleideSchaden(getSchadenswert());
            std::cout << "Bombardement! Ein weiterer Angriff wird ausgefuehrt.\n";
            if (ziel.istZerstoert()) break;
            wuerfelergebnis = rand() % 10 + 1;
        }
    }
};

// Flotte
class Flotte {
private:
    std::vector<Schiff*> schiffe;
    std::string name;
    static const int gridSize = 10;

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

    void angriffAuf(Flotte& gegner) {
        Schiff* angreifer = waehleSchiff();
        Schiff* ziel = gegner.waehleSchiff();

        std::cout << name << " Schiff " << angreifer->getTyp() << " greift " << gegner.name << " Schiff " << ziel->getTyp() << " an.\n";

        double distanz = angreifer->distanzZu(*ziel);
        int wuerfelergebnis = rand() % 10 + 1;
        int modifikation = angreifer->trefferProbeModifikation();
        if (wuerfelergebnis + modifikation >= ziel->getGroessenwert()) {
            // Adjust damage based on distance
            int schaden = static_cast<int>(angreifer->getSchadenswert() * std::max(0.1, 1.0 - distanz / 10));
            ziel->erleideSchaden(schaden);
            std::cout << "Angriff erfolgreich! Schaden: " << schaden << " bei Distanz: " << distanz << "\n";
            angreifer->spezialEffekt(wuerfelergebnis, *ziel);

            if (ziel->istZerstoert()) {
                std::cout << "Das Ziel wurde zerstoert!\n";
                gegner.entferneZerstoerteSchiffe();
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
                      << ", Huelle: " << schiff->getHuellenwert()
                      << ", Position: (" << schiff->getX() << ", " << schiff->getY() << ")\n";
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
                throw std::runtime_error("Ungueltige Eingabe.");
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
    std::cout << "Waehlen Sie die Anzahl der Schiffe (1 bis 9): ";
    int anzahlSchiffe1 = validierteEingabe(1, 9);
    for (int i = 0; i < anzahlSchiffe1; ++i) {
        std::cout << "Waehlen Sie ein Schiff (1: Jaeger, 2: Zerstoerer, 3: Kreuzer): ";
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
    std::cout << "Waehlen Sie die Anzahl der Schiffe (1 bis 9): ";
    int anzahlSchiffe2 = validierteEingabe(1, 9);
    for (int i = 0; i < anzahlSchiffe2; ++i) {
        std::cout << "Waehlen Sie ein Schiff (1: Jaeger, 2: Zerstoerer, 3: Kreuzer): ";
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
        // Flotte 1 moves
        for (int i = 0; i < flotte1.getAnzahlSchiffe(); ++i) {
            int newX = (flotte1.getSchiff(i)->getX() + 1) % 10;
            int newY = (flotte1.getSchiff(i)->getY() + 1) % 10;
            flotte1.bewegenSchiff(i, newX, newY);
        }
        std::cout << "Flotte 1 nach der Bewegung:\n";
        flotte1.zeigeSpielwelt();

        // Flotte 2 moves
        for (int i = 0; i < flotte2.getAnzahlSchiffe(); ++i) {
            int newX = (flotte2.getSchiff(i)->getX() - 1 + 10) % 10;
            int newY = (flotte2.getSchiff(i)->getY() - 1 + 10) % 10;
            flotte2.bewegenSchiff(i, newX, newY);
        }
        std::cout << "Flotte 2 nach der Bewegung:\n";
        flotte2.zeigeSpielwelt();

        // Flotte 1 attacks Flotte 2
        std::cout << "Flotte 1 greift Flotte 2 an:\n";
        flotte1.angriffAuf(flotte2);
        flotte1.zeigeSpielwelt();
        flotte2.zeigeSpielwelt();
        flotte1.zeigeFlotte(); // Add this line to show the state of Flotte 1 after the attack
        flotte2.zeigeFlotte();

        // Check if Flotte 2 is destroyed
        if (flotte2.istVernichtet()) {
            break;
        }

        // Flotte 2 attacks Flotte 1
        std::cout << "Flotte 2 greift Flotte 1 an:\n";
        flotte2.angriffAuf(flotte1);
        flotte1.zeigeSpielwelt();
        flotte2.zeigeSpielwelt();
        flotte1.zeigeFlotte(); // Add this line to show the state of Flotte 1 after the attack
        flotte2.zeigeFlotte();

        // Check if Flotte 1 is destroyed
        if (flotte1.istVernichtet()) {
            break;
        }
    }

    if (flotte1.istVernichtet()) {
        std::cout << "Flotte 2 hat gewonnen!\n";
    } else {
        std::cout << "Flotte 1 hat gewonnen!\n";
    }

    return 0;
}
