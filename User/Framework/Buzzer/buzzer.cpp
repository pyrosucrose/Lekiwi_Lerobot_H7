//
// Created by Glucose_carbide on 2026-4-15.
//

#include "buzzer.hpp"

#include <iterator>

#include "config.hpp"
#include "music_soundtrack.hpp"
#include "tim.hpp"
#include "usartio.hpp"

namespace Buzzer
{
    namespace
    {
        struct BlockNote
        {
            uint16_t pitch = 0;
            int32_t duration_ms = 0;
        };

        constexpr uint8_t MAX_TRACK_COUNT = 10;
        constexpr uint8_t MAX_BLOCK_NOTE_COUNT = 20;

        bool default_mute_trigger = true;

        constexpr Config default_config = {
            .soundtrack = Soundtracks::soundtrack_mute,
            .track_len = std::size(Soundtracks::soundtrack_mute),
            .priority = 0,
            .trigger = &default_mute_trigger,
            .active_trigger_state = true,
            .restart_when_interrupted = false
        };

        cTim tim(&htim12,TIM_CHANNEL_2);
        BlockNote block_notes[MAX_BLOCK_NOTE_COUNT];
        uint8_t block_play_idx = 0;
        uint8_t block_write_idx = 0;
        MusicEvent tracks[MAX_TRACK_COUNT] = {};
        uint8_t track_count = 1;

        void Mute()
        {
            tim.ChangeDuty(0);
        }

        void SetTone(const uint16_t freq, const uint8_t duty = 50)
        {
            if (freq != 0)
            {
                tim.ChangeFreqDuty(freq, duty);
            }
            else
                Mute();
        }

        void UpdateOutput()
        {
            uint16_t pitch_now;
            uint8_t duty_now;

            if (block_play_idx != block_write_idx ||
                (block_play_idx == block_write_idx && block_notes[block_play_idx].duration_ms > 0))
            {
                pitch_now = block_notes[block_play_idx].pitch;
                duty_now = 50;

                block_notes[block_play_idx].duration_ms -= TASK_BUZZER_TASK_PERIOD;

                if (block_notes[block_play_idx].duration_ms <= 0)
                {
                    block_notes[block_play_idx].pitch = 0;
                    block_play_idx++; block_play_idx %= MAX_BLOCK_NOTE_COUNT;
                }
                // usart_printf("%d,%d,%d,%d\n",block_play_idx,block_write_idx,block_notes[block_play_idx].pitch,block_notes[block_play_idx].duration_ms);
            }
            else
            {
                uint8_t best_track = 0;
                uint8_t best_prio = 0;
                for (uint8_t i = 0; i < track_count; i++)
                {
                    if (tracks[i].isTriggered() && tracks[i].getPriority() > best_prio)
                    {
                        best_prio = tracks[i].getPriority();
                        best_track = i;
                    }
                }

                // logger_printf("%d,%d,%d,%d\n",best_track,tracks[1].isTriggered(),tracks[2].isTriggered(),tracks[3].isTriggered());

                for (uint8_t i = 0; i < track_count; i++)
                {
                    if (i != best_track)
                        tracks[i].reload();
                    else
                        tracks[i].update();
                }

                pitch_now = tracks[best_track].getPitch();
                duty_now = tracks[best_track].getDuty();
            }
            SetTone(pitch_now, duty_now);
        }
    }

    void Init()
    {
        tim.Init();
        SetTone(2000); // 在buzzer::loop开始前会一直响 // 妙(板)一直响!
        AddToNoteTrack(do_6, 80);
        AddToNoteTrack(mute, 750);
        AddToNoteTrack(do_5, 60);
        AddToNoteTrack(fa_5, 60);
        AddToNoteTrack(fa_6, 60);
        AddToNoteTrack(lap5, 60);
        AddToNoteTrack(so_6, 60);
        AddToNoteTrack(fa_5, 60);
        AddToNoteTrack(lap6, 60);
        AddToNoteTrack(fa_5, 60);
        AddToNoteTrack(do_7, 60);
        AddToNoteTrack(lap5, 60);
        AddToNoteTrack(do_6, 60);
        AddToNoteTrack(mute, 300);
        tracks[0] = MusicEvent(default_config);
    }

    void SingBlock(const uint16_t pitch, const uint16_t duration_ms)
    {
        SetTone(pitch);
        HAL_Delay(duration_ms);
        Mute();
    }

    bool AddToNoteTrack(const uint16_t pitch, const uint16_t duration_ms)
    {
        // block音轨满则不再添加
        if ((block_write_idx + 1) % MAX_BLOCK_NOTE_COUNT == block_play_idx)
            return false;

        block_write_idx++; block_write_idx %= MAX_BLOCK_NOTE_COUNT;
        block_notes[block_write_idx].pitch = pitch;
        block_notes[block_write_idx].duration_ms = static_cast<int16_t>(duration_ms);
        return true;
    }

    bool AddConfig(const Config& config)
    {
        if (track_count >= MAX_TRACK_COUNT)
            return false;

        tracks[track_count] = MusicEvent(config);
        track_count++;
        return true;
    }

    __weak void UpdateTriggers()
    {
    }

    void ControlLoop()
    {
        UpdateTriggers();
        UpdateOutput();
    }
} // namespace ega::Buzzer
