#include "CSMACD.h"

void CSMACD::reproduce_sending()
{
    while (!stations.empty())
    {
        auto first_station = *stations.begin();
        auto current_station = ++stations.begin();
        std::set<int> has_collision;
        if (full_log)
        {
            std::cout << "Station " << first_station.second << " " << " start sending at " << first_station.first << "\n";
        }
        //если станция current_station начнет отправку кадра раньше, чем закончит first_station
        //то возникнет коллизия, которую станции задетектируют до конца отправки
        //и потому начнут пересылать кадр сначала
        while (current_station != stations.end() && first_station.first + SENDING_TIME > (*current_station).first)
        {
            if (full_log)
            {
                std::cout << "Station " << (*current_station).second << " " << " start sending at " << (*current_station).first << "\n";
            }
            has_collision.insert(first_station.second);
            has_collision.insert((*current_station).second);
            ++current_station;
        }
        handle_collisions(has_collision);

        if (has_collision.find(first_station.second) == has_collision.end())
        {
            std::cout << "** Station " << first_station.second << " end sending at " << first_station.first + SENDING_TIME << " mcs\n";
            stations.erase(first_station);
            times.erase(first_station.second);

            double now = first_station.first + SENDING_TIME;
            handle_end_of_frame_sending(now);
        }
    }
}

void CSMACD::handle_collisions(std::set<int> &has_collision) {
    for (auto station_index : has_collision)
    {
        if (full_log)
        {
            std::cout << station_index << " occur to collision\n";
        }
        attempt[station_index]++;
        //верхнее ограничение на количество отправок -- 16
        if (attempt[station_index] == 16)
        {
            //если они истекли, то станция перестает пытаться отправить кадр
            stations.erase({times[station_index], station_index});
            times.erase(station_index);
            std::cout << "Station " << station_index << " could not send frame";
            continue;
        }
        double new_time =  times[station_index] + backoff(attempt[station_index]);
        update_station_sending_time(station_index, new_time);
    }
}

void CSMACD::handle_end_of_frame_sending(double now) {
    for (auto station : times)
    {
        int station_index = station.first;
        double sending_time = station.second;
        //если время, когда сл станция хочет начать передачу
        //ближе ко времени конца передачи последнего кадра меньше чем межкадрового интервала,
        //то эта станция будет начинать передачу сразу после конца межкадрового интервала
        if (sending_time < now + INTERFRAME_INTERVAL)
        {
            sending_time = times[station_index] = now + INTERFRAME_INTERVAL;
            update_station_sending_time(station_index, sending_time);
        }
    }
}

double CSMACD::backoff(int iteration)
{
    double tme =  (rand() % (1 << iteration)) * SLOT_TIME;
    return tme;
}

void CSMACD::update_station_sending_time(int station_index, double new_time) {
    stations.erase({times[station_index], station_index});
    times[station_index] = new_time;
    stations.insert({times[station_index], station_index});
}
