#include <iostream>
#include <conio.h>
using namespace std;

class Vrijeme
{
	int _sati;
	int _minute;
	int _sekunde;
public:
	Vrijeme(int sati = 0, int minute = 0, int sekunde = 0) {
		_sati = sati;
		_minute = minute;
		_sekunde = sekunde;
	}
	friend ostream& operator <<(ostream& output, Vrijeme& s)
	{
		output << s._sati << " " << s._minute << " " << s._sekunde;
		return output;
	}
	friend Vrijeme operator +=(Vrijeme& left, Vrijeme& right)
	{
		left._minute = left._minute + right._minute;
		left._sati = left._sati + right._sati;
		left._sekunde = left._sekunde + right._sekunde;
		return left;
	}
	friend Vrijeme operator +=(Vrijeme& x, int& right)
	{
		x._minute += right;
		return x;
	}
	Vrijeme operator ++()
	{
		Vrijeme y(_sati, _minute, _sekunde);
		_sati++;
		return y;
	}
	Vrijeme operator ++(int)
	{
		_sati++;		
	}
	bool operator >(Vrijeme& right)
	{
		if (_sati > right._sati)
			return true;
		else if (_sati == right._sati)
		{
			if (_minute > right._minute)
				return true;
			else if (_minute == right._minute)
			{
				if (_sekunde > right._sekunde)
					return true;
				else
					return false;
			}
		}
	}
};

class Let
{
	static const  int _oznakaSize = 10;
	char _oznaka[_oznakaSize];
	char* _odrediste;
	int _brIzlazneKapije;
	Vrijeme _vrijemePolijetanja; //sati i minute
	int _trajanje; //izraženo u minutama
	int _kasnjenje; //izraženo u minutama

public:
	//Potrebne konstruktor i destruktor funkcije
	Let() { }
	Let(char* oznaka, char* odrediste, Vrijeme v, int kapija, int duration) : _vrijemePolijetanja(v)
	{ 
		strcpy_s(_oznaka, _oznakaSize, oznaka);

		_odrediste = new char[strlen(odrediste) + 1];
		strcpy_s(_odrediste, strlen(odrediste) + 1, odrediste);

		_brIzlazneKapije = kapija;

		_trajanje = duration;
	}
	Let(const Let&l) {
		strcpy_s(_oznaka, _oznakaSize, l._oznaka);
		_odrediste = new char[strlen(l._odrediste) + 1];
		strcpy_s(_odrediste, strlen(l._odrediste) + 1, l._odrediste);
		_vrijemePolijetanja = l._vrijemePolijetanja;
		_brIzlazneKapije = l._brIzlazneKapije;
		_trajanje = l._trajanje;
		_kasnjenje = l._kasnjenje;
	}
	~Let() { delete[] _odrediste; }

	//Funkciju sa jednim parametrom koja postavlja informaciju o eventualnom kašnjenju na vrijednost zadanu parametrom.
	void SetLatency(int x) { _kasnjenje = x; }

	/*Preklopiti unarni operator "!" putem kojeg je moguće saznati da li odgovarajući let kasni ili ne 
	(vratiti logičku vrijednost "true" u slučaju kašnjenja, u suprotnom vratiti "false").*/
	bool operator ! () { return (_kasnjenje != 0); }

	//Funkciju koja kao rezultat vraća trajanje leta.
	int TrajanjeLeta() { return _trajanje; }

	/*Preklopiti operator + na način da omogućava sabiranje objekata tipa "Let" i cijelog broja, pri
	čemu se kao rezultat dobiva novi objekat tipa "Let", u kojem je vrijeme polijetanja pomjereno
	unaprijed za iznos određen drugim sabirkom (računato u minutama).

	Također preklopiti i operator "+=" koji osigurava da izraz oblika "X += Y uvijek ima isto značenje
	kao i izraz	"X = X + Y" */
	friend Let operator + (Let& right, float whatever)
	{
		Let x(right._oznaka, right._odrediste, right._vrijemePolijetanja, right._brIzlazneKapije, right._trajanje);
		x._kasnjenje += whatever;
		return x;
	}

	void operator += (Let right)
	{
		this->_trajanje += right._trajanje;
		this->_kasnjenje += right._kasnjenje;
		this->_vrijemePolijetanja += right._vrijemePolijetanja;
	}
	void operator += (int dur)
	{
		this->_trajanje += dur;

	}

	/* Funkciju kojom je moguće saznati očekivano vrijeme polijetanja kada se uračuna iznos kašnjenja
	u odnosu na predviđeno vrijeme polijetanja (iskoristiti preklopljeni operator +). */
	Vrijeme OcekivanoVrijemePolijetanja()
	{
		Vrijeme vrijeme(_vrijemePolijetanja);
		vrijeme += _kasnjenje;
		return vrijeme;
	}

	//Funkciju koja vraća očekivano vrijeme slijetanja


	/*Preklopiti operator "++" na način da pomijera vrijeme polaska za jedan sat unaprijed.
	Potrebno je podržati i prefiksnu i postfiksnu verziju ovog operatora.*/
	Let operator ++() { this->_vrijemePolijetanja++; return *this; }

	/*Preklopiti relacione operatore "<" i ">" koji ispituju koji let nastupa ranije, odnosno kasnije.
	Operator "<" vraća logičku vrijednost "true" ukoliko polijetanje leta sa lijeve strane nastupa
	prije polijetanje leta sa desne strane, a u suprotnom vraća logičku vrijednost "false".
	Analogno vrijedi za operator ">". Prilikom upoređivanja treba uzeti u obzir i očekivano vrijeme kašnjenja,
	a ne samo planirano vrijeme polijetanja.*/
	bool operator <(Let& right)
	{
		return !(this->OcekivanoVrijemePolijetanja() > right.OcekivanoVrijemePolijetanja());
	}

	bool operator >(Let& right)
	{
		return this->OcekivanoVrijemePolijetanja() > right.OcekivanoVrijemePolijetanja();
	}


	/*Preklopiti operator "<<" koji treba da podrži ispis objekata tipa "Let" na ekran. U slučaju da
	se radi o polijetanju bez kašnjenja, ispis bi trebao da izgleda kako slijedi:

	JFK 156 Atalanta    12:50   19:30   5

	Podaci predstavljaju redom: oznaku leta, naziv odredišta, vrijeme polijetanja, očekivano
	vrijeme slijetanja i broj izlazne kapije.

	U slučaju da se radi o letu koji kasni, ispis bi trebao izgledati kako slijedi:

	ZGK 932 Zagreb    15:50 (Planirano 15:30, Kasni 20 min)*/
	friend ostream& operator <<(ostream& output, Let& let)
	{
		output << let._oznaka << " " << let._odrediste << " " << let._vrijemePolijetanja << " " << let._brIzlazneKapije << endl;
		return output;
	}
	bool operator==(Let&l) {
		return !strcmp(_oznaka, l._oznaka) && !strcmp(this->_odrediste,l._odrediste) && this->_brIzlazneKapije == l._brIzlazneKapije;
	}bool operator==(char*oznaka) {
		return !strcmp(_oznaka, _oznaka) ;
	}
	int getTrajanje()const {
		return _trajanje;
	}
};


class RasporedLetova
{
	int _brojRegistrovanihLetova;
	const int _maksimalanBrojLetova;
	Let** _letovi;

public:
	RasporedLetova(int maksimalanBrojLetova) : _maksimalanBrojLetova(maksimalanBrojLetova),
		_letovi(new Let*[_maksimalanBrojLetova]), _brojRegistrovanihLetova(0)
	{

	}

	/*Preklopiti operator "+=" na način da registruje novi let u raspored. 
	Raspored letova u svakom momentu treba biti sortiran
	prema vremenu polijetanja. Voditi računa o maksimalnom broju letova.  
	Za potrebe poređenja vremena polaska letova preklopiti odgovarajuće operatore u klasi Vrijeme.*/

	bool operator+=(Let& l) {
		if (_maksimalanBrojLetova - 1 == _brojRegistrovanihLetova)return false;
		_letovi[_brojRegistrovanihLetova++] = new Let(l);
		Sortiraj();
		return true;
	}

	void Sortiraj() {
		for (int i = 0; i < _brojRegistrovanihLetova-1; i++)
		{
			for (int j = i+1; j < _brojRegistrovanihLetova; j++)
			{
				if (*_letovi[i] > *_letovi[j]) {
					Let*t = _letovi[i];
					_letovi[i] = _letovi[j];
					_letovi[j] = t;
				}
			}
		}
	}
	bool operator-=(Let& l) {
		if (0 == _brojRegistrovanihLetova)return false;

		for (int i = 0; i < _brojRegistrovanihLetova; i++)
		{
			if (*_letovi[i] == l) {
				delete _letovi[i];
				for (int j = i; j < _brojRegistrovanihLetova-1; j++)
				{
					_letovi[j] = _letovi[j + 1];
 				}
				_letovi[_brojRegistrovanihLetova - 1] = nullptr;
				_brojRegistrovanihLetova--;
				return true;
			}
		}
		return false;
	}
	/*Preklopiti operator "-=" na način da ukljanja registrovani let sa oznakom definisanom putem desnog operanda.
	Pri tome sačuvati redoslijed prethodno registrovanih letova. */

	//Preklopiti operator "[]" na način da vraća objekat tipa Let 
	//na osnovu proslijeđene pozicije u rasporedu (pozicije kreću od 1).
	Let operator[](int poy) {
		return *_letovi[poy - 1];
	}
	/*Preklopiti operator "()" 
	na način da letu sa definisanom oznakom pomjeri 
	vrijeme polijetanja za vrijednost drugog parametra izraženog
	u minutama. */

	void operator()(char*oznaka,int min) {
		for (int i = 0; i < _brojRegistrovanihLetova; i++)
		{
			if (*_letovi[i] == oznaka) {
				*_letovi[i] += min;
				return;
			}
		}
	}
	//Funkciju koja daje prosječno trajanje svih pohranjenih letova.
	float Prosjek() {
		float suma = 0;
		for (int i = 0; i < _brojRegistrovanihLetova; i++)
			suma += _letovi[i]->getTrajanje();

		return suma / _brojRegistrovanihLetova;

	}


	/*Preklopiti operator "<<" 
	koji ispisuje kompletan spisak svih letova, sortiran po očekivanim vremenima polazaka.
	Za potrebe poređenja vremena polaska letova preklopiti odgovarajuće operatore u klasi Vrijeme. */
	friend ostream&operator<<(ostream&cin, RasporedLetova&r) {
		cin << r._brojRegistrovanihLetova << endl;
		for (int i = 0; i < r._brojRegistrovanihLetova; i++)
		{
			cout << *r._letovi[i] << endl;
 		}
		return cin;
	}
	~RasporedLetova()
	{
		for (int i = 0; i < _brojRegistrovanihLetova; i++)
		{
			delete _letovi[i];
		}
		delete[] _letovi;
	}

};

int main()
{
	Let l1("IB1212", "Rumunija", Vrijeme(12, 12, 12), 1, 420), l2("IB1112", "Bijelu Rusiju", Vrijeme(12, 11, 11), 1, 460);
	RasporedLetova rs(5);
	rs += l1;
	rs += l2;
	cout << rs << endl;
	rs -= l1;
	cout << rs << endl;
	_getch();
	return 0;
}