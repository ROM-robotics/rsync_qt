#ifndef ROM_TIME_H
#define ROM_TIME_H

namespace rom_dynamics::data_types
{
class RomTime
{
public:
    RomTime();

    RomTime(int sec, unsigned int nsec);

    int seconds() const;
    unsigned int nanoseconds() const;

    // Convert to milliseconds
    long toMilliseconds() const;
    // Convert to microseconds
    long toMicroseconds() const;

    // Static conversion helpers
    long secondsToMilliseconds(double sec);
    long secondsToMicroseconds(double sec);

private:
    int sec_;
    unsigned int nsec_;
};

inline RomTime::RomTime() : sec_(0), nsec_(0) {}

inline RomTime::RomTime(int sec, unsigned int nsec) : sec_(sec), nsec_(nsec) {}

inline int RomTime::seconds() const 
{
    return sec_;
}

inline unsigned int RomTime::nanoseconds() const 
{
    return nsec_;
}

// Convert to milliseconds
inline long RomTime::toMilliseconds() const 
{
    return static_cast<long>(sec_) * 1000 + nsec_ / 1000000;
}

// Convert to microseconds
inline long RomTime::toMicroseconds() const 
{
    return static_cast<long>(sec_) * 1000000 + nsec_ / 1000;
}

// Static conversion helpers
inline long RomTime::secondsToMilliseconds(double sec) 
{
    return static_cast<long>(sec * 1000.0);
}
inline long RomTime::secondsToMicroseconds(double sec) 
{
    return static_cast<long>(sec * 1000000.0);
}

}

#endif // ROM_TIME_H
