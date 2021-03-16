#pragma once

class Weather {
 public:
    Weather();
    virtual ~Weather();
    void run();

 protected:
    bool init();

    int _fd;
};
