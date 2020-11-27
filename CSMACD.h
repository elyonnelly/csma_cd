#ifndef CSMACD_CSMACD_H
#define CSMACD_CSMACD_H
#include <iostream>
#include <map>
#include <set>
#include <time.h>

class CSMACD {

public:
    CSMACD(int n, bool full_log)
    {
        number_of_stations = n;
        for (int i = 0; i < n; i++)
        {
            times[i] = 0;
            stations.insert({times[i], i});
            this->full_log = full_log;
        }
    }

    void reproduce_sending();

private:
    const double SLOT_TIME = 51.2;
    const double SENDING_TIME = 1210.8;
    const double INTERFRAME_INTERVAL = 9.6;

    bool full_log;
    int number_of_stations;
    std::map<int, double> times;
    //мы поддерживаем станции в сортированном порядке по времени, когда каждая их них собирается отправлять кадр
    std::set<std::pair<double, int> > stations;
    std::map<int, int> attempt;

    void handle_collisions(std::set<int> &has_collision);

    void handle_end_of_frame_sending(double now);

    double backoff(int iteration);

    void update_station_sending_time(int station_index, double new_time);
};


#endif //CSMACD_CSMACD_H
