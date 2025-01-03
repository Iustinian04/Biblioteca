#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <exception>
#include <stdexcept> // For standard exceptions

using namespace std;

// ------------------- EXCEPȚII -------------------
class ImprumutException : public runtime_error {
public:
    explicit ImprumutException(const string& mesaj) : runtime_error(mesaj) {}
};

// ------------------- CLASE ȘI IERARHII -------------------

// Penalitate (interfață abstractă)
class Penalitate {
public:
    virtual double calculeazaPenalitate(int zileIntarziere) const = 0;
    virtual ~Penalitate() = default;
};

// Clasa de bază: Carte
class Carte : public Penalitate {
protected:
    string titlu;
    string autor;
    int anPublicare;

public:
    Carte() : titlu(""), autor(""), anPublicare(0) {}

    Carte(const string& titlu, const string& autor, int anPublicare)
        : titlu(titlu), autor(autor), anPublicare(anPublicare) {}

    Carte(const Carte& other) : titlu(other.titlu), autor(other.autor), anPublicare(other.anPublicare) {}

    Carte& operator=(const Carte& other) {
        if (this != &other) {
            titlu = other.titlu;
            autor = other.autor;
            anPublicare = other.anPublicare;
        }
        return *this;
    }

    virtual void afisare() const {
        cout << "Titlu: " << titlu << ", Autor: " << autor << ", An publicare: " << anPublicare << endl;
    }

    [[nodiscard]] string getTitlu() const { return titlu; }

    friend ostream& operator<<(ostream& os, const Carte& carte) {
        os << "Titlu: " << carte.titlu << ", Autor: " << carte.autor << ", An publicare: " << carte.anPublicare;
        return os;
    }

    friend istream& operator>>(istream& is, Carte& carte) {
        cout << "Introduceti titlul: ";
        is >> carte.titlu;
        cout << "Introduceti autorul: ";
        is >> carte.autor;
        cout << "Introduceti anul publicarii: ";
        is >> carte.anPublicare;
        return is;
    }

    bool operator==(const Carte& other) const {
        return titlu == other.titlu && autor == other.autor;
    }

    ~Carte() override = default;

    double calculeazaPenalitate(int zileIntarziere) const override {
        return zileIntarziere * 0.5; // Penalitate standard
    }
};

// Clasă derivată: CarteFizica
class CarteFizica : public Carte {
private:
    int numarPagini;
    string stareFizica; // "buna", "uzata"

public:
    CarteFizica(const string& titlu, const string& autor, int anPublicare, int numarPagini, const string& stareFizica)
        : Carte(titlu, autor, anPublicare), numarPagini(numarPagini), stareFizica(stareFizica) {}

    void afisare() const override {
        Carte::afisare();
        cout << "Numar pagini: " << numarPagini << ", Stare fizica: " << stareFizica << endl;
    }

    double calculeazaPenalitate(int zileIntarziere) const override {
        double penalitate = zileIntarziere * 1.0;
        if (stareFizica == "uzata") {
            penalitate += 10;
        }
        return penalitate;
    }
};

// Clasă derivată: CarteDigitala
class CarteDigitala : public Carte {
private:
    float dimensiuneFisier; // MB
    string format;          // PDF, EPUB

public:
    CarteDigitala(const string& titlu, const string& autor, int anPublicare, float dimensiuneFisier, const string& format)
        : Carte(titlu, autor, anPublicare), dimensiuneFisier(dimensiuneFisier), format(format) {}

    void afisare() const override {
        Carte::afisare();
        cout << "Dimensiune fisier: " << dimensiuneFisier << " MB, Format: " << format << endl;
    }

    double calculeazaPenalitate(int zileIntarziere) const override {
        return 5.0; // Penalitate fixă pentru cărțile digitale
    }
};

// Clasă abstractă: Utilizator
class Utilizator {
protected:
    string nume;
    string email;
    string tipUtilizator;

    static unordered_map<string, shared_ptr<Utilizator>> registruUtilizatori;

public:
    Utilizator(const string& nume, const string& email, const string& tipUtilizator)
        : nume(nume), email(email), tipUtilizator(tipUtilizator) {}

    virtual void afisare() const {
        cout << "Nume: " << nume << ", Email: " << email << ", Tip utilizator: " << tipUtilizator << endl;
    }

    string getEmail() const { return email; }

    virtual int limitaImprumuturi() const = 0;

    static shared_ptr<Utilizator> cautaUtilizator(const string& email) {
        if (registruUtilizatori.find(email) != registruUtilizatori.end()) {
            return registruUtilizatori[email];
        }
        return nullptr;
    }

    virtual ~Utilizator() = default;
};

unordered_map<string, shared_ptr<Utilizator>> Utilizator::registruUtilizatori;

// Clasă derivată: Student
class Student : public Utilizator {
private:
    string facultate;

public:
    Student(const string& nume, const string& email, const string& facultate)
        : Utilizator(nume, email, "Student"), facultate(facultate) {
        registruUtilizatori[email] = make_shared<Student>(*this);
    }

    void afisare() const override {
        Utilizator::afisare();
        cout << "Facultate: " << facultate << endl;
    }

    int limitaImprumuturi() const override {
        return 5;
    }
};

// Clasă derivată: Profesor
class Profesor : public Utilizator {
private:
    string departament;

public:
    Profesor(const string& nume, const string& email, const string& departament)
        : Utilizator(nume, email, "Profesor"), departament(departament) {
        registruUtilizatori[email] = make_shared<Profesor>(*this);
    }

    void afisare() const override {
        Utilizator::afisare();
        cout << "Departament: " << departament << endl;
    }

    int limitaImprumuturi() const override {
        return 10;
    }
};

// Clasă abstractă: ImprumutAbstract
class ImprumutAbstract {
protected:
    int idImprumut;
    string dataImprumut;
    string dataReturnare;

    static int contorID;
    static int numarTotalImprumuturi;

public:
    ImprumutAbstract(int id, const string& imprumut, const string& returnare)
        : idImprumut(id), dataImprumut(imprumut), dataReturnare(returnare) {
        ++numarTotalImprumuturi;
    }

    static int genereazaID() {
        return ++contorID;
    }

    static int getNumarTotalImprumuturi() {
        return numarTotalImprumuturi;
    }

    virtual double calculeazaPenalitate(int zileIntarziere) const = 0;

    virtual ~ImprumutAbstract() = default;
};

int ImprumutAbstract::contorID = 0;
int ImprumutAbstract::numarTotalImprumuturi = 0;

// Clasă derivată: ImprumutCarteFizica
class ImprumutCarteFizica : public ImprumutAbstract {
private:
    CarteFizica carte;
    Utilizator& utilizator;

public:
    ImprumutCarteFizica(const string& imprumut, const string& returnare, const CarteFizica& carte, Utilizator& utilizator)
        : ImprumutAbstract(genereazaID(), imprumut, returnare), carte(carte), utilizator(utilizator) {}

    double calculeazaPenalitate(int zileIntarziere) const override {
        return carte.calculeazaPenalitate(zileIntarziere);
    }

    void afisare() const {
        cout << "ID Imprumut: " << idImprumut << ", Data imprumut: " << dataImprumut
             << ", Data returnare: " << dataReturnare << endl;
        cout << "Carte: ";
        carte.afisare();
        cout << "Utilizator: ";
        utilizator.afisare();
    }
};

// Clasă derivată: ImprumutCarteDigitala
class ImprumutCarteDigitala : public ImprumutAbstract {
private:
    CarteDigitala carte;
    Utilizator& utilizator;

public:
    ImprumutCarteDigitala(const string& imprumut, const string& returnare, const CarteDigitala& carte, Utilizator& utilizator)
        : ImprumutAbstract(genereazaID(), imprumut, returnare), carte(carte), utilizator(utilizator) {}

    double calculeazaPenalitate(int zileIntarziere) const override {
        return carte.calculeazaPenalitate(zileIntarziere);
    }

    void afisare() const {
        cout << "ID Imprumut: " << idImprumut << ", Data imprumut: " << dataImprumut
             << ", Data returnare: " << dataReturnare << endl;
        cout << "Carte: ";
        carte.afisare();
        cout << "Utilizator: ";
        utilizator.afisare();
    }
};

// ------------------- TEMPLATE ȘI STL -------------------
template <typename T>
class Biblioteca {
private:
    vector<T> colectie;

public:
    void adauga(const T& element) {
        colectie.push_back(element);
    }

    const vector<T>& getCarti() const {
    return colectie;
    }

    void afisare() const {
        for (const auto& elem : colectie) {
            elem->afisare(); // Dereferențierea pointerului inteligent
            cout << endl;
        }
    }

    void sorteaza() {
        sort(colectie.begin(), colectie.end(), [](const T& a, const T& b) {
            return a->getTitlu() < b->getTitlu(); // Dereferențierea pointerului inteligent
        });
    }
};


// Design Pattern: Singleton for Library
class BibliotecaSingleton {
private:
    BibliotecaSingleton() {}
    Biblioteca<shared_ptr<Carte>> biblioteca;

public:
    BibliotecaSingleton(const BibliotecaSingleton&) = delete;
    BibliotecaSingleton& operator=(const BibliotecaSingleton&) = delete;

    static BibliotecaSingleton& getInstance() {
        static BibliotecaSingleton instance;
        return instance;
    }

    const vector<shared_ptr<Carte>>& getCarti() const {
    return biblioteca.getCarti();
    }

    void adaugaCarte(const shared_ptr<Carte>& carte) {
        biblioteca.adauga(carte);
    }

    void afisareCarti() {
        biblioteca.afisare();
    }

    void sorteazaCarti() {
        biblioteca.sorteaza();
    }
};

// Design Pattern: Factory for creating users
class UtilizatorFactory {
public:
    static shared_ptr<Utilizator> creareUtilizator(const string& tip, const string& nume, const string& email, const string& facultateDepartament) {
        if (tip == "Student") {
            return make_shared<Student>(nume, email, facultateDepartament);
        } else if (tip == "Profesor") {
            return make_shared<Profesor>(nume, email, facultateDepartament);
        }
        throw ImprumutException("Tip de utilizator necunoscut!");
    }
};

// ------------------- MENIU INTERACTIV -------------------
void afiseazaMeniu() {
    cout << "\n=== MENIU INTERACTIV ===\n";
    cout << "1. Adauga utilizator\n";
    cout << "2. Adauga carte\n";
    cout << "3. Creaza imprumut\n";
    cout << "4. Afiseaza utilizatori\n";
    cout << "5. Afiseaza carti\n";
    cout << "6. Afiseaza numar total de imprumuturi\n";
    cout << "0. Iesire\n";
    cout << "Alege o optiune: ";
}

int main() {
    try {
        auto& biblioteca = BibliotecaSingleton::getInstance();
        vector<shared_ptr<Utilizator>> utilizatori;
        vector<shared_ptr<ImprumutAbstract>> imprumuturi;

        int optiune = -1;

        while (optiune != 0) {
            afiseazaMeniu();
            cin >> optiune;
            cin.ignore(); // Curăță bufferul pentru intrări de tip string

            switch (optiune) {
                case 1: {
                    cout << "Tip utilizator (Student/Profesor): ";
                    string tipUtilizator;
                    getline(cin, tipUtilizator);

                    cout << "Nume: ";
                    string nume;
                    getline(cin, nume);

                    cout << "Email: ";
                    string email;
                    getline(cin, email);

                    cout << (tipUtilizator == "Student" ? "Facultate" : "Departament") << ": ";
                    string facultateDepartament;
                    getline(cin, facultateDepartament);

                    try {
                        auto utilizator = UtilizatorFactory::creareUtilizator(tipUtilizator, nume, email, facultateDepartament);
                        utilizatori.push_back(utilizator);
                        cout << "Utilizator adaugat cu succes!\n";
                    } catch (const ImprumutException& ex) {
                        cout << "Eroare: " << ex.what() << endl;
                    }
                    break;
                }
                case 2: {
                    cout << "Tip carte (Fizica/Digitala): ";
                    string tipCarte;
                    getline(cin, tipCarte);

                    cout << "Titlu: ";
                    string titlu;
                    getline(cin, titlu);

                    cout << "Autor: ";
                    string autor;
                    getline(cin, autor);

                    cout << "An publicare: ";
                    int anPublicare;
                    cin >> anPublicare;
                    cin.ignore();

                    if (tipCarte == "Fizica") {
                        cout << "Numar pagini: ";
                        int numarPagini;
                        cin >> numarPagini;
                        cin.ignore();

                        cout << "Stare fizica (buna/uzata): ";
                        string stareFizica;
                        getline(cin, stareFizica);

                        auto carte = make_shared<CarteFizica>(titlu, autor, anPublicare, numarPagini, stareFizica);
                        biblioteca.adaugaCarte(carte);
                    } else if (tipCarte == "Digitala") {
                        cout << "Dimensiune fisier (MB): ";
                        float dimensiuneFisier;
                        cin >> dimensiuneFisier;
                        cin.ignore();

                        cout << "Format (PDF/EPUB): ";
                        string format;
                        getline(cin, format);

                        auto carte = make_shared<CarteDigitala>(titlu, autor, anPublicare, dimensiuneFisier, format);
                        biblioteca.adaugaCarte(carte);
                    } else {
                        cout << "Tip carte necunoscut!\n";
                    }

                    cout << "Carte adaugata cu succes!\n";
                    break;
                }
                case 3: {
                    cout << "Email utilizator: ";
                    string email;
                    getline(cin, email);

                    auto utilizator = Utilizator::cautaUtilizator(email);
                    if (!utilizator) {
                        cout << "Utilizatorul nu a fost gasit!\n";
                        break;
                    }

                    cout << "Titlu carte imprumutata: ";
                    string titluCarte;
                    getline(cin, titluCarte);

                    cout << "Data imprumut (YYYY-MM-DD): ";
                    string dataImprumut;
                    getline(cin, dataImprumut);

                    cout << "Data returnare (YYYY-MM-DD): ";
                    string dataReturnare;
                    getline(cin, dataReturnare);

                    // Căutare carte
                    auto& bibliotecaSingleton = BibliotecaSingleton::getInstance();
                    const auto& carti = bibliotecaSingleton.getCarti(); // Obține colecția de cărți
                    bool carteGasita = false;
                    for (const auto& carte : carti) {
                        if (carte->getTitlu() == titluCarte) {
                            // Creare împrumut
                            if (auto carteFizica = dynamic_pointer_cast<CarteFizica>(carte)) {
                                imprumuturi.push_back(make_shared<ImprumutCarteFizica>(dataImprumut, dataReturnare, *carteFizica, *utilizator));
                                carteGasita = true;
                            } else if (auto carteDigitala = dynamic_pointer_cast<CarteDigitala>(carte)) {
                                imprumuturi.push_back(make_shared<ImprumutCarteDigitala>(dataImprumut, dataReturnare, *carteDigitala, *utilizator));
                                carteGasita = true;
                            }
                            break;
                        }
                    }

                    if (carteGasita) {
                        cout << "Imprumut creat cu succes!\n";
                    } else {
                        cout << "Carte nu a fost gasita!\n";
                    }
                    break;
                }
                case 4: {
                    cout << "\nUtilizatori:\n";
                    for (const auto& utilizator : utilizatori) {
                        utilizator->afisare();
                        cout << endl;
                    }
                    break;
                }
                case 5: {
                    cout << "\nCarti in biblioteca:\n";
                    biblioteca.afisareCarti();
                    break;
                }
                case 6: {
                    cout << "Numar total de imprumuturi: " << ImprumutAbstract::getNumarTotalImprumuturi() << endl;
                    break;
                }
                case 0:
                    cout << "La revedere!\n";
                    break;
                default:
                    cout << "Optiune invalida!\n";
                    break;
            }
        }
    } catch (const ImprumutException& ex) {
        cout << "Eroare: " << ex.what() << endl;
    } catch (const exception& ex) {
        cout << "Eroare: " << ex.what() << endl;
    }


    return 0;
}
