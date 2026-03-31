// Implementation for ACMOJ Problem 1421 - ACM EXPRESS
// Only uses iostream, algorithm, and base.hpp as required.

#include <iostream>
#include <algorithm>
#include "base.hpp"

using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}

protected:
    // Convert to absolute day count under 360-day year, 30-day month rule
    int to_days() const {
        return year * 360 + (month - 1) * 30 + (day - 1);
    }

public:
    // Stream input operator: "<year> <month> <day>"
    friend istream &operator>>(istream &is, date &dt) {
        return (is >> dt.year >> dt.month >> dt.day);
    }

    // Compare dates: earlier means smaller
    bool operator<(const date &other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    // Difference in days (this - other) using simplified calendar
    int diff_days(const date &other) const {
        return to_days() - other.to_days();
    }
};

class mail : public object {
protected:
    string postmark;
    date send_date;
    date arrive_date;

public:
    mail() = default;

    mail(string _contain_, string _postmark_, date send_d, date arrive_d)
        : object(_contain_), postmark(_postmark_), send_date(send_d), arrive_date(arrive_d) {}

    // Override virtuals
    virtual string send_status(int, int, int) override {
        return string("not send");
    }

    virtual string type() override { return string("no type"); }

    virtual void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << endl;
    }

    // Provided example style
    virtual void copy(object *o) override {
        contain = reinterpret_cast<mail *>(o)->contain;
        postmark = reinterpret_cast<mail *>(o)->postmark;
        send_date = reinterpret_cast<mail *>(o)->send_date;
        arrive_date = reinterpret_cast<mail *>(o)->arrive_date;
    }

    virtual ~mail() {}
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date;
    date land_date;

public:
    air_mail() = default;

    air_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
             date take_off, date land, string _airline)
        : mail(_contain_, _postmark_, send_d, arrive_d), airlines(_airline),
          take_off_date(take_off), land_date(land) {}

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        else if (ask_date < take_off_date)
            return "wait in airport";
        else if (ask_date < land_date)
            return "in flight";
        else if (ask_date < arrive_date)
            return "already land";
        else
            return "already arrive";
    }

    virtual string type() override { return string("air"); }

    virtual void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << endl;
    }

    virtual void copy(object *o) override {
        air_mail *ao = reinterpret_cast<air_mail *>(o);
        contain = ao->contain;
        postmark = ao->postmark;
        send_date = ao->send_date;
        arrive_date = ao->arrive_date;
        airlines = ao->airlines;
        take_off_date = ao->take_off_date;
        land_date = ao->land_date;
    }

    virtual ~air_mail() {}
};

class train_mail : public mail {
protected:
    string *station_name = nullptr;
    date *station_time = nullptr;
    int len = 0;

public:
    train_mail() = default;

    train_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
               string *sname, date *stime, int station_num)
        : mail(_contain_, _postmark_, send_d, arrive_d), len(station_num) {
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = sname[i];
                station_time[i] = stime[i];
            }
        }
    }

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date) return "mail not send";
        // Before first station
        if (len == 0) {
            if (ask_date < arrive_date) return "in train";
            return "already arrive";
        }

        if (ask_date < station_time[0]) return "wait in station";

        // Iterate through stations
        for (int i = 0; i < len; ++i) {
            if (!(station_time[i] < ask_date) && !(ask_date < station_time[i])) {
                // ask_date == station_time[i]
                return "at station";
            }
            if (ask_date < station_time[i]) {
                // Between previous station_time and this station_time
                return "in train";
            }
        }
        // After last station time but before arrive
        if (ask_date < arrive_date) return "wait in station";
        return "already arrive";
    }

    virtual string type() override { return string("train"); }

    virtual void print() override {
        mail::print();
        cout << "[train] station_num: " << len << endl;
    }

    virtual void copy(object *o) override {
        train_mail *to = reinterpret_cast<train_mail *>(o);
        contain = to->contain;
        postmark = to->postmark;
        send_date = to->send_date;
        arrive_date = to->arrive_date;

        // Deep copy arrays
        len = to->len;
        // Clean existing
        if (station_name) {
            delete[] station_name;
            station_name = nullptr;
        }
        if (station_time) {
            delete[] station_time;
            station_time = nullptr;
        }
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = to->station_name[i];
                station_time[i] = to->station_time[i];
            }
        }
    }

    virtual ~train_mail() {
        if (station_name) delete[] station_name;
        if (station_time) delete[] station_time;
        station_name = nullptr;
        station_time = nullptr;
        len = 0;
    }
};

class car_mail : public mail {
protected:
    int total_mile = 0;
    string driver;

public:
    car_mail() = default;

    car_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
             int mile, string _driver)
        : mail(_contain_, _postmark_, send_d, arrive_d), total_mile(mile),
          driver(_driver) {}

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date) return "mail not send";
        if (!(arrive_date < ask_date) && !(ask_date < arrive_date)) {
            // ask_date == arrive_date
            return "already arrive";
        }
        if (arrive_date < ask_date) return "already arrive";

        // Progress between send_date and arrive_date
        double used = (double)ask_date.diff_days(send_date);
        double total = (double)arrive_date.diff_days(send_date);
        if (total <= 0.0) {
            return to_string(0.0);
        }
        double current_mile = (used / total) * (double)total_mile;
        return to_string(current_mile);
    }

    virtual string type() override { return string("car"); }

    virtual void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << endl;
    }

    virtual void copy(object *o) override {
        car_mail *co = reinterpret_cast<car_mail *>(o);
        contain = co->contain;
        postmark = co->postmark;
        send_date = co->send_date;
        arrive_date = co->arrive_date;
        total_mile = co->total_mile;
        driver = co->driver;
    }

    virtual ~car_mail() {}
};

void obj_swap(object *&lhs, object *&rhs) {
    object *tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}

