/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MODUL_TIMER_H
#define MODUL_TIMER_H

#include <chrono>
#include <standard/Platform/Common.h>

using namespace std::chrono;
namespace System
{
    namespace Time
    {
        inline uint64_t TimeMS()
        {
            return uint64(time_point_cast<std::chrono::milliseconds>(steady_clock::now()).time_since_epoch().count());
        }

        inline uint64_t TimeUS()
        {
            return uint64(time_point_cast<std::chrono::nanoseconds>(steady_clock::now()).time_since_epoch().count());
        }

        inline uint64_t getNanoTime()
        {
            static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

            return uint32(duration_cast<nanoseconds>(steady_clock::now() - ApplicationStartTime).count());
        }

        inline uint32 getMSTime()
        {
            static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

            return uint32(duration_cast<milliseconds>(steady_clock::now() - ApplicationStartTime).count());
        }

        TC_CFRAMEWORK_API uint32 getMSTimeInScripts();

        inline uint32 getMSTimeDiff(uint32 oldMSTime, uint32 newMSTime)
        {
            // getMSTime() have limited data range and this is case when it overflow in this tick
            if (oldMSTime > newMSTime)
                return (static_cast<uint32>(0xFFFFFFFF) - oldMSTime) + newMSTime;
            else
                return newMSTime - oldMSTime;
        }


        inline int32 getMSTimeSignedDiff(uint32 oldMSTime, uint32 newMSTime)
        {
            // getMSTime() have limited data range and this is case when it overflow in this tick
            if (oldMSTime > newMSTime)
                return (static_cast<uint32>(0xFFFFFFFF) - oldMSTime) + newMSTime;
            else
                return newMSTime - oldMSTime;
        }

        inline uint32 GetMSTimeDiffToNow(uint32 oldMSTime)
        {
            return getMSTimeDiff(oldMSTime, getMSTime());
        }

        inline bool GetMSTimeElapsed(uint32 MSTime)
        {
            return getMSTimeSignedDiff(MSTime, getMSTime()) > -1;
        }

        class IntervalTimer
        {
        public:
            IntervalTimer() : _interval(0), _current(0) {}

            void Update(time_t diff)
            {
                _current += diff;
                if (_current < 0)
                    _current = 0;
            }
            bool Passed() const { return _current >= _interval; }
            void Reset()
            {
                if (_current >= _interval)
                    _current -= _interval;
            }

            void SetCurrent(time_t current) { _current = current; }
            void SetInterval(time_t interval) { _interval = interval; }
            time_t GetInterval() const { return _interval; }
            time_t GetCurrent() const { return _current; }

        private:
            time_t _interval;
            time_t _current;
        };

        class ShortIntervalTimer
        {
        public:
            ShortIntervalTimer() : _interval(0), _current(0) {}

            void Update(uint32 diff)
            {
                _current += diff;
            }

            bool Passed() const { return _current >= _interval; }
            void Reset()
            {
                if (_current >= _interval)
                    _current -= _interval;
            }

            void SetCurrent(uint32 current) { _current = current; }
            void SetInterval(uint32 interval) { _interval = interval; }
            uint32 GetInterval() const { return _interval; }
            uint32 GetCurrent() const { return _current; }

        private:
            uint32 _interval;
            uint32 _current;
        };

        struct TimeTracker
        {
        public:
            TimeTracker(time_t expiry) : i_expiryTime(expiry) {}
            void Update(time_t diff) { i_expiryTime -= diff; }
            bool Passed() const { return (i_expiryTime <= 0); }
            void Reset(time_t interval) { i_expiryTime = interval; }
            time_t GetExpiry() const { return i_expiryTime; }

        private:
            time_t i_expiryTime;
        };

        struct ShortTimeTracker
        {
        public:
            ShortTimeTracker(int32 expiry) : i_expiryTime(expiry) {}
            void Update(int32 diff) { i_expiryTime -= diff; }
            bool Passed() const { return (i_expiryTime <= 0); }
            void Reset(int32 interval) { i_expiryTime = interval; }
            int32 GetExpiry() const { return i_expiryTime; }

        private:
            int32 i_expiryTime;
        };

        struct ExpireTimer
        {
        public:
            ExpireTimer() : i_expiryTime(0), Expired(true) {}
            ExpireTimer(uint32 interval) : i_expiryTime(TimeMS() + interval), Expired(false) {}
            inline bool Passed() const
            {
                if (Expired)
                    return true;
                if (TimeMS() >= i_expiryTime)
                    Expired = true;
                return Expired;
            }
            inline void Reset(uint32 interval) { Expired = false; i_expiryTime = TimeMS() + interval; }
            inline uint64_t GetExpiry() const { return i_expiryTime; }

        private:
            uint64_t i_expiryTime;
            mutable bool Expired;
        };

        class TC_CFRAMEWORK_API CooldownTimer
        {
        public:
            explicit CooldownTimer() : m_timer(0), m_interval(0) {}
            explicit CooldownTimer(int32 interval) : m_timer(interval), m_interval(interval) {}
            inline void Initialize(uint32 interval) { m_timer = m_interval = (int)interval; }
            void operator = (uint32 interval) { Initialize(interval); }
            inline void Reset() { m_timer = m_interval; }
            inline bool Passed() const { return(m_timer == 0); }
            inline bool Update(uint32 diff)
            {
                if (m_timer <= (int)diff)
                {
                    Reset();
                    return true;
                }

                m_timer -= (int)diff;
                return false;
            }

            inline bool NoResetUpdate(uint32 diff)
            {
                if (m_timer <= (int)diff)
                {
                    if (m_timer != 0)
                        m_timer = 0;
                    return true;
                }

                m_timer -= (int)diff;
                return false;
            }

            int32 m_timer;
            int32 m_interval;
        };

        struct TC_CFRAMEWORK_API RealTimeDiffer
        {
            RealTimeDiffer() : m_lasttimer(time(NULL)) {}
            inline void Reset() { m_lasttimer = time(NULL); }
            inline time_t GetDiff() { return (time(NULL) - m_lasttimer); }
            inline uint32 GetDiffAndReset()
            {
                uint32 t = uint32(GetDiff());
                Reset();
                return t;
            }
            time_t m_lasttimer;
        };

        struct TC_CFRAMEWORK_API NanoTimeStopper
        {
            NanoTimeStopper(const char* name) : Name(name), startTime(0), diff(0) {}

            std::string Name;
            uint64_t startTime;
            uint64_t diff;

            void Start() { startTime = getNanoTime(); }
            void Stop() { diff = getNanoTime() - startTime; }

            void Print() const
            {
                //printf("%s  "UI64FMTD"."UI64FMTD" \n\n\n\n", Name.c_str(), (uint64)(diff / 1000), (uint64)(diff - (diff / 1000)));
            }

            void WaitForKeyBoard()
            {
            }
        };


        struct MSTime
        {
        private:
            enum {
                UInt32Max = uint32(0xFFFFFFFF),
            };
            static uint32 decrease(const uint32& value, const uint32& decrement)
            {
                if (decrement > value)
                    return (UInt32Max - decrement) + value;
                else
                    return value - decrement;
            }

            static uint32 increase(const uint32& value, const uint32& increment)
            {
                if (value < (UInt32Max - increment))
                    return value + increment;
                else
                    return increment - (UInt32Max - value);
            }

        public:
            uint32 time;

            MSTime(const uint32& t) : time(t) {}
            MSTime() : time(0) {}

            MSTime operator + (const MSTime& t) const { return MSTime(increase(time, t.time)); }
            MSTime operator - (const MSTime& t) const { return MSTime(decrease(time, t.time)); }

            void operator = (const MSTime& t) { time = t.time; }
            void operator = (const uint32& t) { time = t; }
            void operator += (const MSTime& t) { *this = *this + t; }
            void operator -= (const MSTime& t) { *this = *this - t; }

            bool operator != (const MSTime& t) const { return time != t.time; }
            bool operator == (const MSTime& t) const { return time == t.time; }

            bool operator < (const MSTime& t) const { return time < t.time; }
            bool operator > (const MSTime& t) const { return time > t.time; }
            bool operator <= (const MSTime& t) const { return time <= t.time; }
            bool operator >= (const MSTime& t) const { return time >= t.time; }

            uint32 ToUInt32() const { return time; }
        };
    };
};
#endif
