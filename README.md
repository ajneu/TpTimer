# TpTimer
QTimer subclassed for timepoint functionality (allowing "timing" without drift)

## The problem 
QTimer's interface is currently only interval-based.

You start a timer... specifying an interval until timeout.  
On timeout you restart the timer... with a new interval. And the restarting causes a few nanoseconds of drift.

```cpp
  QTimer timer;
  timer.setSingleShot(true);

  bool slow_interval = true;                   // if true: interval = 1 sec, else: interval = 0.5 sec
  timer.setInterval(  (slow_interval ? 1000 : 500)  );
  timer.start();
  
  QObject::connect(&timer, &QTimer::timeout,
                   [&]() {
                     std::cout << "timepoint: " << QDateTime::currentMSecsSinceEpoch() << std::endl;

                     slow_interval = ! slow_interval; // toggle interval
                     const int interval = (slow_interval ? 1000 : 500);
                     timer.start(interval);           // timer will eventually drift !!!!!!!!!!!!!
                     });
```

## A possible solution
Subclass QTimer to extend its interface to allow the use a timepoint-based interface.

Start the timer... specifying the timepoint-of-timeout (as milliseconds since Epoch [1 Jan 1970]).  
On timeout you restart the timer... with a new definite timpoint-of-timeout. There is no drift between these timepoints, since they are absolute.

```cpp
  class TpTimer : public QTimer {
    //...
  };
  
  
  TpTimer timer;
  timer.setSingleShot(true);

  bool slow_interval = true;                   // if true: interval = 1 sec, else: interval = 0.5 sec
  timer.setInterval(  (slow_interval ? 1000 : 500)  );

  QObject::connect(&timer, &QTimer::timeout,
                   [&]() {
                     std::cout << "millisSinceEpoch planned-alarm: " << timer.expiryTimePoint() << "\n"
                                  "actual:                         "
                               << TpTimer::nowTimePoint() << '\n' << std::endl;

                     slow_interval = ! slow_interval; // toggle interval
                     const int interval = (slow_interval ? 1000 : 500);
                     timer.startToTimePoint(timer.expiryTimePoint() + interval);  // no drift !!!
                     });

```
