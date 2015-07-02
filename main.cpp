#include <iostream>
#include <QCoreApplication>
#include <QDateTime>
#include <QEventLoop>

#include "tptimer.h"

void startTimerBeforeEventLoop(TpTimer &timer);
void startTimerAtEventLoop(TpTimer &timer);
void startTimerAfterEventLoop(TpTimer &timer);

void behaviorOfPendingTimeouts();

void message();

int main(int argc, char *argv[])
{
  QCoreApplication app{argc, argv};

  // call timer with expiry-timeout-point lying in the past
  std::cout << "Run A) Timepoint lying 1 second in the past (fire immediately):\n"
               "===============================================================\n";
  behaviorOfPendingTimeouts(); // this runs its own eventloop


  
  std::cout << "Run B) Do slow (1 sec) and fast (0.5 sec) intervals, but without drift\n"
               "======================================================================\n";
  
  bool slow_interval = true; // if true: interval = 1 sec, else: interval = 0.5 sec

  // SETUP TIMER
  TpTimer timer;
  timer.setInterval(  (slow_interval ? 1000 : 500)  );
  timer.setSingleShot(true);
  QObject::connect(&timer, &QTimer::timeout,
                   [&]() {
                     std::cout << "millisSinceEpoch planned-alarm: " << timer.expiryTimePoint() << "\n"
                                  "actual:                         "
                               << TpTimer::nowTimePoint() << '\n' << std::endl;

                     slow_interval = ! slow_interval; // toggle interval
                     const int interval = (slow_interval ? 1000 : 500);
                     timer.startToTimePoint(timer.expiryTimePoint() + interval); });

  // START TIMER (choose one of the following 3)
  startTimerBeforeEventLoop(timer);
  //startTimerAtEventLoop(timer);
  //startTimerAfterEventLoop(timer);
  
  // SETUP PROGRAM QUIT
  QTimer::singleShot(5000, [&]() { message(); app.quit(); });
  // alternative:
  // QTimer::singleShot(5000, &app, &QCoreApplication::quit);

  return app.exec();
}

void startTimerBeforeEventLoop(TpTimer &timer)
{
  std::cout                           << "Start:                          "
                                      << TpTimer::nowTimePoint() << '\n' << std::endl;
  timer.start();
}

void startTimerAtEventLoop(TpTimer &timer)
{
  QTimer::singleShot(0,
                     [&]() {std::cout << "Start:                          "
                                      << TpTimer::nowTimePoint() << '\n' << std::endl;
                            timer.start();});
}

void startTimerAfterEventLoop(TpTimer &timer)
{
  QTimer::singleShot(1000,
                     [&]() {std::cout << "Start:                          "
                                      << TpTimer::nowTimePoint() << '\n' << std::endl;
                            timer.start();});
}

void behaviorOfPendingTimeouts()
{
  QEventLoop eventloop;
  TpTimer timer;
  timer.setPassedTimepointsTrigger(true); /* allow startToTimePoint(tp) to take timepoints lying in the past
                                             - these will cause the timer to fire immediately */
  
  timer.startToTimePoint(TpTimer::nowTimePoint()-1000); // timepoint in the past
  QObject::connect(&timer, &QTimer::timeout,
                   [&]() {                     
                     std::cout << "millisSinceEpoch planned-alarm: " << timer.expiryTimePoint() << "\n"
                                  "actual:                         "
                               << TpTimer::nowTimePoint() << '\n' << std::endl;
                     eventloop.exit();
                   });
  eventloop.exec();
}

void message()
{
  std::cout << "Sometimes there is fairly big offsets between planned and actual, the very 1st time (for Run B). Do you also get that?" << std::endl;
}
