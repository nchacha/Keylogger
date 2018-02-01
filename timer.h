#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <thread>
#include <chrono>

//Timer.h deals with time!!
class Timer
{
    //Threads are new to C++
    //this will allow us to run asynchronous code execution
    //without blocking the main thread in main.cpp (the while loop)
    std::thread Thread;
    bool Alive = false; //will check if timer is running
    long CallNumber = -1L; //counts how many times we would like to run a function (ie: send mail funciton should be called 5x)
                            //L stands for long so the number doesn't become an int
    long repeat_count = -1L; //counts how many times a certain function has been called (ie: send mail function has been called 2x so far)
    std::chrono::milliseconds interval = std::chrono::milliseconds(0);

    //this is also new to c++ 11: std::function is a new type that can contain functions.
    //this part says: void(void) it takes a function that takes in nothing and returns nothing
    //nullptr is also new to c++ 11; before we would use zero but that would conflict with stuff
    //so now we use nullptr which means this function is not pointing to anything
    std::function<void(void)> funct = nullptr;

    void SleepAndRun()
    {
        std::this_thread::sleep_for(interval);
        if(Alive)
        {
            Function()();  //double parentheses: 1st pair calls the function to return a function
                            //second pair calls the function the 1st function returns
        }
    }

    void ThreadFunc()
    {
        if(CallNumber == Infinite)
        {
            while(Alive)
            {
                SleepAndRun();
            }
        }
        else
        {
            while(repeat_count--)
            {
                SleepAndRun();
            }

        }
    }

public:
    static const long Infinite = -1L;

    //constructors
    Timer(){}

    Timer(const std::function<void(void)> &f) : funct(f) {}

    Timer(const std::function<void(void)> &f,
          const unsigned long &i,
          const long repeat = Timer::Infinite) : funct(f),
                                                interval(std::chrono::milliseconds(i)),
                                                CallNumber(repeat) {}

    //starts the timer
    void Start(bool Async = true)
    {
        if(IsAlive())
        {
            return;
        }
        Alive = true;
        repeat_count = CallNumber;
        if(Async)
        {
            //this is creating the Thread we initialized above
            //ThreadFunc is the function we created
            //it is being called to run on this thread
            //[this] is the parameter needed for the ThreadFunc function
            Thread = std::thread(ThreadFunc, this);
        }
        else
        {
            //call the ThreadFunc()
            this->ThreadFunc();

        }
    }

    //manually stops the timer
    void Stop()
    {
        Alive = false;
        //joining means the invoked Thread will finish execution before joining the main thread
        //so basically stop the timer
        Thread.join();

    }

    //sets the function to be executed
    void SetFunction(const std::function<void(void)> &f)
    {
        funct = f;
    }

    //is the timer running or not?
    bool IsAlive() const {return Alive;}

    //sets how many times it is going to repeat
    void RepeatCount(const long r)
    {
        if(Alive)
        {
            return;
        }
        CallNumber = r;
    }

    //returns how many iterations are left
    long GetLeftCount() const {return repeat_count;}

    //returns how many iterations we had originally requested
    long RepeatCount() const {return CallNumber;}

    void SetInterval(const unsigned long &i)
    {
        if(Alive)
        {
            return;
        }
        interval = std::chrono::milliseconds(i);
    }

    //.count() converts from chrono milliseconds to long
    unsigned long Interval() const {return interval.count();}


    const std::function<void(void)> &Function() const
    {
        return funct;
    }


};

#endif // TIMER_H_INCLUDED
