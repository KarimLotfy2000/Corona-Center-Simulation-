#include <iostream>
#include <exception>
#include <string.h>
#include <string>
using namespace std;

enum Vaccine {
	astra = 0,
	biontech = 1,
	moderna = 2,
	none = 3
};


ostream& operator << (ostream& out, Vaccine &v) {

	if (v == astra) {
		out << "AstraZeneca";
	}
	else if (v == biontech) {
		out << "BioNTech";
	}
	else if (v == moderna) {
		out << "Moderna";
	}
	else{
		out << "none";
	}

	return out;
}


class SimError : public exception{

private:


public:
	string reason;
	SimError(string reason) {
		this->reason = reason;
	}

};


class Patient {

private:
	static int no;
	const int id;
	bool briefed;
	Vaccine vaccine;

public:

	Patient();
	int get_id();
	bool is_briefed();
	void switch_briefed();
	void set_vaccinated(Vaccine vaccine);
	friend ostream& operator << (ostream & out, Patient& p);

};


Patient::Patient():id(no++) {
	briefed = false;
	vaccine = none;
}

int Patient::get_id() {
	return this->id;
}


bool Patient::is_briefed() {
	return this->briefed;
}


void Patient::switch_briefed() {
	this->briefed = !briefed;
}


void Patient::set_vaccinated(Vaccine vaccine) {

	this->vaccine = vaccine;
}
ostream& operator << (ostream& out, Patient& p) {
	out << "patient " << p.id;

	if (p.vaccine != none) {
		out << " vaccinated with ";
		out << p.vaccine;
	}

	if (p.briefed) {
		out << " briefed" << endl;
	}
	return out;
}
/*ostream& operator<<(ostream& out,Patient& pat)
{
    out<< "patient " << get_id() << "vaccinated with " << set_vaccinated(vacType) << " briefed" <<endl;
}*/

class Station {

protected:
	string station;
	Patient* patient;

public:

	Station(string station, Patient* patient) {
		this->station = station;
		this->patient = patient;
	}


	virtual void enter(Patient* patient) = 0;

	virtual Patient* leave()throw(SimError) = 0;

	virtual void print() = 0;

	virtual ~Station() {
		if (patient != NULL)
			delete []patient;
		patient = NULL;
	}
};






class WaitingArea: public Station {
private:
	class Waiting {
	public:
		Patient* patient;
		Waiting* next;
		Waiting(Patient* p) {
			patient = p;
			next = NULL;
		}
	};
	int count;
	Waiting* waiting;
public:

	WaitingArea(string description):Station(description, NULL){

		waiting = NULL;
		count = 0;

	}

	virtual void enter(Patient * p) {
		Waiting* w = new Waiting(p);
		if (waiting == NULL) {
			waiting = w;
		}
		else {
			Waiting* temp = waiting;
			while (temp->next != NULL) {
				temp = temp->next;
			}
			temp->next = NULL;
		}
		count++;
	}

	virtual Patient* leave() throw(SimError) {

		if (waiting == NULL) {
			throw SimError("no patient waiting in " + station);
		}
		else {
			Waiting* toReturn = waiting;
			waiting = waiting->next;
			toReturn->next = NULL;
			count--;

			Patient* pToRet = toReturn->patient;

			toReturn->patient = NULL;
			delete toReturn;


			return pToRet;
		}

	}


	virtual void print() {
		cout << this->station << " ";
		cout << this->count << " ";
		cout << "waiting patients: ";
		Waiting* temp = waiting;

		while (temp != NULL) {
			cout << temp->patient->get_id() << endl;
			temp = temp->next;
		}
		cout << endl;

	}

};



class Single: public Station {

private:
	int total;
public:
	Single(string de):Station(de, NULL) {
		total = 0;
	}

	int get_total() {
		return total;
	}

	virtual void enter(Patient* p) {

		p->switch_briefed();
		this->patient = p;

		total++;



	}

	virtual Patient* leave(){
		cout << *this->patient << " " << " leaves " << this->station << endl;
		return this->patient;
	}



};


class Administrative: public Single {

public:
	Administrative(string d):Single(d) {

	}

	virtual void print() {
		cout << this->station << " " << get_total() << " patients processed ";
		cout << endl;
	}


};

class Medical : public Single {

private:
	string doctor;

public:
	Medical(string desc, string doc) : Single(desc) {
		this->doctor = doc;
	}


	virtual void enter(Patient* p) {
		if (!p->is_briefed()) {
			throw SimError("not briefed patient can not be vaccinated");
		}
		else {
			p->set_vaccinated(astra);
			Single::enter(p);
		}

	}

	virtual Patient* leave() throw(SimError) {
		return Single::leave();


	}

	virtual void print() {
		cout << this->station << " " << get_total();
		cout << " patients vaccinated by " << this->doctor;
		cout << endl;
	}

};


class Center {
private:
	Station* station[5];

public:
	Center(){
		station[0] =  Administrative("Registration and Briefing");
		station[1] = WaitingArea("Wait for Vaccination");
		station[2] =  Medical("Vaccination", "Mustafa Elsersy");
		station[3] =  WaitingArea("Wait after Vaccination");
		station[4] =  Administrative("Debriefing");
	}

	void brief(Patient* p) {
		station[0]->enter(p);
		station[0]->leave();
		station[1]->enter(p);
	}

	void vaccinate() {
		Patient *p = station[1]->leave();
		station[2]->enter(p);
		Patient* p1 = station[2]->leave();
		station[3]->enter(p1);
	}

	void debrief() {
		Patient* p = station[3]->leave();
		station[4]->enter(p);
		station[4]->leave();
		delete p;
	}

	void print() {
		cout << "LINE DATA" << endl;
		for (int i = 0; i < 5; i++) {
			station[i]->print();
		}
	}


};

//Initialized to matriculation number
int Patient::no = 3093543;

int main() {

	Center c;

	char choice = '0';

	do {
		try {
			c.print();
			cout << "(a) end" << endl;
			cout << "(b) new arrival at center" << endl;
			cout << "(c) next vaccination" << endl;
			cout << "(d) next debriefing" << endl;

			cin >> choice;
			if (choice == 'b') {
				c.brief(new Patient());
			}
			else if (choice == 'c') {
				c.vaccinate();
			}
			else if (choice == 'd') {
				c.debrief();
			}

		}
		catch (SimError e) {
			cout << "simulation error : " << e.reason << endl;
		}
	} while (choice != 'a');


	return 0;
}
