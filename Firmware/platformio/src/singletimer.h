#ifndef SINGLETIMER_H
#define SINGLETIMER_H


class SingleTimer
{
public:
    SingleTimer(){}
    void start(unsigned long timeout);
    bool hasRunout();
private:
    unsigned long ulStarttime;
    unsigned long ulTimeout;
    bool bRunning;
};

#endif // SINGLETIMER_H
