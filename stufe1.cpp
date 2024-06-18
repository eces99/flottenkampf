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

public:
    Schiff(int huelle, int groesse, int schaden)
        : huellenwert(huelle), groessenwert(groesse), schadenswert(schaden) {}

    virtual ~Schiff() {}

    virtual std::string getSpezial() const = 0;
    virtual std::string getTyp() const = 0;

    int getHuellenwert() const { return huellenwert; }
    int getGroessenwert() const { return groessenwert; }
    int getSchadenswert() const { return schadenswert; }

    void erleideSchaden(int schaden) { huellenwert -= schaden; }

    bool istZerstoert() const { return huellenwert <= 0; }

    virtual int trefferProbeModifikation() const { return 0; }
    virtual void spezialEffekt(int wuerfelergebnis, Schiff& ziel) const {}
};

// Abgeleitete Klassen
class Jaeger : public Schiff {
public:
    Jaeger() : Schiff(75, 4, 30) {}

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
    Zerstoerer() : Schiff(150, 6, 60) {}

    std::string getSpezial() const override { return "Zielsuchend"; }
    std::string getTyp() const override { return "Zerstoerer"; }

    int trefferProbeModifikation() const override { return +2; }
};

class Kreuzer : public Schiff {
public:
    Kreuzer() : Schiff(250, 8, 50) {}

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

        int wuerfelergebnis = rand() % 10 + 1;
        int modifikation = angreifer->trefferProbeModifikation();
        if (wuerfelergebnis + modifikation >= ziel->getGroessenwert()) {
            int schaden = angreifer->getSchadenswert();
            ziel->erleideSchaden(schaden);
            std::cout << "Angriff erfolgreich! Schaden: " << schaden << "\n";
            angreifer->spezialEffekt(wuerfelergebnis, *ziel);

            if (ziel->istZerstoert()) {
                std::cout << "Das Ziel wurde zerstoert!\n";
                gegner.entferneZerstoerteSchiffe();
            }
        } else {
            std::cout << "Angriff verfehlt!\n";
        }
    }

    void zeigeFlotte() const {
        std::cout << "Flotte " << name << ":\n";
        for (const auto& schiff : schiffe) {
            std::cout << "Schiff: " << schiff->getTyp()
                      << ", Huelle: " << schiff->getHuellenwert() << "\n";
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
        switch (wahl) {
        case 1:
            flotte1.hinzufuegen(new Jaeger());
            break;
        case 2:
            flotte1.hinzufuegen(new Zerstoerer());
            break;
        case 3:
            flotte1.hinzufuegen(new Kreuzer());
            break;
        }
    }

    std::cout << "Flotte 2 zusammenstellen:\n";
    std::cout << "Waehlen Sie die Anzahl der Schiffe (1 bis 9): ";
    int anzahlSchiffe2 = validierteEingabe(1, 9);
    for (int i = 0; i < anzahlSchiffe2; ++i) {
        std::cout << "Waehlen Sie ein Schiff (1: Jaeger, 2: Zerstoerer, 3: Kreuzer): ";
        int wahl = validierteEingabe(1, 3);
        switch (wahl) {
        case 1:
            flotte2.hinzufuegen(new Jaeger());
            break;
        case 2:
            flotte2.hinzufuegen(new Zerstoerer());
            break;
        case 3:
            flotte2.hinzufuegen(new Kreuzer());
            break;
        }
    }

    std::cout << "Kampf beginnt!\n";
    while (!flotte1.istVernichtet() && !flotte2.istVernichtet()) {
        flotte1.zeigeFlotte();
        flotte2.zeigeFlotte();

        flotte1.angriffAuf(flotte2);
        flotte1.zeigeFlotte();
        flotte2.zeigeFlotte();
        if (flotte2.istVernichtet()) {
            break;
        }

        flotte2.angriffAuf(flotte1);
        flotte1.zeigeFlotte();
        flotte2.zeigeFlotte();
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
