#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;
string implicenty[256];
int zmienne[256];
int zmiennestart[256];
int n = 0;
ofstream wynik;
std::stringstream ss;
bool czydopliku = false;
bool czyPrzerwac = true;

void wypiszStany(bool czyDoPliku = false){
    for(int i = 0; i < 256; i++){
        if(zmienne[i] != -1){
            char znak = i;
            cout << znak << "=" << zmienne[i] << "; ";
            if(czyDoPliku) wynik << znak << "=" << zmienne[i] << "; ";
        }
    }
    cout << endl;
}

bool sprawdz(int j, bool *czyZadowolony){
    string text = implicenty[j];
    bool czyNegacja = false;
    for(int i = 0; i < text.size(); i++){
        if(text[i]=='|'){
            czyZadowolony[j] = true;
            return true;
        }
        if( ('A' <= text[i] && text[i] <= 'Z') || ('a' <= text[i] && text[i] <= 'z')){
            if( (zmienne[text[i]] == 0 && !czyNegacja) || (zmienne[text[i]] == 1 && czyNegacja) ){
                //cout << text[i] << "=" << zmienne[text[i]] << endl;
                bool czyReturnFalse = true;
                while(++i<text.size()){
                    if(text[i]=='|'){
                        czyReturnFalse = false;
                        break;
                    }
                }
                if(czyReturnFalse){
                    czyZadowolony[j] = false;
                    return false;
                }
            }
            czyNegacja = false;
        }else if (text[i] == '~' || text[i] == '!') czyNegacja = true;
    }
    czyZadowolony[j] = true;
    return true;
}

bool sprawdzWszystkie(bool *czyZadowolony){
    for(int j = 0; j<n ; j++){
        if(!sprawdz(j,czyZadowolony)) return false;
    }
    return true;
}

int main()
{
    cout.setf(ios::fixed);
    clock_t start, koniec;
    double roznica;
    srand( time( NULL ) );
    wynik.open("wynik.txt");
    for(int i = 0; i < 256; i++){
        zmienne[i] = -1;
        zmiennestart[i] = -1;
    }
    bool czyWygenerowac = true;
    int ileZmiennych = 0;
    int maxZmiennych = 0;
    bool czyUproszczony = false;
    bool czyWalkSAT = true;
    double maxtime = 0;

    cout << "Program rozwiazuje problem spelnialnosci dla wprowadzonej formuly logicznej." << endl;
    while(1){
        cout << "Wybierz algorytm: " << endl;
        cout << "1.Prosty WalkSAT - wybiera losowy implicent o wartosci \"0\", a nastepnie neguje losowa w nim zmienna" << endl;
        cout << "2.Ulepszony WalkSAT - wybiera losowy implicent o wartosci \"0\", a nastepnie neguje te zmienna w nim, ktora zamieni najmniej implicentow na wartosc \"0\"" << endl;
        cout << "3.GSAT - wybiera te zmienna, ktorej negacja minimalizuje liczbe implicentow o wartosci \"0\"" << endl;
        cout << "4.Uruchom i porownaj oba - WalkSAT oraz GSAT." << endl;
        cin >> n;
        if(n==1) czyUproszczony = true;
        if(n==3) czyWalkSAT = false;
        if(n==4) czyPrzerwac = false;

        cout << "Czy zapisac wyniki do pliku? 1 - tak 2 - nie" << endl;
        cin >> n;
        if(n==1) czydopliku = true;
        n = 0;

        cout << "Skad wziac dane? 1-wygeneruj losowe 2-pozwol mi podac" << endl;
        cin >> n;
        if(n==2) czyWygenerowac = false;

        cout << "Podaj ilosc implicentow: ";
        cin >> n;
        if(!(n>0)) continue;

        if(czyWygenerowac){
            cout << "Podaj ilosc dostepnych przy generacji zmiennych: ";
            cin >> ileZmiennych;
            if(0>=ileZmiennych || ileZmiennych>26){
                cout << "Podales nieakceptowalna liczbe zmiennych" << endl;
                continue;
            }

            cout << "Podaj maksymalna ilosc zmiennych w jednym implicencie: ";
            cin >> maxZmiennych;
            if(0>=maxZmiennych || maxZmiennych>ileZmiennych){
                cout << "Podales nieakceptowalna liczbe zmiennych" << endl;
                continue;
            }
        }
        cout << "Podaj maksymalny czas wyszukiwania (w sekundach): ";
        cin >> maxtime;
        if(0>=maxtime) continue;

        break;
    }

    bool czyZadowolony[n];
    for(int i = 0; i<n; i++) czyZadowolony[i] = false;
    vector<char> elementy[n];

    if(!czyWygenerowac){
        for(int j = 0; j<n ; j++){
            cout << "Podaj " << j+1 << " implicent: ";
            cin >> implicenty[j];
            string text = implicenty[j];
            for(int i = 0; i < text.size(); i++){
                if( ('A' <= text[i] && text[i] <= 'Z') || ('a' <= text[i] && text[i] <= 'z')){
                    zmienne[text[i]] = ( std::rand() % 2 );
                    elementy[j].push_back(text[i]);
                }
            }
        }
        for(int i = 0; i < 256; i++){
            if(zmienne[i] != -1) ileZmiennych++;
        }
    }else{
        bool czyBylJednoelementowy = false;
        for(int j = 0; j<n ; j++){
            int iloscWtym = (std::rand() % maxZmiennych)+1;
            if(iloscWtym == 1){
                while(iloscWtym==1 && czyBylJednoelementowy){
                    iloscWtym = (std::rand() % maxZmiennych)+1;
                }
                czyBylJednoelementowy = true;
            }

            ss.str( std::string() ); ss.clear();
            for(int i = 0; i<iloscWtym; i++){
                char znak = 'A'+(std::rand() % ileZmiennych);
                if(!elementy[j].empty()){
                    while(std::find(elementy[j].begin(), elementy[j].end(), znak) != elementy[j].end()) znak = 'A'+(std::rand() % ileZmiennych);
                    ss << "|";
                }
                zmienne[znak] = ( std::rand() % 2 );
                elementy[j].push_back(znak);
                if(std::rand() % 2)
                    ss << znak;
                else
                    ss << "~" << znak;
            }
            implicenty[j] = ss.str();
        }
    }

    cout << "Objasnienie symboli: ~ oznacza negacje, | oznacza operator logiczny OR, & oznacza operator logiczny AND." << endl;
    if(czyWygenerowac) cout << "Wygenerowana funkcja (formula): f("; else cout << "Wczytana funkcja (formula): f(";
    bool czyPrzecinek = false;
    for(int i = 0; i < 256; i++)
        if(zmienne[i] != -1){
            char znak = i;
            if(!czyPrzecinek){
                czyPrzecinek = true;
                cout << znak;
            }else cout << "," << znak;
        }
    cout << ")=(" << implicenty[0] << ") ";
    for(int i=1; i<n; i++) cout << "& (" << implicenty[i] << ") ";
    cout << endl;
    system("pause");
    cout << endl;

    if(!czyPrzerwac){
        for(int i = 0; i < 256; i++){
            //zmiennestart[i] = zmienne[i];
            if(zmienne[i] != -1){
                zmiennestart[i] = 0;
                zmienne[i] = 0;
            }
        }
    }

    if(czyPrzerwac) cout << "Losowo przypisane poczatkowe wartosci: " << endl;
    else cout << "Dla lepszego porowniania, kazdy z algorytmow rozpoczynam ze zmiennymi ustawionymi na 0: " << endl;
    wypiszStany();
    cout << endl;

    start=clock();
    int wyl = -1;
    int ktoryy = 0;
    int iloscinteracji = 0;
    do{
        if(czyPrzerwac == false && ktoryy == 0){
            czyWalkSAT = true;
            czyUproszczony = true;
        }else if (ktoryy == 1){
            czyWalkSAT = true;
            czyUproszczony = false;
        }else if(ktoryy == 2){
            czyWalkSAT = false;
        }
        wyl = -1;
        iloscinteracji == 0;
        //start=clock();
    while( !sprawdzWszystkie(czyZadowolony) ){
        koniec=clock();//zapisanie konca mierzenia
        roznica=(koniec-start)/(double)CLOCKS_PER_SEC;
        if(roznica>maxtime){
            cout << "Przekroczono maksymalny czas wykonywania programu." << endl;
            cout << "Czy chcesz kontynuowac wyszukiwanie dla innego przyporzadkowania poczatkowego?" << endl;
            system("pause");
            for(int i = 0; i < 256; i++){
                if(zmienne[i] != -1) zmienne[i] = ( std::rand() % 2 );
            }
            cout << "Losowo przypisane poczatkowe wartosci: " << endl;
            wypiszStany();
            cout << endl;
            system("pause");
            start=clock();
        }
        /*bool czyAllZadowolone = true;
        for(int i = 0; i<n; i++){
            if(czyZadowolony[i]==false) czyAllZadowolone = false;
        }
        if(czyAllZadowolone){
            cout << "brejkuje tym czyAll.." << endl;
            break;
        }*/
        if(czyWalkSAT){
            while(1){
                wyl = std::rand() % n;
                if(czyZadowolony[wyl] == false) break;
            }
            if(czyPrzerwac) cout << "Wylosowalem implicent numer " << wyl+1 << endl;
            if(!czyUproszczony && elementy[wyl].size() != 1 && std::rand() % 12){
                int iloscElementow = elementy[wyl].size();
                int ileZepsuje[iloscElementow];
                for(int i = 0; i<iloscElementow; i++) ileZepsuje[i] = 0;
                for(int i = 0; i<iloscElementow; i++){
                    char zmienna = elementy[wyl][i];
                    bool czyZadowolony2[n];
                    for(int k = 0; k<n; k++) czyZadowolony2[k] = czyZadowolony[k];
                    int war = zmienne[zmienna];
                    if(war == 1) zmienne[zmienna] = 0; else zmienne[zmienna] = 1;
                    sprawdzWszystkie(czyZadowolony);
                    for(int k = 0; k<n; k++){
                        if(czyZadowolony2[k] == true){
                            if(czyZadowolony[k] == false){
                                ileZepsuje[i]++;
                            }
                        }
                    }
                    war = zmienne[zmienna];
                    if(war == 1) zmienne[zmienna] = 0; else zmienne[zmienna] = 1;
                }
                int najlepszy = 9999;
                int najID = -1;
                if(czyPrzerwac) cout << "Sprawdzam negacja ktorej zmiennej \"zepsuje\" najmniej gotowych implicentow:" << endl;
                for(int i = 0; i<iloscElementow; i++){
                    if(czyPrzerwac) cout << elementy[wyl][i] << " zepsuje " << ileZepsuje[i] << "; ";
                    if(ileZepsuje[i]<najlepszy){
                        najlepszy=ileZepsuje[i];
                        najID = i;
                    }else if(ileZepsuje[i]==najlepszy){
                        if( (std::rand() % 4) == 0 ) najID = i;
                    }
                }
                if(czyPrzerwac) cout << endl;
                char zmienna = elementy[wyl][najID];
                int war = zmienne[zmienna];
                if(war == 1) zmienne[zmienna] = 0; else zmienne[zmienna] = 1;
                if(czyPrzerwac) cout << "Neguje zmienna " << zmienna << endl; //<< "(" << najID << "/" << elementy[wyl].size() << ")" << endl;
                if(czyPrzerwac) wypiszStany();
            }else{
                int element = std::rand() % elementy[wyl].size();
                char zmienna = elementy[wyl][element];
                int war = zmienne[zmienna];
                if(war == 1) zmienne[zmienna] = 0; else zmienne[zmienna] = 1;
                if(elementy[wyl].size() != 1 && !czyUproszczony && czyPrzerwac) cout << "Losowo wybieram zmienna do zanegowania: (wybieranie co jakis czas losowej wartosci daje szanse algorytmowi na wyjscie z nieprawidlowego przyporzadkowania - \"blednej petli\")" << endl;
                if(czyPrzerwac) cout << "Neguje zmienna " << zmienna << endl; //<< "(" << element << "/" << elementy[wyl].size() << ")" << endl;
                if(czyPrzerwac) wypiszStany();
            }
        }else{
            //GSAT
            if(std::rand() % 10){
                //int iloscElementow = elementy[wyl].size();
                int ileZepsutych[ileZmiennych];
                for(int i = 0; i<ileZmiennych; i++) ileZepsutych[i] = 0;
                int licznik = -1;
                for(int i = 0; i<256; i++){
                    if(zmienne[i] == -1) continue;
                    licznik++;
                    //char zmienna = elementy[wyl][i];
                    //bool czyZadowolony2[n];
                    //for(int k = 0; k<n; k++) czyZadowolony2[k] = czyZadowolony[k];
                    int war = zmienne[i];
                    if(war == 1) zmienne[i] = 0; else zmienne[i] = 1;
                    sprawdzWszystkie(czyZadowolony);
                    for(int k = 0; k<n; k++){
                        if(czyZadowolony[k] == false){
                            ileZepsutych[licznik]++;
                        }
                    }
                    war = zmienne[i];
                    if(war == 1) zmienne[i] = 0; else zmienne[i] = 1;
                }
                int najlepszy = 9999;
                int najID = -1;
                //cout << "Sprawdzam negacja ktorej zmiennej \"zepsuje\" najmniej gotowych implicentow:" << endl;
                for(int i = 0; i<ileZmiennych; i++){
                    //cout << elementy[wyl][i] << " zepsuje " << ileZepsutych[i] << "; ";
                    if(ileZepsutych[i]<najlepszy){
                        najlepszy=ileZepsutych[i];
                        najID = i;
                    }else if(ileZepsutych[i]==najlepszy){
                        if( (std::rand() % 4) == 0 ) najID = i;
                    }
                }
                if(czyPrzerwac) cout << endl;
                char zmienna;
                licznik = -1;
                for(int i = 0; i<256; i++){
                    if(zmienne[i] == -1) continue;
                    if(++licznik==najID){
                        zmienna = i;
                        break;
                    }
                }
                int war = zmienne[zmienna];
                if(war == 1) zmienne[zmienna] = 0; else zmienne[zmienna] = 1;
                if(czyPrzerwac) cout << "Neguje zmienna " << zmienna << endl; //<< "(" << najID << "/" << elementy[wyl].size() << ")" << endl;
                if(czyPrzerwac) wypiszStany();
            }else{
                int wyl = std::rand() % ileZmiennych;
                char zmienna;
                int licznik = -1;
                for(int i = 0; i<256; i++){
                    if(zmienne[i] == -1) continue;
                    if(++licznik==wyl){
                        zmienna = i;
                        break;
                    }
                }
                int war = zmienne[zmienna];
                if(war == 1) zmienne[zmienna] = 0; else zmienne[zmienna] = 1;
                if(czyPrzerwac) cout << "Neguje zmienna " << zmienna << endl; //<< "(" << najID << "/" << elementy[wyl].size() << ")" << endl;
                if(czyPrzerwac) wypiszStany();
            }
        }
    if(czyPrzerwac) cout << endl;
    iloscinteracji++;
    }
        //koniec=clock();//zapisanie konca mierzenia
        //roznica=(koniec-start)/(double)CLOCKS_PER_SEC;
        if(czyPrzerwac == false && ktoryy == 0){
            cout << "Wynik dla WalkSAT: " << "(znaleziony w " << iloscinteracji << " krokach)" << endl; //<< "(znaleziony w czasie: " << roznica << " sec. w " <<
            if(czydopliku) wynik<< "Wynik dla prostego WalkSAT: " << "(znaleziony w " << iloscinteracji << " krokach)" << endl;
        }/*else if (ktoryy == 1){
            cout << "Wynik dla ulepszonego WalkSAT: " << "(znaleziony w " << iloscinteracji << " krokach)" << endl;
            if(czydopliku) wynik<< "Wynik dla ulepszonego WalkSAT: " << "(znaleziony w " << iloscinteracji << " krokach)" << endl;
        }*/else if(ktoryy == 2){
            //czyPrzerwac = true;
            cout << "Wynik dla GSAT: " << "(znaleziony w " << iloscinteracji << " krokach)" << endl;
            if(czydopliku) wynik<< "Wynik dla GSAT: " << "(znaleziony w " << iloscinteracji << " krokach)" << endl;
        }
        wypiszStany(czydopliku);
        if(ktoryy >= 2) break;
        ktoryy++;
        if(ktoryy == 1) ktoryy++; //ominiecie "ulepszony WalkSAT"
        if(ktoryy==1 || ktoryy==2){
            for(int i = 0; i < 256; i++){
                zmienne[i] = zmiennestart[i];
            }
        }
        //cout << endl;

    }while(!czyPrzerwac);
    cout << endl;
    if(czyPrzerwac){
        cout << "Koniec. Uzyskane spelniajace formule przyporzadkowanie:" << endl;
        wypiszStany(czydopliku);
        cout << endl;
        koniec=clock();//zapisanie konca mierzenia
        roznica=(koniec-start)/(double)CLOCKS_PER_SEC;//obliczenie roznicy, czyli czasu wykonania
        cout<<"Czas wykonania: "<<roznica<<" (sekundy) "<<endl;
        if(czydopliku) wynik<<"Czas wykonania: "<<roznica<<" (sekundy) "<<endl;
    }



    wynik.close();
    system("pause");
    return 0;
}
